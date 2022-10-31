#include <QColor>
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
    setUpdateInterval( 3 * 1000 );

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

void VCNanoleaf::refreshEffects()
{
    QUrl Destination( QString( "%1/effects" ).arg( m_BaseURL ) );
    QJsonObject Write { { "command", "requestAll" } };
    QJsonObject Body { { "write", Write } };

    // BDP: A put request containing a command to the write endpoint is actually just a query? Really?
    NetworkInterface::instance()->sendJSONRequest( Destination,
                                                   this,
                                                   QNetworkAccessManager::PutOperation,
                                                   QJsonDocument( Body ) );
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
                if ( ResponseObject.contains( "animations" ) )
                {
                    QVariantList Effects;
                    const QJsonArray AnimationsArray = ResponseObject.value( "animations" ).toArray();
                    for ( const auto & Animation : AnimationsArray )
                    {
                        QJsonObject AnimationObject = Animation.toObject();
                        if ( !AnimationObject.isEmpty() )
                        {
                            QVariantMap Effect { { "name", AnimationObject.value( "animName" ).toString() } };

                            QStringList Colors;
                            const QJsonArray PaletteArray = AnimationObject.value( "palette" ).toArray();
                            for ( const auto & Palette : PaletteArray )
                            {
                                QJsonObject PaletteObject = Palette.toObject();
                                if ( !PaletteObject.isEmpty() )
                                {
                                    double dHue = PaletteObject.value( "hue" ).toInt() / 359.0;
                                    double dSaturation = PaletteObject.value( "saturation" ).toInt() / 100.0;
                                    double dBrightness = PaletteObject.value( "brightness" ).toInt() / 100.0;
                                    Colors.append( QColor::fromHsvF( dHue, dSaturation, dBrightness ).name() );
                                }
                            }
                            Effect[ "colors" ] = Colors;

                            const QJsonArray OptionsArray = AnimationObject.value( "pluginOptions" ).toArray();
                            for ( const auto & Option : OptionsArray )
                            {
                                QJsonObject OptionObject = Option.toObject();
                                if ( !OptionObject.isEmpty() )
                                {
                                    QString OptionName = OptionObject.value( "name" ).toString();
                                    if ( ( "delayTime" == OptionName ) || ( "transTime" == OptionName ) )
                                    {
                                        // These values are presented in tenths of a second, convert.
                                        Effect[ OptionName ] = OptionObject.value( "value" ).toInt() / 10.0;
                                    }
                                }
                            }

                            Effects.append( Effect );
                        }
                    }

                    if ( !Effects.isEmpty() )
                    {
                        // Sort alphabetically by name.
                        std::sort( Effects.begin(), Effects.end(), []( const QVariant & Left, const QVariant & Right ) {
                            return Left.toMap().value( "name" ).toString() < Right.toMap().value( "name" ).toString();
                        } );

                        if ( m_Effects != Effects )
                        {
                            m_Effects = Effects;
                            emit effectsChanged();
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
        refreshEffects();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
