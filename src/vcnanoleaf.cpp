#include "vcnanoleaf.h"

#include <QColor>
#include <QJsonArray>
#include <QJsonObject>

#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

namespace {
constexpr const char* NANOLEAF_SERVICE_TYPE = "_nanoleafapi._tcp";
}  // namespace
/*--------------------------------------------------------------------------------------------------------------------*/

VCNanoleaf::VCNanoleaf(const QString& name, QObject* parent)
    : VCPlugin(name, parent), isOn_(false), commandedPower_(-1) {
    // Don't start refreshing until the Nanoleaf has been found.
    updateTimer_.stop();
    setUpdateInterval(3 * 1000);

    // Handle network responses.
    connect(NetworkInterface::instance(),
            &NetworkInterface::zeroConfServiceFound,
            this,
            &VCNanoleaf::handleZeroConfServiceFound);
    connect(NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCNanoleaf::handleNetworkReply);

    // Update the base URL whenever dependent properties change.
    connect(this, &VCNanoleaf::ipAddressChanged, this, &VCNanoleaf::updateBaseURL);
    connect(this, &VCNanoleaf::authTokenChanged, this, &VCNanoleaf::updateBaseURL);

    // Look for the Nanoleaf.
    NetworkInterface::instance()->browseZeroConf(NANOLEAF_SERVICE_TYPE);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::refresh() {
    NetworkInterface::instance()->sendJSONRequest(QUrl(baseURL_), this);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::commandPower(const bool on) {
    QJsonObject command{{"on", QJsonObject{{"value", on}}}};
    QUrl destination(QString("%1/state").arg(baseURL_));

    // Assume the command will succeed.
    commandedPower_ = on ? 1 : 0;
    isOn_ = on;
    emit isOnChanged();

    NetworkInterface::instance()->sendJSONRequest(
        destination, this, QNetworkAccessManager::PutOperation, QJsonDocument(command));
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::selectEffect(const QString& effect) {
    QJsonObject command{{"select", effect}};
    QUrl destination(QString("%1/effects").arg(baseURL_));

    // Assume the command will succeed.
    commandedEffect_ = effect;
    selectedEffect_ = effect;
    emit selectedEffectChanged();

    NetworkInterface::instance()->sendJSONRequest(
        destination, this, QNetworkAccessManager::PutOperation, QJsonDocument(command));
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::refreshEffects() {
    QUrl destination(QString("%1/effects").arg(baseURL_));
    QJsonObject write{{"command", "requestAll"}};
    QJsonObject body{{"write", write}};

    // BDP: A put request containing a command to the write endpoint is actually just a query? Really?
    NetworkInterface::instance()->sendJSONRequest(
        destination, this, QNetworkAccessManager::PutOperation, QJsonDocument(body));
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::handleZeroConfServiceFound(const QString& serviceType, const QString& ipAddress) {
    if (ipAddress_.isEmpty() && serviceType.startsWith(NANOLEAF_SERVICE_TYPE)) {
        ipAddress_ = ipAddress;
        qDebug() << "Nanoleaf found at IP address: " << ipAddress_;
        emit ipAddressChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::handleNetworkReply(int statusCode, QObject* sender, const QJsonDocument& body) {
    if (this == sender) {
        if (200 == statusCode) {
            if (body.isObject()) {
                QJsonObject responseObject = body.object();
                if (responseObject.contains("name")) {
                    QString name = responseObject.value("name").toString();
                    if (name_ != name) {
                        name_ = name;
                        emit nameChanged();
                    }
                }
                if (responseObject.contains("effects")) {
                    QJsonObject effectsObject = responseObject.value("effects").toObject();
                    if (effectsObject.contains("select")) {
                        QString selected = effectsObject.value("select").toString();

                        // BDP: Ensure the commanded effect is applied, which seems to possibly take more than one try
                        //      if the Nanoleaf has been sitting idle for a while.
                        if (!commandedEffect_.isEmpty() && (commandedEffect_ != selected)) {
                            // Command the effect again after a short delay.
                            // TODO(BDP): Give up after a certain number of attempts?
                            QTimer::singleShot(500, this, [this] { selectEffect(commandedEffect_); });
                        } else {
                            commandedEffect_.clear();

                            if (selectedEffect_ != selected) {
                                selectedEffect_ = selected;
                                emit selectedEffectChanged();
                            }
                        }
                    }
                }
                if (responseObject.contains("state")) {
                    QJsonObject stateObject = responseObject.value("state").toObject();
                    if (stateObject.contains("on")) {
                        QJsonObject onObject = stateObject.value("on").toObject();
                        if (onObject.contains("value")) {
                            bool on = onObject.value("value").toBool();

                            // BDP: Ensure the commanded power is applied.
                            if (((0 == commandedPower_) && on) || ((1 == commandedPower_) && !on)) {
                                // Command again after a short delay.
                                QTimer::singleShot(500, this, [this] { commandPower(1 == commandedPower_); });
                            } else {
                                commandedPower_ = -1;

                                if (isOn_ != on) {
                                    isOn_ = on;
                                    emit isOnChanged();
                                }
                            }
                        }
                    }
                }
                if (responseObject.contains("animations")) {
                    QVariantList effects;
                    const QJsonArray animationsArray = responseObject.value("animations").toArray();
                    for (const auto& animation : animationsArray) {
                        QJsonObject animationObject = animation.toObject();
                        if (!animationObject.isEmpty()) {
                            QVariantMap effect{{"name", animationObject.value("animName").toString()}};

                            QStringList colors;
                            const QJsonArray paletteArray = animationObject.value("palette").toArray();
                            for (const auto& palette : paletteArray) {
                                QJsonObject paletteObject = palette.toObject();
                                if (!paletteObject.isEmpty()) {
                                    double hue = paletteObject.value("hue").toInt() / 359.0;
                                    double saturation = paletteObject.value("saturation").toInt() / 100.0;
                                    double brightness = paletteObject.value("brightness").toInt() / 100.0;
                                    colors.append(QColor::fromHsvF(hue, saturation, brightness).name());
                                }
                            }
                            effect["colors"] = colors;

                            const QJsonArray optionsArray = animationObject.value("pluginOptions").toArray();
                            for (const auto& option : optionsArray) {
                                QJsonObject optionObject = option.toObject();
                                if (!optionObject.isEmpty()) {
                                    QString optionName = optionObject.value("name").toString();
                                    if (("delayTime" == optionName) || ("transTime" == optionName)) {
                                        // These values are presented in tenths of a second, convert.
                                        effect[optionName] = optionObject.value("value").toInt() / 10.0;
                                    }
                                }
                            }

                            effects.append(effect);
                        }
                    }

                    if (!effects.isEmpty()) {
                        // Sort alphabetically by name.
                        std::sort(effects.begin(), effects.end(), [](const QVariant& left, const QVariant& right) {
                            return left.toMap().value("name").toString() < right.toMap().value("name").toString();
                        });

                        if (effects_ != effects) {
                            effects_ = effects;
                            emit effectsChanged();
                        }
                    }
                }
            } else {
                qDebug() << "Failed to parse response from Nanoleaf";
            }
        } else if (204 == statusCode) {
            // Successful ACK of effect selection, but no content in the response.
        } else {
            qDebug() << "Ignoring unsuccessful reply from Nanoleaf with status code: " << statusCode;
        }
    } else {
        // Not for us, ignore.
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::updateBaseURL() {
    if (!ipAddress_.isEmpty() && !authToken_.isEmpty()) {
        baseURL_ = QString("http://%1:16021/api/v1/%2").arg(ipAddress_, authToken_);

        // With the IP address known, start the update timer and refesh immediately.
        updateTimer_.start();
        refresh();
        refreshEffects();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
