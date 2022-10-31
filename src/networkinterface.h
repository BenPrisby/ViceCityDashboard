#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include <QtZeroConf/qzeroconf.h>

#include <QByteArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QTimer>

class NetworkInterface final : public QObject {
    Q_OBJECT

 public:
    static NetworkInterface* instance();

    void sendRequest(const QUrl& destination,
                     QObject* sender = nullptr,
                     QNetworkAccessManager::Operation requestType = QNetworkAccessManager::GetOperation,
                     const QByteArray& body = {},
                     const QByteArray& contentType = "text/plain",
                     const QByteArray& authorization = {});
    void sendJSONRequest(const QUrl& destination,
                         QObject* sender = nullptr,
                         QNetworkAccessManager::Operation requestType = QNetworkAccessManager::GetOperation,
                         const QJsonDocument& body = {},
                         const QByteArray& authorization = {});
    void browseZeroConf(const QString& serviceType);

 signals:
    void replyReceived(int statusCode, QObject* sender, const QByteArray& body);
    void jsonReplyReceived(int statusCode, QObject* sender, const QJsonDocument& body);
    void zeroConfServiceFound(const QString& serviceType, const QString& ipAddress);

 private slots:
    void handleReply(QNetworkReply* reply);
    void handleZeroConfServiceAdded(QZeroConfService service);

 private:
    explicit NetworkInterface(QObject* parent = nullptr);

    QNetworkAccessManager* manager_;
    QZeroConf* zeroConf_;
    QQueue<QString> zeroConfBrowseRequests_;
    QTimer zeroConfBrowseTimer_;

    Q_DISABLE_COPY_MOVE(NetworkInterface)
};

#endif  // NETWORKINTERFACE_H_
