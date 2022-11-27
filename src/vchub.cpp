#include "vchub.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QLocale>
#include <QNetworkInterface>
#include <QStandardPaths>
#include <QSysInfo>

#include "hueambiancelight.h"
#include "huecolorlight.h"
#include "huelight.h"
#include "networkinterface.h"
#include "vcconfig.h"
/*--------------------------------------------------------------------------------------------------------------------*/

namespace {
VCHub* instance_ = nullptr;
}  // namespace
/*--------------------------------------------------------------------------------------------------------------------*/

VCHub::VCHub(QObject* parent)
    : QObject(parent),
      isActive_(true),
      currentDateTime_(QDateTime::currentDateTime()),
      hostname_(QHostInfo::localHostName()),
      platform_(QSysInfo::prettyProductName().split('(').first().trimmed()),
      architecture_(QSysInfo::currentCpuArchitecture()),
      qtVersion_(qVersion()),
      use24HourClock_(false),
      darkerBackground_(false),
      screensaverEnabled_(true),
      hue_(new VCHue("Hue", this)),
      nanoleaf_(new VCNanoleaf("Nanoleaf", this)),
      pihole_(new VCPiHole("PiHole", this)),
      weather_(new VCWeather("Weather", this)),
      facts_(new VCFacts("Facts", this)),
      spotify_(new VCSpotify("Spotify", this)),
      isRunningScene_(false) {
    setObjectName("Hub");
    qDebug() << "Initializing dashboard hub";

    // Take ownership of the network interface.
    NetworkInterface::instance()->setParent(this);

#ifdef QT_DEBUG
    // Reload the config file if it changes externally.
    connect(&configFileWatcher_, &QFileSystemWatcher::fileChanged, this, [this](const QString& path) {
        qDebug() << "Reloading config file because it has changed externally";
        (void)loadConfig(path);
    });
#endif

    // Periodically refresh the current date and time.
    currentDateTimeRefreshTimer_.setInterval(10 * 1000);
    currentDateTimeRefreshTimer_.setSingleShot(false);
    connect(&currentDateTimeRefreshTimer_, &QTimer::timeout, this, &VCHub::updateCurrentDateTime);
    currentDateTimeRefreshTimer_.start();

    // Periodically refresh the IP addresses.
    ipAddressesRefreshTimer_.setInterval(15 * 1000);
    ipAddressesRefreshTimer_.setSingleShot(false);
    connect(&ipAddressesRefreshTimer_, &QTimer::timeout, this, &VCHub::refreshIPAddresses);
    ipAddressesRefreshTimer_.start();
    refreshIPAddresses();
}
/*--------------------------------------------------------------------------------------------------------------------*/

