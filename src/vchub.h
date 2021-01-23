#ifndef VCHUB_H_
#define VCHUB_H_

#include <QDateTime>
#ifdef QT_DEBUG
#include <QFileSystemWatcher>
#endif
#include <QQmlEngine>
#include <QTimer>

#include "vcfacts.h"
#include "vchue.h"
#include "vcnanoleaf.h"
#include "vcpihole.h"
#include "vcspotify.h"
#include "vcweather.h"

class VCHub final : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QDateTime currentDateTime  READ currentDateTime     MEMBER m_CurrentDateTime      NOTIFY currentDateTimeChanged )
    Q_PROPERTY( QString ethernetIPAddress  READ ethernetIPAddress                                 NOTIFY ethernetIPAddressChanged )
    Q_PROPERTY( QString wifiIPAddress      READ wifiIPAddress                                     NOTIFY wifiIPAddressChanged )
    Q_PROPERTY( QString hostname           READ hostname                                          CONSTANT )
    Q_PROPERTY( QString platform           READ platform                                          CONSTANT )
    Q_PROPERTY( QString architecture       READ architecture                                      CONSTANT )
    Q_PROPERTY( QString qtVersion          READ qtVersion                                         CONSTANT )
    Q_PROPERTY( bool use24HourClock        READ use24HourClock      WRITE setUse24HourClock       NOTIFY use24HourClockChanged )
    Q_PROPERTY( bool darkerBackground      READ darkerBackground    MEMBER m_bDarkerBackground    NOTIFY darkerBackgroundChanged )
    Q_PROPERTY( bool screensaverEnabled    READ screensaverEnabled  MEMBER m_bScreensaverEnabled  NOTIFY screensaverEnabledChanged )
    Q_PROPERTY( VCHue * hue                READ hue                                               CONSTANT )
    Q_PROPERTY( VCNanoleaf * nanoleaf      READ nanoleaf                                          CONSTANT )
    Q_PROPERTY( VCPiHole * piHole          READ piHole                                            CONSTANT )
    Q_PROPERTY( VCWeather * weather        READ weather                                           CONSTANT )
    Q_PROPERTY( VCFacts * facts            READ facts                                             CONSTANT )
    Q_PROPERTY( VCSpotify * spotify        READ spotify                                           CONSTANT )
    Q_PROPERTY( QVariantList scenes        READ scenes              MEMBER m_Scenes               NOTIFY scenesChanged )
    Q_PROPERTY( QString homeMap            READ homeMap             MEMBER m_HomeMap              NOTIFY homeMapChanged )
    Q_PROPERTY( bool isRunningScene        READ isRunningScene                                    NOTIFY isRunningSceneChanged )

public:
    static VCHub * instance();

    const QDateTime & currentDateTime() const { return m_CurrentDateTime; }
    const QString & ethernetIPAddress() const { return m_EthernetIPAddress; }
    const QString & wifiIPAddress() const { return m_WifiIPAddress; }
    const QString & hostname() const { return m_Hostname; }
    const QString & platform() const { return m_Platform; }
    const QString & architecture() const { return m_Architecture; }
    const QString & qtVersion() const { return m_QtVersion; }
    bool use24HourClock() const { return m_bUse24HourClock; }
    void setUse24HourClock( bool bValue );
    bool darkerBackground() const { return m_bDarkerBackground; }
    bool screensaverEnabled() const { return m_bScreensaverEnabled; }
    VCHue * hue() const { return m_pHue; }
    VCNanoleaf * nanoleaf() const { return m_pNanoleaf; }
    VCPiHole * piHole() const { return m_pPiHole; }
    VCFacts * facts() const { return m_pFacts; }
    VCWeather * weather() const { return m_pWeather; }
    VCSpotify * spotify() const { return m_pSpotify; }
    const QVariantList & scenes() const { return m_Scenes; }
    const QString & homeMap() const { return m_HomeMap; }
    bool isRunningScene() const { return m_bIsRunningScene; }

    bool loadConfig( const QString & Path );

signals:
    void currentDateTimeChanged();
    void ethernetIPAddressChanged();
    void wifiIPAddressChanged();
    void use24HourClockChanged();
    void darkerBackgroundChanged();
    void screensaverEnabledChanged();
    void scenesChanged();
    void homeMapChanged();
    void isRunningSceneChanged();

public slots:
    void runScene( const QString & Scene );
    QStringList parseSceneColors( const QString & Scene );

    QString dayOfWeek( const QDateTime & DateTime ) const;
    QString formatTime( const QDateTime & DateTime ) const;
    QString formatInt( int iValue, const QString & Unit = QString() ) const;
    QString formatDecimal( double dValue, const QString & Unit = QString() ) const;
    QString formatPercentage( double dValue, bool bWholeNumber = false ) const;
    QUrl localFileToURL( const QString & Path ) const { return QUrl::fromLocalFile( Path ); }
    QString screenshotPath() const;

private slots:
    void updateCurrentDateTime();
    void refreshIPAddresses();

private:
    explicit VCHub( QObject * pParent = nullptr );

    QDateTime m_CurrentDateTime;
    QTimer m_CurrentDateTimeRefreshTimer;
    QString m_EthernetIPAddress;
    QString m_WifiIPAddress;
    QTimer m_IPAddressesRefreshTimer;
    QString m_Hostname;
    QString m_Platform;
    QString m_Architecture;
    QString m_QtVersion;
    bool m_bUse24HourClock;
    bool m_bDarkerBackground;
    bool m_bScreensaverEnabled;
    VCHue * m_pHue;
    VCNanoleaf * m_pNanoleaf;
    VCPiHole * m_pPiHole;
    VCWeather * m_pWeather;
    VCFacts * m_pFacts;
    VCSpotify * m_pSpotify;
    QVariantList m_Scenes;
    QString m_HomeMap;
    bool m_bIsRunningScene;
#ifdef QT_DEBUG
    QFileSystemWatcher m_ConfigFileWatcher;
#endif

    QVariantList extractSceneSteps( const QString & Scene );

    Q_DISABLE_COPY_MOVE( VCHub )
};

QObject * vchub_singletontype_provider( QQmlEngine * pEngine, QJSEngine * pScriptEngine );

#endif // VCHUB_H_
