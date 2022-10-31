#include "huedevice.h"

#include <QJsonArray>

#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

HueDevice::HueDevice(int id, QObject* parent) : QObject(parent), id_(id), isReachable_(false), isOn_(false) {
    qDebug() << "Created Hue device with ID: " << id_;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueDevice::setRoom(const QString& value) {
    if (room_ != value) {
        room_ = value;
        emit roomChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueDevice::commandPower(const bool on) {
    VCHub::instance()->hue()->commandDeviceState(id_, QJsonObject{{"on", on}});
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueDevice::handleResponse(const QJsonDocument& response) {
    // Is this a direct reply to a command?
    if (response.isArray()) {
        // Yes, unpack the response structure.
        // BDP: There are a few layers to unpack here, but it seems to be to support multiple replies in the same
        //      message. We will only be sending one at a time, but still support multiple.
        QJsonObject state;
        const QJsonArray responseArray = response.array();
        for (const auto& responseItem : responseArray) {
            QJsonObject responseObject = responseItem.toObject();
            if (responseObject.contains("success")) {
                QJsonObject successObject = responseObject.value("success").toObject();
                const QStringList arguments = successObject.keys();
                for (const auto& argument : arguments) {
                    // The reply is identified by the API endpoint portion at the end of the URL.
                    if (argument.startsWith(QString("/lights/%1/state/").arg(id_))) {
                        // The final token will be the state property that was set.
                        QStringList parts = argument.split(QChar('/'));
                        state.insert(parts.last(), successObject.value(argument));
                    } else {
                        qDebug() << "Got unexpected argument when handling response for Hue device: " << id_;
                    }
                }
            } else {
                qDebug() << "Received error when handling response for Hue device: " << id_;
            }
        }

        // Process any collected data, providing it in the expected structure.
        handleResponseData(QJsonObject{{"state", state}});
    } else if (response.isObject()) {
        // No, pass through.
        handleResponseData(response.object());
    } else {
        qDebug() << "Failed to parse response for Hue device: " << id_;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueDevice::handleStateData(const QJsonObject& state) {
    if (state.contains("reachable")) {
        bool reachable = state.value("reachable").toBool();
        if (isReachable_ != reachable) {
            isReachable_ = reachable;
            emit isReachableChanged();
        }
    }
    if (state.contains("on")) {
        bool on = state.value("on").toBool();
        if (isOn_ != on) {
            isOn_ = on;
            emit isOnChanged();
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueDevice::handleResponseData(const QJsonObject& response) {
    // Process top-level values.
    if (response.contains("name")) {
        QString name = response.value("name").toString();
        if (name_ != name) {
            name_ = name;
            emit nameChanged();
        }
    }
    if (response.contains("type")) {
        QString type = response.value("type").toString();
        if (type_ != type) {
            type_ = type;
            emit typeChanged();
        }
    }
    if (response.contains("productname")) {
        QString productName = response.value("productname").toString();
        if (productName_ != productName) {
            productName_ = productName;
            emit productNameChanged();
        }
    }

    // Unpack and process state information.
    handleStateData(response.value("state").toObject());
}
/*--------------------------------------------------------------------------------------------------------------------*/
