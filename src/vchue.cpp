#include "vchue.h"
#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

static const QString HUE_SERVICE_TYPE = "_hue._tcp";
/*--------------------------------------------------------------------------------------------------------------------*/

VCHue::VCHue( const QString & Name, QObject * pParent ) :
    VCPlugin( Name, pParent )
{
    // Don't start refreshing until the Bridge has been found.
    m_UpdateTimer.stop();
    setUpdateInterval( 1000 );

    // Handle network responses.
    connect( NetworkInterface::instance(),
             &NetworkInterface::zeroConfServiceFound,
             this,
             &VCHue::handleZeroConfServiceFound );
    connect( NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCHue::handleNetworkReply );

    // Update the base URL whenever dependent properties change.
    connect( this, &VCHue::bridgeIPAddressChanged, this, &VCHue::updateBaseURL );
    connect( this, &VCHue::bridgeUsernameChanged, this, &VCHue::updateBaseURL );

    // Look for the Bridge.
    NetworkInterface::instance()->browseZeroConf( HUE_SERVICE_TYPE );
}
/*--------------------------------------------------------------------------------------------------------------------*/

int VCHue::onDevicesCount() const
{
     int iCount = 0;

     for ( const HueDevice * pDevice : qAsConst( m_Devices ) )
     {
         if ( pDevice->isOn() )
         {
             iCount++;
         }
     }

     return iCount;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::refresh()
{
    NetworkInterface::instance()->sendJSONRequest( QUrl( m_BaseURL ), this );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::commandDeviceState( const int iID, const QJsonObject & Parameters )
{
    HueDevice * pDevice = m_DeviceTable.value( iID, nullptr );
    if ( nullptr != pDevice )
    {
        QUrl URL( QString( "%1/%2/state" ).arg( m_BaseURL ).arg( iID ) );
        NetworkInterface::instance()->sendJSONRequest( URL,
                                                       pDevice,
                                                       QNetworkAccessManager::PutOperation,
                                                       QJsonDocument( Parameters ) );
    }
    else
    {
        qDebug() << "Ignoring request to command state of unknown device: " << iID;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::handleZeroConfServiceFound( const QString & ServiceType, const QString & IPAddress )
{
    if ( m_BridgeIPAddress.isEmpty() && ServiceType.startsWith( HUE_SERVICE_TYPE ) )
    {
        m_BridgeIPAddress = IPAddress;
        qDebug() << "Hue Bridge found at IP address: " << m_BridgeIPAddress;
        emit bridgeIPAddressChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

static bool sortDevice( const HueDevice * const pLeft, const HueDevice * const pRight )
{
    if ( nullptr == pLeft )
    {
        return false;
    }
    if ( nullptr == pRight )
    {
        return true;
    }
    return pLeft->id() < pRight->id();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body )
{
    // Check if this is for us or should be dispatched to a device.
    if ( this == pSender )
    {
        if ( 200 == iStatusCode )
        {
            // Query response for all light information.
            if ( Body.isObject() )
            {
                QJsonObject ResponseObject = Body.object();
                const QStringList Keys = ResponseObject.keys();
                for ( const auto & Key : Keys )
                {
                    bool bOk = false;
                    int iID = Key.toInt( &bOk );
                    if ( bOk )
                    {
                        QJsonObject DeviceObject = ResponseObject.value( Key ).toObject();
                        if ( !DeviceObject.isEmpty() )
                        {
                            // Is there already a record of this device?
                            HueDevice * pDevice = m_DeviceTable.value( iID, nullptr );
                            if ( nullptr == pDevice )
                            {
                                // Inspect the device type to determine the correct object type.
                                QString Type = DeviceObject.value( "type" ).toString().toLower();
                                if ( "dimmable light" == Type )
                                {
                                    pDevice = new HueLight( iID, this );
                                }
                                else if ( "color temperature light" == Type )
                                {
                                    pDevice = new HueAmbianceLight( iID, this );
                                }
                                else if ( Type.endsWith( "color light" ) )
                                {
                                    pDevice = new HueColorLight( iID, this );
                                }
                                else
                                {
                                    pDevice = new HueDevice( iID, this );
                                }

                                // Update the number of devices powered on when it changes.
                                connect( pDevice, &HueDevice::isOnChanged, this, &VCHue::onDevicesCountChanged );

                                // Record the device.
                                m_DeviceTable.insert( iID, pDevice );
                                m_Devices.append( pDevice );
                                std::sort( m_Devices.begin(), m_Devices.end(), sortDevice );
                                emit devicesChanged();
                            }

                            // Dispatch device and state information to the device.
                            pDevice->handleResponse( QJsonDocument( DeviceObject ) );
                        }
                        else
                        {
                            qDebug() << "Got empty or invalid device object in query response from Hue Bridge at key: "
                                     << Key;
                        }
                    }
                    else
                    {
                        qDebug() << "Got invalid ID in query response from Hue Bridge";
                    }
                }
            }
            else
            {
                qDebug() << "Failed to parse query response from Hue Bridge";
            }
        }
        else
        {
            qDebug() << "Ignoring unsuccessful reply from Hue Bridge with status code: " << iStatusCode;
        }
    }
    else
    {
        auto * pDevice = qobject_cast<HueDevice *>( pSender );
        if ( nullptr != pDevice )
        {
            if ( 200 == iStatusCode )
            {
                // Dispatch to the device.
                pDevice->handleResponse( Body );
            }
            else
            {
                qDebug() << "Ignoring unsuccessful reply from Hue Bridge for device " << pDevice->name()
                         << " with status code: " << iStatusCode;
            }
        }
        else
        {
            // Not for us, ignore.
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHue::updateBaseURL()
{
    if ( ( !m_BridgeIPAddress.isEmpty() ) && ( !m_BridgeUsername.isEmpty() ) )
    {
        m_BaseURL = QString( "http://%1/api/%2/lights" ).arg( m_BridgeIPAddress, m_BridgeUsername );

        // With the IP address known, start the update timer and refesh immediately.
        m_UpdateTimer.start();
        refresh();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
