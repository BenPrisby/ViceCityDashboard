#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QLocale>
#include <QNetworkInterface>
#include <QStandardPaths>
#include <QSysInfo>

#include "vchub.h"
#include "networkinterface.h"
#include "vcconfig.h"
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
    m_bUse24HourClock( false ),
    m_bDarkerBackground( false ),
    m_bScreensaverEnabled( true ),
    m_pHue( new VCHue( "Hue", this ) ),
    m_pNanoleaf( new VCNanoleaf( "Nanoleaf", this ) ),
    m_pPiHole( new VCPiHole( "PiHole", this ) ),
    m_pWeather( new VCWeather( "Weather", this ) ),
    m_pFacts( new VCFacts( "Facts", this ) ),
    m_pSpotify( new VCSpotify( "Spotify", this ) ),
    m_bIsRunningScene( false )
{
    setObjectName( "Hub" );
    qDebug() << "Initializing dashboard hub";

    // Take ownership of the network interface.
    NetworkInterface::instance()->setParent( this );

#ifdef QT_DEBUG
    // Reload the config file if it changes externally.
    connect( &m_ConfigFileWatcher, &QFileSystemWatcher::fileChanged, this, [=]( const QString & Path ) {
        qDebug() << "Reloading config file because it has changed externally";
        ( void )loadConfig( Path );
    } );
#endif

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

void VCHub::setUse24HourClock( const bool bValue )
{
    if ( m_bUse24HourClock != bValue )
    {
        m_bUse24HourClock = bValue;
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

bool VCHub::loadConfig( const QString & Path )
{
    bool bReturn = VCConfig::instance()->load( Path );

    if ( bReturn )
    {
        // Translate the home map path if it is relative.
        if ( ( !m_HomeMap.isEmpty() ) && QFileInfo( m_HomeMap ).isRelative() )
        {
            // Assume the same directory as the config file.
            m_HomeMap = QFileInfo( Path ).dir().filePath( m_HomeMap );
            emit homeMapChanged();
        }

#ifdef QT_DEBUG
        // Watch the config file on disk.
        if ( m_ConfigFileWatcher.files().isEmpty() )
        {
            m_ConfigFileWatcher.addPath( Path );
        }
#endif
    }

    return bReturn;
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCHub::runScene( const QString & Scene )
{
    QVariantList Steps = extractSceneSteps( Scene );
    if ( !Steps.isEmpty() )
    {
        // Indicate that execution is starting.
        m_bIsRunningScene = true;
        emit isRunningSceneChanged();

        // Scenes steps are constructed as a list of objects that contain device and state information.
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
        // No steps, assume this means the scene was not found.
        qDebug() << "Ignoring request to run unknown scene: " << Scene;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

QStringList VCHub::parseSceneColors( const QString & Scene )
{
    QStringList Colors;

    // Pull any called-out colors from the step states.
    const QVariantList Steps = extractSceneSteps( Scene );
    if ( !Steps.isEmpty() )
    {
        for ( const auto & Step : Steps )
        {
            QVariantMap State = Step.toMap().value( "state" ).toMap();
            if ( State.contains( "xy" ) )
            {
                QVariantList XY = State.value( "xy" ).toList();
                if ( 2  == XY.size() )
                {
                    QColor Color = HueColorLight::xyToColor( XY.at( 0 ).toDouble(), XY.at( 1 ).toDouble() );
                    if ( Color.isValid() )
                    {
                        Colors.append( Color.name() );
                    }
                }
            }
            if ( State.contains( "hue" ) )
            {
                QColor Color = HueColorLight::hueToColor( State.value( "hue" ).toInt() );
                if ( Color.isValid() )
                {
                    Colors.append( Color.name() );
                }
            }
        }
    }

    // Sort the list alphabetically so duplicate colors are adjacent.
    if ( !Colors.isEmpty() )
    {
        std::sort( Colors.begin(), Colors.end() );
    }

    return Colors;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::dayOfWeek( const QDateTime & DateTime ) const
{
    return DateTime.toString( "dddd" );
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatTime( const QDateTime & DateTime ) const
{
    return DateTime.toString( use24HourClock() ? "hh:mm" : "h:mm AP" );
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatInt( int iValue, const QString & Unit ) const
{
    QString Display = QLocale::system().toString( iValue );
    if ( !Unit.isEmpty() )
    {
        Display.append( QString( " %1" ).arg( Unit ) );
    }
    return Display;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatDecimal( double dValue, const QString & Unit ) const
{
    QString Display = QLocale::system().toString( dValue, 'f', 1 );  // 1 decimal place
    if ( !Unit.isEmpty() )
    {
        Display.append( QString( " %1" ).arg( Unit ) );
    }
    return Display;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::formatPercentage( double dValue, bool bWholeNumber ) const
{
    return QString( "%1%" ).arg( QLocale::system().toString( dValue, 'f', bWholeNumber ? 0 : 1 ) );
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCHub::screenshotPath() const
{
    QString Format = QString( "yyyy-MM-dd %1" ).arg( use24HourClock() ? "hh.mm.ss" : "h.mm.ss AP" );
    QString Filename = QString( "VC Screenshot %1.png" ).arg( QDateTime::currentDateTime().toString( Format ) );
    return QDir( QStandardPaths::writableLocation( QStandardPaths::DesktopLocation ) ).absoluteFilePath( Filename );
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

QVariantList VCHub::extractSceneSteps( const QString & Scene )
{
    QVariantList Steps;

    // Find the scene in the list and extract its steps.
    for ( const auto & SceneItem : qAsConst( m_Scenes ) )
    {
        QVariantMap SceneMap = SceneItem.toMap();
        QString Name = SceneMap.value( "name" ).toString();
        if ( ( !Name.isEmpty() ) && ( Scene == Name ) )
        {
            Steps = SceneMap.value( "steps" ).toList();
            break;
        }
    }

    return Steps;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QObject * vchub_singletontype_provider( QQmlEngine * pEngine, QJSEngine * pScriptEngine )
{
    ( void )pEngine;
    ( void )pScriptEngine;

    return VCHub::instance();
}
/*--------------------------------------------------------------------------------------------------------------------*/
