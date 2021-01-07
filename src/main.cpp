#include <QApplication>
#include <QFontDatabase>
#include <QQmlApplicationEngine>

#include <QFile>
#include <QCryptographicHash>

#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

int main( int argc, char * argv[] )
{
    // Bring in the Virtual Keyboard.
    qputenv( "QT_IM_MODULE", QByteArray( "qtvirtualkeyboard" ) );
    qputenv( "QT_VIRTUALKEYBOARD_LAYOUT_PATH", QByteArray( "qrc:/keyboard/layouts" ) );
    qputenv( "QT_VIRTUALKEYBOARD_STYLE", QByteArray( "vc" ) );

    // Apply high-level application properties.
    QCoreApplication::setApplicationName( "Vice City Dashboard" );
    QCoreApplication::setApplicationVersion( APPLICATION_VERSION );
    QCoreApplication::setOrganizationName( "Ben Prisby" );
    QCoreApplication::setOrganizationDomain( "benprisby.com" );
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );

    // Create the application context.
    QApplication App( argc, argv );
    App.setObjectName( "app" );

    // Register the fonts for the application.
    QFontDatabase::addApplicationFont( ":/fonts/Lato-Bold.ttf" );
    QFontDatabase::addApplicationFont( ":/fonts/Lato-Regular.ttf" );
    QGuiApplication::setFont( QFont( "Lato" ) );

    // Register the central C++ type that should be exposed to QML.
    qmlRegisterSingletonType<VCHub>( "com.benprisby.vc.vchub", 1, 0, "VCHub", vchub_singletontype_provider );
    QQmlEngine::setObjectOwnership( VCHub::instance(), QQmlEngine::CppOwnership );
    VCHub::instance()->setParent( &App );

    // Create the QML context.
    QQmlApplicationEngine Engine( &App );
    Engine.addImportPath( "qrc:/" );
    Engine.addImportPath( "qrc:/keyboard/style" );
    Engine.load( "qrc:/main.qml" );
    if ( Engine.rootObjects().isEmpty() )
    {
        return -1;
    }

    return App.exec();
}
/*--------------------------------------------------------------------------------------------------------------------*/