VCHub* VCHub::instance() {
    if (!instance_) {
        instance_ = new VCHub();
    }

    return instance_;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::setActive(const bool value) {
    static const QList<VCPlugin*> plugins{hue_, nanoleaf_, pihole_, weather_, facts_, spotify_};

    if (isActive_ != value) {
        isActive_ = value;
        emit isActiveChanged();

        // Propagate the active state to each plugin.
        for (auto plugin : plugins) {
            plugin->setActive(value);
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::setUse24HourClock(const bool value) {
    if (use24HourClock_ != value) {
        use24HourClock_ = value;
        emit use24HourClockChanged();

        // Update the time display right away.
        updateCurrentDateTime();

        // Indicate that any QDateTime properties should be reevaluated by emitting the NOTIFY signal.
        const QObjectList childrenList = children();
        for (auto child : childrenList) {
            const QMetaObject* childMeta = child->metaObject();
            int propertyCount = childMeta->propertyCount();
            for (int i = 0; i < propertyCount; i++) {
                QMetaProperty property = childMeta->property(i);
                if ((property.type() == QVariant::DateTime) && property.hasNotifySignal()) {
                    property.notifySignal().invoke(child);
                }
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

bool VCHub::loadConfig(const QString& path) {
    bool success = VCConfig::instance()->load(path);

    if (success) {
        // Translate the home map path if it is relative.
        if (!homeMap_.isEmpty() && QFileInfo(homeMap_).isRelative()) {
            // Assume the same directory as the config file.
            homeMap_ = QFileInfo(path).dir().filePath(homeMap_);
            emit homeMapChanged();
        }

#ifdef QT_DEBUG
        // Watch the config file on disk.
        if (configFileWatcher_.files().isEmpty()) {
            configFileWatcher_.addPath(path);
        }
#endif
    }

    return success;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::runScene(const QString& scene) {
    QVariantList steps = extractSceneSteps(scene);
    if (steps.isEmpty()) {
        // No steps, assume this means the scene was not found.
        qDebug() << "Ignoring request to run unknown scene: " << scene;
        return;
    }

    // Indicate that execution is starting.
    isRunningScene_ = true;
    emit isRunningSceneChanged();

    // Scenes steps are constructed as a list of objects that contain device and state information.
    qDebug() << "Processing scene " << scene << " with " << steps.size() << " steps";
    for (int i = 0; i < steps.size(); i++) {
        int stepNumber = i + 1;
        QVariantMap step = steps.at(i).toMap();

        // Ensure the expected structure is present.
        if (step.contains("device") && step.contains("state")) {
            QVariantMap device = step.value("device").toMap();
            QString name = device.value("name").toString();
            QString className = device.value("class").toString();
            QVariantMap state = step.value("state").toMap();

            // Execute the actions of the step based on the device class.
            if (className == "hue") {
                // Locate the Hue device by name.
                const QList<HueDevice*>& hueDevices = hue_->devices();
                HueDevice* hueDevice = nullptr;
                for (auto device : hueDevices) {
                    if (name == device->name()) {
                        hueDevice = device;
                        break;
                    }
                }

                // Check if the device has been discovered.
                if (hueDevice) {
                    qDebug() << "Executing step " << stepNumber << " on Hue device: " << name;

                    // Apply state properties from most to least generic.
                    if (state.contains("on")) {
                        qDebug() << "\t=> Command power";
                        hueDevice->commandPower(state.take("on").toBool());
                    }
                    if (state.contains("brightness")) {
                        // This must be a light.
                        auto hueLight = qobject_cast<HueLight*>(hueDevice);
                        if (hueLight) {
                            qDebug() << "\t=> Command brightness";
                            hueLight->commandBrightness(state.take("brightness").toDouble());
                        } else {
                            state.remove("brightness");  // Still consume the value
                            qDebug() << "Encountered brightness command for Hue device " << name
                                     << " that is not a light in step " << stepNumber
                                     << " when processing scene: " << scene;
                        }
                    }
                    if (state.contains("colorTemperature")) {
                        // This must be an ambiance light.
                        auto hueLight = qobject_cast<HueAmbianceLight*>(hueDevice);
                        if (hueLight) {
                            qDebug() << "\t=> Command color temperature";
                            hueLight->commandColorTemperature(state.take("colorTemperature").toInt());
                        } else {
                            state.remove("colorTemperature");  // Still consume the value
                            qDebug() << "Encountered color temperature command for Hue device " << name
                                     << " that is not an ambiance light in step " << stepNumber
                                     << " when processing scene: " << scene;
                        }
                    }
                    if (state.contains("xy")) {
                        // This must be a color light.
                        auto hueLight = qobject_cast<HueColorLight*>(hueDevice);
                        if (hueLight) {
                            qDebug() << "\t=> Command XY color";
                            QVariantList xy = state.take("xy").toList();
                            hueLight->commandColor(xy.at(0).toDouble(), xy.at(1).toDouble());
                        } else {
                            state.remove("xy");  // Still consume the value
                            qDebug() << "Encountered XY color command for Hue device " << name
                                     << " that is not a color light in step " << stepNumber
                                     << " when processing scene: " << scene;
                        }
                    }
                    if (state.contains("hue")) {
                        // This must be a color light.
                        auto hueLight = qobject_cast<HueColorLight*>(hueDevice);
                        if (hueLight) {
                            qDebug() << "\t=> Command hue color";
                            hueLight->commandColor(state.take("hue").toInt());
                        } else {
                            state.remove("hue");  // Still consume the value
                            qDebug() << "Encountered hue color command for Hue device " << name
                                     << " that is not a color light in step " << stepNumber
                                     << " when processing scene: " << scene;
                        }
                    }
                    if (!state.isEmpty()) {
                        qDebug() << "Detected unsupported state properties " << state.keys() << " for Hue device "
                                 << name << " in step " << stepNumber << " when processing scene: " << scene;
                    }
                } else {
                    qDebug() << "Encountered unknown Hue device name " << name << " in step " << stepNumber
                             << " when processing scene: " << scene;
                }
            } else if (className == "nanoleaf") {
                // Ensure this is the discovered Nanoleaf.
                if (name == nanoleaf_->name()) {
                    qDebug() << "Executing step " << stepNumber << " on Nanoleaf: " << name;
                    if (state.contains("on")) {
                        qDebug() << "\t=> Command power";
                        nanoleaf_->commandPower(state.take("on").toBool());
                    }
                    if (state.contains("effect")) {
                        qDebug() << "\t=> Select effect";
                        nanoleaf_->selectEffect(state.take("effect").toString());
                    }
                    if (!state.isEmpty()) {
                        qDebug() << "Detected unsupported state properties " << state.keys() << " for Nanoleaf " << name
                                 << " in step " << stepNumber << " when processing scene: " << scene;
                    }
                } else {
                    qDebug() << "Encountered unknown Nanoleaf name " << name << " in step " << stepNumber
                             << " when processing scene: " << scene;
                }
            } else {
                qDebug() << "Encountered unsupported class " << className << " in step " << stepNumber
                         << " when processing scene: " << scene;
            }
        } else {
            qDebug() << "Missing device and/or state for step " << stepNumber << " in scene: " << scene;
        }

        // Insert a brief pause in between steps to prevent overloading the devices.
        QTime future = QTime::currentTime().addMSecs(200);
        while (future > QTime::currentTime()) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
        }
    }

    qDebug() << "Finished processing scene: " << scene;
    isRunningScene_ = false;
    emit isRunningSceneChanged();
}
/*--------------------------------------------------------------------------------------------------------------------*/

QStringList VCHub::parseSceneColors(const QString& scene) {
    QStringList colors;

    // Pull any called-out colors from the step states.
    const QVariantList steps = extractSceneSteps(scene);
    if (!steps.isEmpty()) {
        for (const auto& step : steps) {
            QVariantMap state = step.toMap().value("state").toMap();
            if (state.contains("xy")) {
                QVariantList xy = state.value("xy").toList();
                if (xy.size() == 2) {
                    QColor color = HueColorLight::xyToColor(xy.at(0).toDouble(), xy.at(1).toDouble());
                    if (color.isValid()) {
                        colors.append(color.name());
                    }
                }
            }
            if (state.contains("hue")) {
                QColor color = HueColorLight::hueToColor(state.value("hue").toInt());
                if (color.isValid()) {
                    colors.append(color.name());
                }
            }
        }
    }

    // Sort the list alphabetically so duplicate colors are adjacent.
    if (!colors.isEmpty()) {
        std::sort(colors.begin(), colors.end());
    }

    return colors;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::dayOfWeek(const QDateTime& dateTime) const {
    return dateTime.toString("dddd");
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatTime(const QDateTime& dateTime) const {
    return dateTime.toString(use24HourClock() ? "hh:mm" : "h:mm AP");
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatInt(int value, const QString& unit) const {
    QString display = QLocale::system().toString(value);
    if (!unit.isEmpty()) {
        display.append(QString(" %1").arg(unit));
    }
    return display;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatDecimal(double dValue, const QString& Unit) const {
    QString Display = QLocale::system().toString(dValue, 'f', 1);  // 1 decimal place
    if (!Unit.isEmpty()) {
        Display.append(QString(" %1").arg(Unit));
    }
    return Display;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatPercentage(double value, bool wholeNumber) const {
    return QString("%1%").arg(QLocale::system().toString(value, 'f', wholeNumber ? 0 : 1));
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::screenshotPath() const {
    QString format = QString("yyyy-MM-dd %1").arg(use24HourClock() ? "hh.mm.ss" : "h.mm.ss AP");
    QString filename = QString("VC Screenshot %1.png").arg(QDateTime::currentDateTime().toString(format));
    return QDir(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).absoluteFilePath(filename);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::updateCurrentDateTime() {
    currentDateTime_ = QDateTime::currentDateTime();
    emit currentDateTimeChanged();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::refreshIPAddresses() {
    // Examine all of the system network interfaces and any associated IP addresses they may have.
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const auto& interface : interfaces) {
        // Avoid loopback, P2P, or otherwise virtual interfaces.
        if ((interface.type() == QNetworkInterface::Ethernet) || (interface.type() == QNetworkInterface::Wifi)) {
            const QList<QNetworkAddressEntry> addressEntries = interface.addressEntries();
            for (const auto& addressEntry : addressEntries) {
                QHostAddress address = addressEntry.ip();
                if ((address.protocol() == QAbstractSocket::IPv4Protocol) && !address.isLoopback()) {
                    QString ipAddress = address.toString();
                    if ((interface.type() == QNetworkInterface::Ethernet) && (ethernetIPAddress_ != ipAddress)) {
                        ethernetIPAddress_ = ipAddress;
                        emit ethernetIPAddressChanged();
                    } else if ((interface.type() == QNetworkInterface::Wifi) && (ethernetIPAddress_ != ipAddress)) {
                        wifiIPAddress_ = ipAddress;
                        emit wifiIPAddressChanged();
                    }
                }
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

QVariantList VCHub::extractSceneSteps(const QString& scene) {
    QVariantList steps;

    // Find the scene in the list and extract its steps.
    for (const auto& sceneItem : qAsConst(scenes_)) {
        QVariantMap sceneMap = sceneItem.toMap();
        QString name = sceneMap.value("name").toString();
        if (!name.isEmpty() && (scene == name)) {
            steps = sceneMap.value("steps").toList();
            break;
        }
    }

    return steps;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QObject* vchub_singletontype_provider(QQmlEngine* engine, QJSEngine* scriptEngine) {
    (void)engine;
    (void)scriptEngine;

    return VCHub::instance();
}
/*--------------------------------------------------------------------------------------------------------------------*/
