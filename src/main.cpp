#include <QApplication>
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QQmlApplicationEngine>

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

    // Register command line options.
    QCommandLineParser Parser;
    Parser.setApplicationDescription( "Smart home integration dashboard for Vice City" );
    Parser.addHelpOption();
    Parser.addVersionOption();
    Parser.addOption( { { "c", "config" }, "Load configuration from <file>.", "file" } );

    // Process the command line options.
    Parser.process( App );

    // Ensure a config file was specified.
    if ( !Parser.isSet( "config" ) )
    {
        qWarning() << "No configuration file specified\n";
        Parser.showHelp( 1 );
    }

    // Load the specified config file.
    if ( !VCHub::instance()->loadConfig( Parser.value( "config" ) ) )
    {
        return 2;
    }

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
        return 3;
    }

    return App.exec();
}
/*--------------------------------------------------------------------------------------------------------------------*/
