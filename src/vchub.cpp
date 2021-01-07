#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QLocale>
#include <QNetworkInterface>
#include <QSysInfo>

#include "vchub.h"
#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

static VCHub * m_pInstance = nullptr;
/*--------------------------------------------------------------------------------------------------------------------*/

VCHub::VCHub( QObject * pParent ) :
    QObject( pParent ),
    m_CurrentDateTime( QDateTime::currentDateTime() ),
    m_Hostname( QHostInfo::localHostName() ),
    m_Platform( QSysInfo::prettyProductName().split( QChar( '(' ) ).first().trimmed() ),
    m_Architecture( QSysInfo::currentCpuArchitecture() ),
    m_QtVersion( qVersion() ),
    m_pHue( new VCHue( "Hue", this ) ),
    m_pNanoleaf( new VCNanoleaf( "Nanoleaf", this ) ),
    m_pPiHole( new VCPiHole( "PiHole", this ) ),
    m_pWeather( new VCWeather( "Weather", this ) ),
    m_pInsults( new VCInsults( "Insults", this ) ),
    m_pSpotify( new VCSpotify( "Spotify", this ) ),
    m_bIsRunningScene( false )
{
    setObjectName( "VCHub" );
    qDebug() << "Initializing dashboard hub";

    // Take ownership of the network interface.
    NetworkInterface::instance()->setParent( this );

    // Set properties from the persistent config file, assumed to be in the home directory.
    loadConfig( QDir::home().filePath( "vcconfig.json" ) );

    // Translate the home map path if it is relative.
    if ( ( !m_HomeMap.isEmpty() ) && QFileInfo( m_HomeMap ).isRelative() )
    {
        // Assume the home directory.
        m_HomeMap = QDir::home().filePath( m_HomeMap );
        emit homeMapChanged();
    }

    // Periodically refresh the current date and time.
    m_CurrentDateTimeRefreshTimer.setInterval( 10 * 1000 );
    m_CurrentDateTimeRefreshTimer.setSingleShot( false );
    connect( &m_CurrentDateTimeRefreshTimer, &QTimer::timeout, this, &VCHub::updateCurrentDateTime );
    m_CurrentDateTimeRefreshTimer.start();

    // Periodically refresh the IP addresses.
    m_IPAddressesRefreshTimer.setInterval( 15 * 1000 );
    m_IPAddressesRefreshTimer.setSingleShot( false );
    connect( &m_IPAddressesRefreshTimer, &QTimer::timeout, this, &VCHub::refreshIPAddresses );
    m_IPAddressesRefreshTimer.start();
    refreshIPAddresses();
}
/*--------------------------------------------------------------------------------------------------------------------*/

VCHub * VCHub::instance()
{
    if ( nullptr == m_pInstance )
    {
        m_pInstance = new VCHub();
    }

    return m_pInstance;
}
/*--------------------------------------------------------------------------------------------------------------------*/

