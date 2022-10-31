#include <QApplication>
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QQmlApplicationEngine>

#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

int main(int argc, char* argv[]) {
    // Bring in the Virtual Keyboard.
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", QByteArray("qrc:/keyboard/layouts"));
    qputenv("QT_VIRTUALKEYBOARD_STYLE", QByteArray("vc"));

    // Apply high-level application properties.
    QCoreApplication::setApplicationName("Vice City Dashboard");
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);
    QCoreApplication::setOrganizationName("Ben Prisby");
    QCoreApplication::setOrganizationDomain("benprisby.com");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Create the application context.
    QApplication app(argc, argv);
    app.setObjectName("app");

    // Register command line options.
    QCommandLineParser parser;
    parser.setApplicationDescription("Smart home integration dashboard for Vice City");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption({{"c", "config"}, "Load configuration from <file>.", "file"});

    // Process the command line options.
    parser.process(app);

    // Ensure a config file was specified.
    if (!parser.isSet("config")) {
        qWarning() << "No configuration file specified\n";
        parser.showHelp(1);
    }

    // Load the specified config file.
    if (!VCHub::instance()->loadConfig(parser.value("config"))) {
        return 2;
    }

    // Register the fonts for the application.
    QFontDatabase::addApplicationFont(":/fonts/Lato-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Lato-Regular.ttf");
    QGuiApplication::setFont(QFont("Lato"));

    // Register the central C++ type that should be exposed to QML.
    qmlRegisterSingletonType<VCHub>("com.benprisby.vc.vchub", 1, 0, "VCHub", vchub_singletontype_provider);
    QQmlEngine::setObjectOwnership(VCHub::instance(), QQmlEngine::CppOwnership);
    VCHub::instance()->setParent(&app);

    // Create the QML context.
    QQmlApplicationEngine engine(&app);
    engine.addImportPath("qrc:/");
    engine.addImportPath("qrc:/keyboard/style");
    engine.load("qrc:/main.qml");
    if (engine.rootObjects().isEmpty()) {
        return 3;
    }

    return app.exec();
}
/*--------------------------------------------------------------------------------------------------------------------*/
