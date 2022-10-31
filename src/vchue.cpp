#include "vchue.h"

#include <QJsonArray>

#include "hueambiancelight.h"
#include "huecolorlight.h"
#include "huelight.h"
#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

namespace {
constexpr const char* HUE_SERVICE_TYPE = "_hue._tcp";
}  // namespace
/*--------------------------------------------------------------------------------------------------------------------*/

VCHue::VCHue(const QString& name, QObject* parent) : VCPlugin(name, parent) {
    // Don't start refreshing until the Bridge has been found.
    updateTimer_.stop();
    setUpdateInterval(1000);

    // Handle network responses.
    connect(NetworkInterface::instance(),
            &NetworkInterface::zeroConfServiceFound,
            this,
            &VCHue::handleZeroConfServiceFound);
    connect(NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCHue::handleNetworkReply);

    // Update the base URL whenever dependent properties change.
    connect(this, &VCHue::bridgeIPAddressChanged, this, &VCHue::updateBaseURL);
    connect(this, &VCHue::bridgeUsernameChanged, this, &VCHue::updateBaseURL);

    // Look for the Bridge.
    NetworkInterface::instance()->browseZeroConf(HUE_SERVICE_TYPE);
}
/*--------------------------------------------------------------------------------------------------------------------*/

int VCHue::onDevicesCount() const {
    int count = 0;

    for (const HueDevice* device : qAsConst(devices_)) {
        if (device->isOn()) {
            count++;
        }
    }

    return count;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::refresh() {
    NetworkInterface::instance()->sendJSONRequest(lightsURL_, this);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::refreshGroups() {
    NetworkInterface::instance()->sendJSONRequest(groupsURL_, this);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::commandDeviceState(const int id, const QJsonObject& parameters) {
    HueDevice* device = deviceTable_.value(id, nullptr);
    if (device) {
        QUrl url(QString("%1/%2/state").arg(lightsURL_.toString()).arg(id));
        NetworkInterface::instance()->sendJSONRequest(
            url, device, QNetworkAccessManager::PutOperation, QJsonDocument(parameters));
    } else {
        qDebug() << "Ignoring request to command state of unknown device: " << id;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::handleZeroConfServiceFound(const QString& serviceType, const QString& ipAddress) {
    if (bridgeIPAddress_.isEmpty() && serviceType.startsWith(HUE_SERVICE_TYPE)) {
        bridgeIPAddress_ = ipAddress;
        qDebug() << "Hue Bridge found at IP address: " << bridgeIPAddress_;
        emit bridgeIPAddressChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::handleNetworkReply(int statusCode, QObject* sender, const QJsonDocument& body) {
    // Check if this is for us or should be dispatched to a device.
    if (this == sender) {
        if (200 == statusCode) {
            // Query response for all light information.
            if (body.isObject()) {
                QJsonObject responseObject = body.object();
                const QStringList keys = responseObject.keys();
                for (const auto& key : keys) {
                    bool ok = false;
                    int id = key.toInt(&ok);
                    if (ok) {
                        QJsonObject itemObject = responseObject.value(key).toObject();
                        if (!itemObject.isEmpty()) {
                            // Is this device information?
                            if (!itemObject.contains("lights")) {
                                // Device information, is there already a record of this device?
                                HueDevice* device = deviceTable_.value(id, nullptr);
                                if (!device) {
                                    // Inspect the device type to determine the correct object type.
                                    QString type = itemObject.value("type").toString().toLower();
                                    if ("dimmable light" == type) {
                                        device = new HueLight(id, this);
                                    } else if ("color temperature light" == type) {
                                        device = new HueAmbianceLight(id, this);
                                    } else if (type.endsWith("color light")) {
                                        device = new HueColorLight(id, this);
                                    } else {
                                        device = new HueDevice(id, this);
                                    }

                                    // Update the number of devices powered on when it changes.
                                    connect(device, &HueDevice::isOnChanged, this, &VCHue::onDevicesCountChanged);

                                    // Record the device.
                                    deviceTable_.insert(id, device);
                                    devices_.append(device);
                                    std::sort(devices_.begin(),
                                              devices_.end(),
                                              [](const HueDevice* left, const HueDevice* right) {
                                                  if (!left) {
                                                      return false;
                                                  }
                                                  if (!right) {
                                                      return true;
                                                  }
                                                  return left->id() < right->id();
                                              });
                                    emit devicesChanged();
                                }

                                // Dispatch device and state information to the device.
                                device->handleResponse(QJsonDocument(itemObject));
                            } else {
                                // Group information.
                                QString name = itemObject.value("name").toString();
                                QString type = itemObject.value("type").toString();
                                if (!name.isEmpty() && ("room" == type.toLower())) {
                                    const QJsonArray lights = itemObject.value("lights").toArray();
                                    for (const auto& light : lights) {
                                        bool ok = false;
                                        int lightID = light.toString().toInt(&ok);
                                        if (ok) {
                                            HueDevice* device = deviceTable_.value(lightID, nullptr);
                                            if (device) {
                                                // Tell the device which room it's in.
                                                device->setRoom(name);
                                            } else {
                                                // No record of the device, try again next time.
                                            }
                                        } else {
                                            qDebug() << "Got invalid light ID in groups response from Hue Bridge";
                                        }
                                    }
                                }
                            }
                        } else {
                            qDebug() << "Got empty or invalid item object in query response from Hue Bridge at key: "
                                     << key;
                        }
                    } else {
                        qDebug() << "Got invalid ID in query response from Hue Bridge";
                    }
                }
            } else {
                qDebug() << "Failed to parse query response from Hue Bridge";
            }
        } else {
            qDebug() << "Ignoring unsuccessful reply from Hue Bridge with status code: " << statusCode;
        }
    } else {
        auto device = qobject_cast<HueDevice*>(sender);
        if (device) {
            if (200 == statusCode) {
                // Dispatch to the device.
                device->handleResponse(body);
            } else {
                qDebug() << "Ignoring unsuccessful reply from Hue Bridge for device " << device->name()
                         << " with status code: " << statusCode;
            }
        } else {
            // Not for us, ignore.
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::updateBaseURL() {
    if (!bridgeIPAddress_.isEmpty() && !bridgeUsername_.isEmpty()) {
        QString baseURL = QString("http://%1/api/%2").arg(bridgeIPAddress_, bridgeUsername_);
        lightsURL_ = QUrl(QString("%1/lights").arg(baseURL));
        groupsURL_ = QUrl(QString("%1/groups").arg(baseURL));

        // With the IP address known, start the update timer and refesh immediately.
        updateTimer_.start();
        refresh();
        refreshGroups();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