bool VCHub::use24HourClock() const
{
    return m_PersistentSettings.value( "use24HourClock", QVariant( false ) ).toBool();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::setUse24HourClock( const bool bValue )
{
    if ( use24HourClock() != bValue )
    {
        m_PersistentSettings.setValue( "use24HourClock", QVariant( bValue ) );
        emit use24HourClockChanged();

        // Update the time display right away.
        updateCurrentDateTime();

        // Indicate that any QDateTime properties should be reevaluated by emitting the NOTIFY signal.
        const QObjectList Children = children();
        for ( auto * pChild : Children )
        {
            const QMetaObject * pChildMetaObject = pChild->metaObject();
            int iPropertyCount = pChildMetaObject->propertyCount();
            for ( int i = 0; i < iPropertyCount; i++ )
            {
                QMetaProperty Property = pChildMetaObject->property( i );
                if ( ( QVariant::DateTime == Property.type() ) && Property.hasNotifySignal() )
                {
                    Property.notifySignal().invoke( pChild );
                }
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

bool VCHub::darkerBackground() const
{
    return m_PersistentSettings.value( "darkerBackground", QVariant( false ) ).toBool();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::setDarkerBackground( const bool bValue )
{
    if ( darkerBackground() != bValue )
    {
        m_PersistentSettings.setValue( "darkerBackground", QVariant( bValue ) );
        emit darkerBackgroundChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

bool VCHub::screensaverEnabled() const
{
    return m_PersistentSettings.value( "screensaverEnabled", QVariant( true ) ).toBool();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::setScreensaverEnabled( const bool bValue )
{
    if ( screensaverEnabled() != bValue )
    {
        m_PersistentSettings.setValue( "screensaverEnabled", QVariant( bValue ) );
        emit screensaverEnabledChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::runScene( const QString & Scene )
{
    if ( m_Scenes.contains( Scene ) )
    {
        // Indicate that execution is starting.
        m_bIsRunningScene = true;
        emit isRunningSceneChanged();

        // Scenes are constructed as a list of objects that contain device and state information.
        QVariantList Steps = m_Scenes.value( Scene ).toList();
        qDebug() << "Processing scene " << Scene << " with " << Steps.size() << " steps";
        for ( int i = 0; i < Steps.size(); i++ )
        {
            int iStepNumber = i + 1;
            QVariantMap Step = Steps.at( i ).toMap();

            // Ensure the expected structure is present.
            if ( Step.contains( "device" ) && Step.contains( "state" ) )
            {
                QVariantMap Device = Step.value( "device" ).toMap();
                QString Name = Device.value( "name" ).toString();
                QString Class = Device.value( "class" ).toString();
                QVariantMap State = Step.value( "state" ).toMap();

                // Execute the actions of the step based on the device class.
                if ( "hue" == Class )
                {
                    // Locate the Hue device by name.
                    const QList<HueDevice *> & HueDevices = m_pHue->devices();
                    HueDevice * pHueDevice = nullptr;
                    for ( auto * const pDevice : HueDevices )
                    {
                        if ( Name == pDevice->name() )
                        {
                            pHueDevice = pDevice;
                            break;
                        }
                    }

                    // Check if the device has been discovered.
                    if ( nullptr != pHueDevice )
                    {
                        qDebug() << "Executing step " << iStepNumber << " on Hue device: " << Name;

                        // Apply state properties from most to least generic.
                        if ( State.contains( "on" ) )
                        {
                            qDebug() << "\t=> Command power";
                            pHueDevice->commandPower( State.take( "on" ).toBool() );
                        }
                        if ( State.contains( "brightness" ) )
                        {
                            // This must be a light.
                            auto * pHueLight = qobject_cast<HueLight *>( pHueDevice );
                            if ( nullptr != pHueLight )
                            {
                                qDebug() << "\t=> Command brightness";
                                pHueLight->commandBrightness( State.take( "brightness" ).toDouble() );
                            }
                            else
                            {
                                State.remove( "brightness" );  // Still consume the value
                                qDebug() << "Encountered brightness command for Hue device " << Name
                                         << " that is not a light in step " << iStepNumber
                                         << " when processing scene: " << Scene;
                            }
                        }
                        if ( State.contains( "colorTemperature" ) )
                        {
                            // This must be an ambiance light.
                            auto * pHueLight = qobject_cast<HueAmbianceLight *>( pHueDevice );
                            if ( nullptr != pHueLight )
                            {
                                qDebug() << "\t=> Command color temperature";
                                pHueLight->commandColorTemperature( State.take( "colorTemperature" ).toInt() );
                            }
                            else
                            {
                                State.remove( "colorTemperature" );  // Still consume the value
                                qDebug() << "Encountered color temperature command for Hue device " << Name
                                         << " that is not an ambiance light in step " << iStepNumber
                                         << " when processing scene: " << Scene;
                            }
                        }
                        if ( State.contains( "xy" ) )
                        {
                            // This must be a color light.
                            auto * pHueLight = qobject_cast<HueColorLight *>( pHueDevice );
                            if ( nullptr != pHueLight )
                            {
                                qDebug() << "\t=> Command XY color";
                                QVariantList XY = State.take( "xy" ).toList();
                                pHueLight->commandColor( XY.at( 0 ).toDouble(), XY.at( 1 ).toDouble() );
                            }
                            else
                            {
                                State.remove( "xy" );  // Still consume the value
                                qDebug() << "Encountered XY color command for Hue device " << Name
                                         << " that is not a color light in step " << iStepNumber
                                         << " when processing scene: " << Scene;
                            }
                        }
                        if ( State.contains( "hue" ) )
                        {
                            // This must be a color light.
                            auto * pHueLight = qobject_cast<HueColorLight *>( pHueDevice );
                            if ( nullptr != pHueLight )
                            {
                                qDebug() << "\t=> Command hue color";
                                pHueLight->commandColor( State.take( "hue" ).toInt() );
                            }
                            else
                            {
                                State.remove( "hue" );  // Still consume the value
                                qDebug() << "Encountered hue color command for Hue device " << Name
                                         << " that is not a color light in step " << iStepNumber
                                         << " when processing scene: " << Scene;
                            }
                        }
                        if ( !State.isEmpty() )
                        {
                            qDebug() << "Detected unsupported state properties " << State.keys() << " for Hue device "
                                     << Name << " in step " << iStepNumber << " when processing scene: " << Scene;
                        }
                    }
                    else
                    {
                        qDebug() << "Encountered unknown Hue device name " << Name << " in step " << iStepNumber
                                 << " when processing scene: " << Scene;
                    }
                }
                else if ( "nanoleaf" == Class )
                {
                    // Ensure this is the discovered Nanoleaf.
                    if ( Name == m_pNanoleaf->name() )
                    {
                        qDebug() << "Executing step " << iStepNumber << " on Nanoleaf: " << Name;
                        if ( State.contains( "on" ) )
                        {
                            qDebug() << "\t=> Command power";
                            m_pNanoleaf->commandPower( State.take( "on" ).toBool() );
                        }
                        if ( State.contains( "effect" ) )
                        {
                            qDebug() << "\t=> Select effect";
                            m_pNanoleaf->selectEffect( State.take( "effect" ).toString() );
                        }
                        if ( !State.isEmpty() )
                        {
                            qDebug() << "Detected unsupported state properties " << State.keys() << " for Nanoleaf "
                                     << Name << " in step " << iStepNumber << " when processing scene: " << Scene;
                        }
                    }
                    else
                    {
                        qDebug() << "Encountered unknown Nanoleaf name " << Name << " in step " << iStepNumber
                                 << " when processing scene: " << Scene;
                    }
                }
                else
                {
                    qDebug() << "Encountered unsupported class " << Class << " in step " << iStepNumber
                             << " when processing scene: " << Scene;
                }
            }
            else
            {
                qDebug() << "Missing device and/or state for step " << iStepNumber << " in scene: " << Scene;
            }

            // Insert a brief pause in between steps to prevent overloading the devices.
            QTime Future = QTime::currentTime().addMSecs( 200 );
            for ( ; Future > QTime::currentTime(); /* Passage of time. */ )
            {
                QCoreApplication::processEvents( QEventLoop::AllEvents, 500 );
            }
        }

        qDebug() << "Finished processing scene: " << Scene;
        m_bIsRunningScene = false;
        emit isRunningSceneChanged();
    }
    else
    {
        qDebug() << "Ignoring request to run unknown scene: " << Scene;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::dayOfWeek( const QDateTime & DateTime )
{
    return DateTime.toString( "dddd" );
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatTime( const QDateTime & DateTime )
{
    return DateTime.toString( use24HourClock() ? "hh:mm" : "h:mm AP" );
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatInt( int iValue, const QString & Unit )
{
    QString Display = QLocale::system().toString( iValue );
    if ( !Unit.isEmpty() )
    {
        Display.append( QString( " %1" ).arg( Unit ) );
    }
    return Display;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatDecimal( double dValue, const QString & Unit )
{
    QString Display = QLocale::system().toString( dValue, 'f', 1 );  // 1 decimal place
    if ( !Unit.isEmpty() )
    {
        Display.append( QString( " %1" ).arg( Unit ) );
    }
    return Display;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatPercentage( double dValue, bool bWholeNumber )
{
    return QString( "%1%" ).arg( QLocale::system().toString( dValue, 'f', bWholeNumber ? 0 : 1 ) );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::updateCurrentDateTime()
{
    m_CurrentDateTime = QDateTime::currentDateTime();
    emit currentDateTimeChanged();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::refreshIPAddresses()
{
    // Examine all of the system network interfaces and any associated IP addresses they may have.
    const QList<QNetworkInterface> Interfaces = QNetworkInterface::allInterfaces();
    for ( const auto & Interface : Interfaces )
    {
        // Avoid loopback, P2P, or otherwise virtual interfaces.
        if ( ( QNetworkInterface::Ethernet == Interface.type() ) || ( QNetworkInterface::Wifi == Interface.type() ) )
        {
            const QList<QNetworkAddressEntry> AddressEntries = Interface.addressEntries();
            for ( const auto & AddressEntry : AddressEntries )
            {
                QHostAddress Address = AddressEntry.ip();
                if ( ( QAbstractSocket::IPv4Protocol == Address.protocol() ) && ( !Address.isLoopback() ) )
                {
                    QString IPAddress = Address.toString();
                    if ( ( QNetworkInterface::Ethernet == Interface.type() ) && ( m_EthernetIPAddress != IPAddress ) )
                    {
                        m_EthernetIPAddress = IPAddress;
                        emit ethernetIPAddressChanged();
                    }
                    else if ( ( QNetworkInterface::Wifi == Interface.type() ) && ( m_EthernetIPAddress != IPAddress ) )
                    {
                        m_WifiIPAddress = IPAddress;
                        emit wifiIPAddressChanged();
                    }
                }
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::loadConfig( const QString & Path )
{
    QFile ConfigFile( Path );
    if ( ConfigFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        qDebug() << "Applying config file: " << ConfigFile.fileName();
        QJsonDocument ConfigDocument = QJsonDocument::fromJson( ConfigFile.readAll() );
        if ( ConfigDocument.isObject() )
        {
            QJsonObject Config = ConfigDocument.object();
            const QStringList Settings = Config.keys();
            for ( const auto & Setting : Settings )
            {
                // Keys are paths into the Meta Object system using object names, with the property to set at the end.
                QStringList Parts = Setting.split( QChar( '.' ) );
                if ( ( 2 <= Parts.length() ) && ( objectName() == Parts.takeFirst() ) )
                {
                    QString Property = Parts.takeLast();
                    QObject * pObject = this;  // Treat ourselves as the top-most item in the heirarchy

                    // If necessary, recurse down into the children.
                    for ( QString ObjectName; ( nullptr != pObject ) && ( !Parts.isEmpty() ); /* Advanced in loop. */ )
                    {
                        ObjectName = Parts.takeFirst();
                        pObject = pObject->findChild<QObject *>( ObjectName );
                    }

                    // Was the object found in the heirarchy?
                    if ( nullptr != pObject )
                    {
                        // Ensure the property exists.
                        if ( pObject->property( Property.toStdString().c_str() ).isValid() )
                        {
                            // Set the property to the value specified in the file.
                            pObject->setProperty( Property.toStdString().c_str(), Config.value( Setting ).toVariant() );
                        }
                        else
                        {
                            qWarning() << "Ignoring setting path because the property does not exist: " << Setting;
                        }
                    }
                    else
                    {
                        qWarning() << "Ignoring setting path because it does not reference a valid object: " << Setting;
                    }
                }
                else
                {
                    qWarning() << "Ignorning invalid setting path structure: " << Setting;
                }
            }
        }
        else
        {
            qWarning() << "Failed to parse config file structure: " << ConfigFile.fileName();
        }
        ConfigFile.close();
    }
    else
    {
        qWarning() << "Failed to open config file: " << ConfigFile.fileName();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

QObject * vchub_singletontype_provider( QQmlEngine * pEngine, QJSEngine * pScriptEngine )
{
    ( void )pEngine;
    ( void )pScriptEngine;

    return VCHub::instance();
}
/*--------------------------------------------------------------------------------------------------------------------*/
