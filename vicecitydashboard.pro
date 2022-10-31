QT += charts network quick virtualkeyboard widgets

CONFIG += c++11

CONFIG(release, debug|release):CONFIG += qtquickcompiler

!defined(VERSION,var):{ VERSION=1.0.0 }
DEFINES += APPLICATION_VERSION=\"\\\"$${VERSION}\\\"\"

# Treat warnings as errors.
QMAKE_CXXFLAGS += -Werror

# Ignore GCC warnings about ABI changes since all code will be built with the same compiler.
unix:!macx: QMAKE_CXXFLAGS += -Wno-psabi

SOURCES += \
        src/hueambiancelight.cpp \
        src/huecolorlight.cpp \
        src/huedevice.cpp \
        src/huelight.cpp \
        src/main.cpp \
        src/networkinterface.cpp \
        src/vcconfig.cpp \
        src/vcfacts.cpp \
        src/vchub.cpp \
        src/vchue.cpp \
        src/vcnanoleaf.cpp \
        src/vcpihole.cpp \
        src/vcplugin.cpp \
        src/vcspotify.cpp \
        src/vcweather.cpp

HEADERS += \
    src/hueambiancelight.h \
    src/huecolorlight.h \
    src/huedevice.h \
    src/huelight.h \
    src/networkinterface.h \
    src/vcconfig.h \
    src/vcfacts.h \
    src/vchub.h \
    src/vchue.h \
    src/vcnanoleaf.h \
    src/vcpihole.h \
    src/vcplugin.h \
    src/vcspotify.h \
    src/vcweather.h

RESOURCES += qml/qml.qrc \
    resources/resources.qrc

INCLUDEPATH += src

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH = qml

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Target for running the installation step.
target.path = $$_PRO_FILE_PWD_/bin

# QtZeroConf linking.
include($$_PRO_FILE_PWD_/external/QtZeroConf/qtzeroconf.pri)
INCLUDEPATH += $$_PRO_FILE_PWD_/external
DEFINES += QZEROCONF_STATIC
