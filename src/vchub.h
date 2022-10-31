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

class VCHub final : public QObject {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(bool isActive                                          READ isActive            WRITE setActive          NOTIFY isActiveChanged)
    Q_PROPERTY(QDateTime currentDateTime  MEMBER currentDateTime_     READ currentDateTime                              NOTIFY currentDateTimeChanged)
    Q_PROPERTY(QString ethernetIPAddress                              READ ethernetIPAddress                            NOTIFY ethernetIPAddressChanged)
    Q_PROPERTY(QString wifiIPAddress                                  READ wifiIPAddress                                NOTIFY wifiIPAddressChanged)
    Q_PROPERTY(QString hostname                                       READ hostname                                     CONSTANT)
    Q_PROPERTY(QString platform                                       READ platform                                     CONSTANT)
    Q_PROPERTY(QString architecture                                   READ architecture                                 CONSTANT)
    Q_PROPERTY(QString qtVersion                                      READ qtVersion                                    CONSTANT)
    Q_PROPERTY(bool use24HourClock                                    READ use24HourClock      WRITE setUse24HourClock  NOTIFY use24HourClockChanged)
    Q_PROPERTY(bool darkerBackground      MEMBER darkerBackground_    READ darkerBackground                             NOTIFY darkerBackgroundChanged)
    Q_PROPERTY(bool screensaverEnabled    MEMBER screensaverEnabled_  READ screensaverEnabled                           NOTIFY screensaverEnabledChanged)
    Q_PROPERTY(VCHue * hue                                            READ hue                                          CONSTANT)
    Q_PROPERTY(VCNanoleaf * nanoleaf                                  READ nanoleaf                                     CONSTANT)
    Q_PROPERTY(VCPiHole * piHole                                      READ piHole                                       CONSTANT)
    Q_PROPERTY(VCWeather * weather                                    READ weather                                      CONSTANT)
    Q_PROPERTY(VCFacts * facts                                        READ facts                                        CONSTANT)
    Q_PROPERTY(VCSpotify * spotify                                    READ spotify                                      CONSTANT)
    Q_PROPERTY(QVariantList scenes        MEMBER scenes_              READ scenes                                       NOTIFY scenesChanged)
    Q_PROPERTY(QString homeMap            MEMBER homeMap_             READ homeMap                                      NOTIFY homeMapChanged)
    Q_PROPERTY(bool isRunningScene                                    READ isRunningScene                               NOTIFY isRunningSceneChanged)
    // clang-format on

 public:
    static VCHub* instance();

    bool isActive() const { return isActive_; }
    void setActive(bool value);
    const QDateTime& currentDateTime() const { return currentDateTime_; }
    const QString& ethernetIPAddress() const { return ethernetIPAddress_; }
    const QString& wifiIPAddress() const { return wifiIPAddress_; }
    const QString& hostname() const { return hostname_; }
    const QString& platform() const { return platform_; }
    const QString& architecture() const { return architecture_; }
    const QString& qtVersion() const { return qtVersion_; }
    bool use24HourClock() const { return use24HourClock_; }
    void setUse24HourClock(bool value);
    bool darkerBackground() const { return darkerBackground_; }
    bool screensaverEnabled() const { return screensaverEnabled_; }
    VCHue* hue() const { return hue_; }
    VCNanoleaf* nanoleaf() const { return nanoleaf_; }
    VCPiHole* piHole() const { return pihole_; }
    VCFacts* facts() const { return facts_; }
    VCWeather* weather() const { return weather_; }
    VCSpotify* spotify() const { return spotify_; }
    const QVariantList& scenes() const { return scenes_; }
    const QString& homeMap() const { return homeMap_; }
    bool isRunningScene() const { return isRunningScene_; }

    bool loadConfig(const QString& path);

 signals:
    void isActiveChanged();
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
    void runScene(const QString& scene);
    QStringList parseSceneColors(const QString& scene);

    QString dayOfWeek(const QDateTime& dateTime) const;
    QString formatTime(const QDateTime& dateTime) const;
    QString formatInt(int value, const QString& unit = QString()) const;
    QString formatDecimal(double value, const QString& unit = QString()) const;
    QString formatPercentage(double value, bool wholeNumber = false) const;
    QUrl localFileToURL(const QString& path) const { return QUrl::fromLocalFile(path); }
    QString screenshotPath() const;

 private slots:
    void updateCurrentDateTime();
    void refreshIPAddresses();

 private:
    explicit VCHub(QObject* parent = nullptr);

    bool isActive_;
    QDateTime currentDateTime_;
    QTimer currentDateTimeRefreshTimer_;
    QString ethernetIPAddress_;
    QString wifiIPAddress_;
    QTimer ipAddressesRefreshTimer_;
    QString hostname_;
    QString platform_;
    QString architecture_;
    QString qtVersion_;
    bool use24HourClock_;
    bool darkerBackground_;
    bool screensaverEnabled_;
    VCHue* hue_;
    VCNanoleaf* nanoleaf_;
    VCPiHole* pihole_;
    VCWeather* weather_;
    VCFacts* facts_;
    VCSpotify* spotify_;
    QVariantList scenes_;
    QString homeMap_;
    bool isRunningScene_;
#ifdef QT_DEBUG
    QFileSystemWatcher configFileWatcher_;
#endif

    QVariantList extractSceneSteps(const QString& scene);

    Q_DISABLE_COPY_MOVE(VCHub)
};

QObject* vchub_singletontype_provider(QQmlEngine* engine, QJSEngine* scriptEngine);

#endif  // VCHUB_H_
