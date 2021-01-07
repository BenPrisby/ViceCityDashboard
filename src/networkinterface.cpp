#include <QCoreApplication>

#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

static NetworkInterface * m_pInstance = nullptr;
static QByteArray JSON_CONTENT_TYPE = "application/json";
/*--------------------------------------------------------------------------------------------------------------------*/

NetworkInterface::NetworkInterface( QObject * pParent ) :
    QObject( pParent ),
    m_pManager( new QNetworkAccessManager( this ) ),
    m_pZeroConf( new QZeroConf( this ) )
{
    setObjectName( "NetworkInterface" );

    connect( m_pManager, &QNetworkAccessManager::finished, this, &NetworkInterface::handleReply );
    connect( m_pZeroConf, &QZeroConf::serviceAdded, this, &NetworkInterface::handleZeroConfServiceAdded );

    // Configure a timeout on browsing for ZeroConf services.
    m_ZeroConfBrowseTimer.setInterval( 15 * 1000 );
    m_ZeroConfBrowseTimer.setSingleShot( true );
    connect( &m_ZeroConfBrowseTimer, &QTimer::timeout, this, [=]{
        QString ServiceType = m_ZeroConfBrowseRequests.dequeue();
        qDebug() << "Failed to find ZeroConf service type: " << ServiceType;
        m_pZeroConf->stopBrowser();

        // Are there more requests pending?
        if ( !m_ZeroConfBrowseRequests.isEmpty() )
        {
            m_pZeroConf->startBrowser( m_ZeroConfBrowseRequests.front(), QAbstractSocket::IPv4Protocol );
            m_ZeroConfBrowseTimer.start();
        }
    });
}
/*--------------------------------------------------------------------------------------------------------------------*/

NetworkInterface * NetworkInterface::instance()
{
    if ( nullptr == m_pInstance )
    {
        m_pInstance = new NetworkInterface();
    }

    return m_pInstance;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::sendRequest( const QUrl & Destination,
                                    QObject * pSender,
                                    QNetworkAccessManager::Operation eRequestType,
                                    const QByteArray & Body,
                                    const QByteArray & ContentType,
                                    const QByteArray & Authorization )
{
    if ( Destination.isValid() )
    {
        QNetworkRequest Request( Destination );
        QByteArray Data;

        // Attach the application information to the request.
        static QByteArray ApplicationInfo = QString( "%1 %2" )
                .arg( QCoreApplication::applicationName(), QCoreApplication::applicationVersion() ).toUtf8();
        Request.setRawHeader( "User-Agent", ApplicationInfo );

        // Was a sender specified for context?
        if ( nullptr != pSender )
        {
            Request.setOriginatingObject( pSender );
        }

        // Were a body and corresponding content type supplied?
        if ( ( ( !Body.isEmpty() ) && ( !ContentType.isEmpty() ) )
             || ( QNetworkAccessManager::PostOperation == eRequestType ) )
        {
            Request.setHeader( QNetworkRequest::ContentTypeHeader, ContentType );
        }

        // Was an authorization token supplied?
        if ( !Authorization.isEmpty() )
        {
            Request.setRawHeader( "Authorization", Authorization );
        }

        switch ( eRequestType )
        {
        case QNetworkAccessManager::GetOperation:
            m_pManager->get( Request );
            break;

        case QNetworkAccessManager::PostOperation:
            m_pManager->post( Request, Body );
            break;

        case QNetworkAccessManager::PutOperation:
            m_pManager->put( Request, Body );
            break;

        case QNetworkAccessManager::DeleteOperation:
            m_pManager->deleteResource( Request );
            break;

        default:
            qDebug() << "Ignoring unsupported request type";
            break;
        }
    }
    else
    {
        qDebug() << "Ignoring request with invalid URL";
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::sendJSONRequest( const QUrl & Destination,
                                        QObject * pSender,
                                        QNetworkAccessManager::Operation eRequestType,
                                        const QJsonDocument & Body,
                                        const QByteArray & Authorization )
{
    sendRequest( Destination,
                 pSender,
                 eRequestType,
                 Body.toJson( QJsonDocument::Compact ),
                 JSON_CONTENT_TYPE,
                 Authorization );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::browseZeroConf( const QString & ServiceType )
{
    if ( !ServiceType.isEmpty() )
    {
        bool bOnlyRequest = m_ZeroConfBrowseRequests.isEmpty();
        m_ZeroConfBrowseRequests.enqueue( ServiceType );
        if ( bOnlyRequest )
        {
            m_pZeroConf->startBrowser( ServiceType, QAbstractSocket::IPv4Protocol );
            m_ZeroConfBrowseTimer.start();
        }
    }
    else
    {
        qDebug() << "Ignoring request to browse for empty ZeroConf service";
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::handleReply( QNetworkReply * pReply )
{
    int iStatusCode = pReply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    QObject * pSender = pReply->request().originatingObject();
    QByteArray Body = pReply->readAll();

    // Emit an additional signal if this is JSON content.
    if ( pReply->header( QNetworkRequest::ContentTypeHeader ).toString().startsWith( JSON_CONTENT_TYPE ) )
    {
        emit jsonReplyReceived( iStatusCode, pSender, QJsonDocument::fromJson( Body ) );
    }

    emit replyReceived( iStatusCode, pSender, Body );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void NetworkInterface::handleZeroConfServiceAdded( QZeroConfService pService )
{
    if ( ( !m_ZeroConfBrowseRequests.isEmpty() )
         && ( pService->type().startsWith( m_ZeroConfBrowseRequests.front() ) ) )
    {
        emit zeroConfServiceFound( m_ZeroConfBrowseRequests.dequeue(), pService->ip().toString() );
        m_pZeroConf->stopBrowser();

        // Are there more requests pending?
        if ( !m_ZeroConfBrowseRequests.isEmpty() )
        {
            // Yes, start looking for the next one.
            m_pZeroConf->startBrowser( m_ZeroConfBrowseRequests.front(), QAbstractSocket::IPv4Protocol );
            m_ZeroConfBrowseTimer.start();  // Restart
        }
        else
        {
            // No, stop looking.
            m_ZeroConfBrowseTimer.stop();
        }
    }
    else
    {
        // Service does not match the next one we were looking for, ignore.
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
