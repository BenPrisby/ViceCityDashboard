#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>
#include <QTimer>
#include <QtZeroConf/qzeroconf.h>

class NetworkInterface : public QObject
{
    Q_OBJECT

public:
    static NetworkInterface * instance();

    void sendRequest( const QUrl & Destination,
                      QObject * pSender = nullptr,
                      QNetworkAccessManager::Operation eRequestType = QNetworkAccessManager::GetOperation,
                      const QByteArray & Body = QByteArray(),
                      const QByteArray & ContentType = "text/plain",
                      const QByteArray & Authorization = QByteArray() );
    void sendJSONRequest( const QUrl & Destination,
                          QObject * pSender = nullptr,
                          QNetworkAccessManager::Operation eRequestType = QNetworkAccessManager::GetOperation,
                          const QJsonDocument & Body = QJsonDocument(),
                          const QByteArray & Authorization = QByteArray() );
    void browseZeroConf( const QString & ServiceType );

signals:
    void replyReceived( int iStatusCode, QObject * pSender, const QByteArray & Body );
    void jsonReplyReceived( int iStatusCode, QObject * pSender, const QJsonDocument & Body );
    void zeroConfServiceFound( const QString & ServiceType, const QString & IPAddress );

private slots:
    void handleReply( QNetworkReply * pReply );
    void handleZeroConfServiceAdded( QZeroConfService pService );

private:
    explicit NetworkInterface( QObject * pParent = nullptr );

    QNetworkAccessManager * m_pManager;
    QZeroConf * m_pZeroConf;
    QQueue<QString> m_ZeroConfBrowseRequests;
    QTimer m_ZeroConfBrowseTimer;

    Q_DISABLE_COPY_MOVE( NetworkInterface )
};

#endif  // NETWORKINTERFACE_H_
