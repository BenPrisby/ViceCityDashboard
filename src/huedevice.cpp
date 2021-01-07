#include <QJsonArray>

#include "huedevice.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

HueDevice::HueDevice( int iID, QObject * pParent ) :
    QObject( pParent ),
    m_iID( iID ),
    m_bIsReachable( false ),
    m_bIsOn( false )
{
    qDebug() << "Created Hue device with ID: " << m_iID;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueDevice::commandPower( const bool bOn )
{
    VCHub::instance()->hue()->commandDeviceState( m_iID, QJsonObject { { "on", bOn } } );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueDevice::handleResponse( const QJsonDocument & Response )
{
    // Is this a direct reply to a command?
    if ( Response.isArray() )
    {
        // Yes, unpack the response structure.
        // BDP: There are a few layers to unpack here, but it seems to be to support multiple replies in the same
        //      message. We will only be sending one at a time, but still support multiple.
        QJsonObject State;
        const QJsonArray ResponseArray = Response.array();
        for ( const auto & ResponseItem : ResponseArray )
        {
            QJsonObject ResponseObject = ResponseItem.toObject();
            if ( ResponseObject.contains( "success" ) )
            {
                QJsonObject SuccessObject = ResponseObject.value( "success" ).toObject();
                const QStringList Arguments = SuccessObject.keys();
                for ( const auto & Argument : Arguments )
                {
                    // The reply is identified by the API endpoint portion at the end of the URL.
                    if ( Argument.startsWith( QString( "/lights/%1/state/" ).arg( m_iID ) ) )
                    {
                        // The final token will be the state property that was set.
                        QStringList Parts = Argument.split( QChar( '/' ) );
                        State.insert( Parts.last(), SuccessObject.value( Argument ) );
                    }
                    else
                    {
                        qDebug() << "Got unexpected argument when handling response for Hue device: " << m_iID;
                    }
                }
            }
            else
            {
                qDebug() << "Received error when handling response for Hue device: " << m_iID;
            }
        }

        // Process any collected data, providing it in the expected structure.
        handleResponseData( QJsonObject { { "state", State } } );
    }
    else if ( Response.isObject() )
    {
        // No, pass through.
        handleResponseData( Response.object() );
    }
    else
    {
        qDebug() << "Failed to parse response for Hue device: " << m_iID;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueDevice::handleStateData( const QJsonObject & State )
{
    if ( State.contains( "reachable" ) )
    {
        bool bReachable = State.value( "reachable" ).toBool();
        if ( m_bIsReachable != bReachable )
        {
            m_bIsReachable = bReachable;
            emit isReachableChanged();
        }
    }
    if ( State.contains( "on" ) )
    {
        bool bOn = State.value( "on" ).toBool();
        if ( m_bIsOn != bOn )
        {
            m_bIsOn = bOn;
            emit isOnChanged();
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueDevice::handleResponseData( const QJsonObject & Response )
{
    // Process top-level values.
    if ( Response.contains( "name" ) )
    {
        QString Name = Response.value( "name" ).toString();
        if ( m_Name != Name )
        {
            m_Name = Name;
            emit nameChanged();
        }
    }
    if ( Response.contains( "type" ) )
    {
        QString Type = Response.value( "type" ).toString();
        if ( m_Type != Type )
        {
            m_Type = Type;
            emit typeChanged();
        }
    }
    if ( Response.contains( "productname" ) )
    {
        QString ProductName = Response.value( "productname" ).toString();
        if ( m_ProductName != ProductName )
        {
            m_ProductName = ProductName;
            emit productNameChanged();
        }
    }

    // Unpack and process state information.
    handleStateData( Response.value( "state" ).toObject() );
}
/*--------------------------------------------------------------------------------------------------------------------*/
