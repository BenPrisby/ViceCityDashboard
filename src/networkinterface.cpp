#include "networkinterface.h"

#include <QCoreApplication>
/*--------------------------------------------------------------------------------------------------------------------*/

namespace {
constexpr const char* JSON_CONTENT_TYPE = "application/json";

NetworkInterface* instance_ = nullptr;
}  // namespace
/*--------------------------------------------------------------------------------------------------------------------*/

NetworkInterface::NetworkInterface(QObject* parent)
    : QObject(parent), manager_(new QNetworkAccessManager(this)), zeroConf_(new QZeroConf(this)) {
    setObjectName("NetworkInterface");

    connect(manager_, &QNetworkAccessManager::finished, this, &NetworkInterface::handleReply);
    connect(zeroConf_, &QZeroConf::serviceAdded, this, &NetworkInterface::handleZeroConfServiceAdded);

    // Configure a timeout on browsing for ZeroConf services.
    zeroConfBrowseTimer_.setInterval(15 * 1000);
    zeroConfBrowseTimer_.setSingleShot(true);
    connect(&zeroConfBrowseTimer_, &QTimer::timeout, this, [this] {
        QString serviceType = zeroConfBrowseRequests_.dequeue();
        qDebug() << "Failed to find ZeroConf service type: " << serviceType;
        zeroConf_->stopBrowser();

        // Are there more requests pending?
        if (!zeroConfBrowseRequests_.isEmpty()) {
            zeroConf_->startBrowser(zeroConfBrowseRequests_.front(), QAbstractSocket::IPv4Protocol);
            zeroConfBrowseTimer_.start();
        }
    });
}
/*--------------------------------------------------------------------------------------------------------------------*/

NetworkInterface* NetworkInterface::instance() {
    if (!instance_) {
        instance_ = new NetworkInterface();
    }

    return instance_;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::sendRequest(const QUrl& destination,
                                   QObject* sender,
                                   QNetworkAccessManager::Operation requestType,
                                   const QByteArray& body,
                                   const QByteArray& contentType,
                                   const QByteArray& authorization) {
    if (!destination.isValid()) {
        qDebug() << "Ignoring request with invalid URL";
        return;
    }

    QNetworkRequest request(destination);

    // Attach the application information to the request.
    static QByteArray applicationInfo =
        QString("%1 %2").arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()).toUtf8();
    request.setRawHeader("User-Agent", applicationInfo);

    // Was a sender specified for context?
    if (sender) {
        request.setOriginatingObject(sender);
    }

    // Were a body and corresponding content type supplied?
    if ((!body.isEmpty() && !contentType.isEmpty()) || (requestType == QNetworkAccessManager::PostOperation)) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    }

    // Was an authorization token supplied?
    if (!authorization.isEmpty()) {
        request.setRawHeader("Authorization", authorization);
    }

    switch (requestType) {
        case QNetworkAccessManager::GetOperation:
            manager_->get(request);
            break;

        case QNetworkAccessManager::PostOperation:
            manager_->post(request, body);
            break;

        case QNetworkAccessManager::PutOperation:
            manager_->put(request, body);
            break;

        case QNetworkAccessManager::DeleteOperation:
            manager_->deleteResource(request);
            break;

        default:
            qDebug() << "Ignoring unsupported request type";
            break;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::sendJSONRequest(const QUrl& destination,
                                       QObject* sender,
                                       QNetworkAccessManager::Operation requestType,
                                       const QJsonDocument& body,
                                       const QByteArray& authorization) {
    sendRequest(
        destination, sender, requestType, body.toJson(QJsonDocument::Compact), JSON_CONTENT_TYPE, authorization);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::browseZeroConf(const QString& serviceType) {
    if (serviceType.isEmpty()) {
        qDebug() << "Ignoring request to browse for empty ZeroConf service";
        return;
    }

    bool onlyRequest = zeroConfBrowseRequests_.isEmpty();
    zeroConfBrowseRequests_.enqueue(serviceType);
    if (onlyRequest) {
        zeroConf_->startBrowser(serviceType, QAbstractSocket::IPv4Protocol);
        zeroConfBrowseTimer_.start();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::handleReply(QNetworkReply* reply) {
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QObject* sender = reply->request().originatingObject();
    QByteArray body = reply->readAll();

    // Emit an additional signal if this is JSON content.
    if (reply->header(QNetworkRequest::ContentTypeHeader).toString().startsWith(JSON_CONTENT_TYPE)) {
        emit jsonReplyReceived(statusCode, sender, QJsonDocument::fromJson(body));
    }

    emit replyReceived(statusCode, sender, body);
    reply->deleteLater();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::handleZeroConfServiceAdded(QZeroConfService service) {
    if (zeroConfBrowseRequests_.isEmpty() || !service->type().startsWith(zeroConfBrowseRequests_.front())) {
        // Service does not match the next one we were looking for, ignore.
        return;
    }

    emit zeroConfServiceFound(zeroConfBrowseRequests_.dequeue(), service->ip().toString());
    zeroConf_->stopBrowser();

    // Are there more requests pending?
    if (!zeroConfBrowseRequests_.isEmpty()) {
        // Yes, start looking for the next one.
        zeroConf_->startBrowser(zeroConfBrowseRequests_.front(), QAbstractSocket::IPv4Protocol);
        zeroConfBrowseTimer_.start();  // Restart
    } else {
        // No, stop looking.
        zeroConfBrowseTimer_.stop();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
