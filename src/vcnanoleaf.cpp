#include <QJsonArray>
#include <QJsonObject>

#include "vcnanoleaf.h"
#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

static const QString NANOLEAF_SERVICE_TYPE = "_nanoleafapi._tcp";
/*--------------------------------------------------------------------------------------------------------------------*/

VCNanoleaf::VCNanoleaf( const QString & Name, QObject * pParent ) :
    VCPlugin( Name, pParent ),
    m_bIsOn( false ),
    m_iCommandedPower( -1 )
{
    // Don't start refreshing until the Nanoleaf has been found.
    m_UpdateTimer.stop();
    setUpdateInterval( 1000 );

    // Handle network responses.
    connect( NetworkInterface::instance(),
             &NetworkInterface::zeroConfServiceFound,
             this,
             &VCNanoleaf::handleZeroConfServiceFound );
    connect( NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCNanoleaf::handleNetworkReply );

    // Update the base URL whenever dependent properties change.
    connect( this, &VCNanoleaf::ipAddressChanged, this, &VCNanoleaf::updateBaseURL );
    connect( this, &VCNanoleaf::authTokenChanged, this, &VCNanoleaf::updateBaseURL );

    // Look for the Nanoleaf.
    NetworkInterface::instance()->browseZeroConf( NANOLEAF_SERVICE_TYPE );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::refresh()
{
    NetworkInterface::instance()->sendJSONRequest( QUrl( m_BaseURL ), this );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::commandPower( const bool bOn )
{
    QJsonObject Command { { "on", QJsonObject { { "value", bOn } } } };
    QUrl Destination( QString( "%1/state" ).arg( m_BaseURL ) );

    // Assume the command will succeed.
    m_iCommandedPower = bOn ? 1 : 0;
    m_bIsOn = bOn;
    emit isOnChanged();

    NetworkInterface::instance()->sendJSONRequest( Destination,
                                                   this,
                                                   QNetworkAccessManager::PutOperation,
                                                   QJsonDocument( Command ) );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::selectEffect( const QString & Effect )
{
    if ( m_Effects.contains( Effect ) )
    {
        QJsonObject Command { { "select", Effect } };
        QUrl Destination( QString( "%1/effects" ).arg( m_BaseURL ) );

        // Assume the command will succeed.
        m_CommandedEffect = Effect;
        m_SelectedEffect = Effect;
        emit selectedEffectChanged();

        NetworkInterface::instance()->sendJSONRequest( Destination,
                                                       this,
                                                       QNetworkAccessManager::PutOperation,
                                                       QJsonDocument( Command ) );
    }
    else
    {
        qDebug() << "Ignoring request to select Nanoleaf effect that is not installed";
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::handleZeroConfServiceFound( const QString & ServiceType, const QString & IPAddress )
{
    if ( m_IPAddress.isEmpty() && ServiceType.startsWith( NANOLEAF_SERVICE_TYPE ) )
    {
        m_IPAddress = IPAddress;
        qDebug() << "Nanoleaf found at IP address: " << m_IPAddress;
        emit ipAddressChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body )
{
    if ( this == pSender )
    {
        if ( 200 == iStatusCode )
        {
            if ( Body.isObject() )
            {
                QJsonObject ResponseObject = Body.object();
                if ( ResponseObject.contains( "name" ) )
                {
                    QString Name = ResponseObject.value( "name" ).toString();
                    if ( m_Name != Name )
                    {
                        m_Name = Name;
                        emit nameChanged();
                    }
                }
                if ( ResponseObject.contains( "effects" ) )
                {
                    QJsonObject EffectsObject = ResponseObject.value( "effects" ).toObject();
                    if ( EffectsObject.contains( "select" ) )
                    {
                        QString Selected = EffectsObject.value( "select" ).toString();

                        // BDP: Ensure the commanded effect is applied, which seems to possibly take more than one try
                        //      if the Nanoleaf has been sitting idle for a while.
                        if ( ( !m_CommandedEffect.isEmpty() ) && ( m_CommandedEffect != Selected ) )
                        {
                            // Command the effect again after a short delay.
                            // TODO(BDP): Give up after a certain number of attempts?
                            QTimer::singleShot( 500, this, [=] { selectEffect( m_CommandedEffect ); } );
                        }
                        else
                        {
                            m_CommandedEffect.clear();

                            if ( m_SelectedEffect != Selected )
                            {
                                m_SelectedEffect = Selected;
                                emit selectedEffectChanged();
                            }
                        }
                    }
                    if ( EffectsObject.contains( "effectsList" ) )
                    {
                        const QJsonArray EffectsArray = EffectsObject.value( "effectsList" ).toArray();
                        QStringList EffectsList;
                        for ( const auto & Effect : EffectsArray )
                        {
                            EffectsList.append( Effect.toString() );
                        }
                        if ( m_Effects != EffectsList )
                        {
                            m_Effects = EffectsList;
                            emit effectsChanged();
                        }
                    }
                }
                if ( ResponseObject.contains( "state" ) )
                {
                    QJsonObject StateObject = ResponseObject.value( "state" ).toObject();
                    if ( StateObject.contains( "on" ) )
                    {
                        QJsonObject OnObject = StateObject.value( "on" ).toObject();
                        if ( OnObject.contains( "value" ) )
                        {
                            bool bOn = OnObject.value( "value" ).toBool();

                            // BDP: Ensure the commanded power is applied.
                            if ( ( ( 0 == m_iCommandedPower ) && bOn ) || ( ( 1 == m_iCommandedPower ) && ( !bOn ) ) )
                            {
                                // Command again after a short delay.
                                QTimer::singleShot( 500, this, [=] { commandPower( 1 == m_iCommandedPower ); } );
                            }
                            else
                            {
                                m_iCommandedPower = -1;

                                if ( m_bIsOn != bOn )
                                {
                                    m_bIsOn = bOn;
                                    emit isOnChanged();
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                qDebug() << "Failed to parse response from Nanoleaf";
            }
        }
        else if ( 204 == iStatusCode )
        {
            // Successful ACK of effect selection, but no content in the response.
        }
        else
        {
            qDebug() << "Ignoring unsuccessful reply from Nanoleaf with status code: " << iStatusCode;
        }
    }
    else
    {
        // Not for us, ignore.
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCNanoleaf::updateBaseURL()
{
    if ( ( !m_IPAddress.isEmpty() ) && ( !m_AuthToken.isEmpty() ) )
    {
        m_BaseURL = QString( "http://%1:16021/api/v1/%2" ).arg( m_IPAddress, m_AuthToken );

        // With the IP address known, start the update timer and refesh immediately.
        m_UpdateTimer.start();
        refresh();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
