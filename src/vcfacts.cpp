#include "vcfacts.h"

#include <QJsonObject>

#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

VCFacts::VCFacts(const QString& name, QObject* parent)
    : VCPlugin(name, parent),
      requestURL_(QString("https://uselessfacts.jsph.pl/random.json?language=%1").arg(QLocale::system().bcp47Name())) {
    setUpdateInterval(60 * 1000);

    // Handle network responses.
    connect(NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCFacts::handleNetworkReply);

    refresh();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCFacts::refresh() {
    NetworkInterface::instance()->sendJSONRequest(requestURL_, this);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCFacts::handleNetworkReply(const int statusCode, QObject* const sender, const QJsonDocument& body) {
    if (sender != this) {
        // Not for us, ignore.
        return;
    }
    if (statusCode != 200) {
        qDebug() << "Ignoring bad reply when requesting fact";
        return;
    }
    if (!body.isObject()) {
        qDebug() << "Failed to parse facts response";
        return;
    }

    QJsonObject responseObject = body.object();
    if (responseObject.contains("text")) {
        QString fact = responseObject.value("text").toString().simplified().replace('`', '\'');
        if (!fact.isEmpty()) {
            fact_ = fact;
            emit factChanged();
        } else {
            qDebug() << "No fact received in reply";
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
