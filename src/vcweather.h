#ifndef VCWEATHER_H_
#define VCWEATHER_H_

#include <QDateTime>
#include <QJsonDocument>
#include <QUrl>

#include "vcplugin.h"

class VCWeather final : public VCPlugin {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(double latitude            MEMBER latitude_            READ latitude            NOTIFY latitudeChanged)
    Q_PROPERTY(double longitude           MEMBER longitude_           READ longitude           NOTIFY longitudeChanged)
    Q_PROPERTY(double currentTemperature                              READ currentTemperature  NOTIFY currentTemperatureChanged)
    Q_PROPERTY(QString currentCondition                               READ currentCondition    NOTIFY currentConditionChanged)
    Q_PROPERTY(double currentFeelsLike                                READ currentFeelsLike    NOTIFY currentFeelsLikeChanged)
    Q_PROPERTY(int currentHumidity                                    READ currentHumidity     NOTIFY currentHumidityChanged)
    Q_PROPERTY(double currentWindSpeed                                READ currentWindSpeed    NOTIFY currentWindSpeedChanged)
    Q_PROPERTY(QString currentIconKey                                 READ currentIconKey      NOTIFY currentIconKeyChanged)
    Q_PROPERTY(QDateTime sunriseTime                                  READ sunriseTime         NOTIFY sunriseTimeChanged)
    Q_PROPERTY(QDateTime sunsetTime                                   READ sunsetTime          NOTIFY sunsetTimeChanged)
    Q_PROPERTY(QDateTime hour1Time        MEMBER hour1Time_           READ hour1Time           NOTIFY hour1TimeChanged)
    Q_PROPERTY(QString hour1IconKey       MEMBER hour1IconKey_        READ hour1IconKey        NOTIFY hour1IconKeyChanged)
    Q_PROPERTY(double hour1Temperature    MEMBER hour1Temperature_    READ hour1Temperature    NOTIFY hour1TemperatureChanged)
    Q_PROPERTY(QDateTime hour2Time        MEMBER hour2Time_           READ hour2Time           NOTIFY hour2TimeChanged)
    Q_PROPERTY(QString hour2IconKey       MEMBER hour2IconKey_        READ hour2IconKey        NOTIFY hour2IconKeyChanged)
    Q_PROPERTY(double hour2Temperature    MEMBER hour2Temperature_    READ hour2Temperature    NOTIFY hour2TemperatureChanged)
    Q_PROPERTY(QDateTime hour3Time        MEMBER hour3Time_           READ hour3Time           NOTIFY hour3TimeChanged)
    Q_PROPERTY(QString hour3IconKey       MEMBER hour3IconKey_        READ hour3IconKey        NOTIFY hour3IconKeyChanged)
    Q_PROPERTY(double hour3Temperature    MEMBER hour3Temperature_    READ hour3Temperature    NOTIFY hour3TemperatureChanged)
    Q_PROPERTY(QDateTime hour4Time        MEMBER hour4Time_           READ hour4Time           NOTIFY hour4TimeChanged)
    Q_PROPERTY(QString hour4IconKey       MEMBER hour4IconKey_        READ hour4IconKey        NOTIFY hour4IconKeyChanged)
    Q_PROPERTY(double hour4Temperature    MEMBER hour4Temperature_    READ hour4Temperature    NOTIFY hour4TemperatureChanged)
    Q_PROPERTY(QDateTime hour5Time        MEMBER hour5Time_           READ hour5Time           NOTIFY hour5TimeChanged)
    Q_PROPERTY(QString hour5IconKey       MEMBER hour5IconKey_        READ hour5IconKey        NOTIFY hour5IconKeyChanged)
    Q_PROPERTY(double hour5Temperature    MEMBER hour5Temperature_    READ hour5Temperature    NOTIFY hour5TemperatureChanged)
    Q_PROPERTY(QDateTime hour6Time        MEMBER hour6Time_           READ hour6Time           NOTIFY hour6TimeChanged)
    Q_PROPERTY(QString hour6IconKey       MEMBER hour6IconKey_        READ hour6IconKey        NOTIFY hour6IconKeyChanged)
    Q_PROPERTY(double hour6Temperature    MEMBER hour6Temperature_    READ hour6Temperature    NOTIFY hour6TemperatureChanged)
    Q_PROPERTY(QDateTime day1Time         MEMBER day1Time_            READ day1Time            NOTIFY day1TimeChanged)
    Q_PROPERTY(QString day1IconKey        MEMBER day1IconKey_         READ day1IconKey         NOTIFY day1IconKeyChanged)
    Q_PROPERTY(double day1TemperatureMin  MEMBER day1TemperatureMin_  READ day1TemperatureMin  NOTIFY day1TemperatureMinChanged)
    Q_PROPERTY(double day1TemperatureMax  MEMBER day1TemperatureMax_  READ day1TemperatureMax  NOTIFY day1TemperatureMaxChanged)
    Q_PROPERTY(QDateTime day2Time         MEMBER day2Time_            READ day2Time            NOTIFY day2TimeChanged)
    Q_PROPERTY(QString day2IconKey        MEMBER day2IconKey_         READ day2IconKey         NOTIFY day2IconKeyChanged)
    Q_PROPERTY(double day2TemperatureMin  MEMBER day2TemperatureMin_  READ day2TemperatureMin  NOTIFY day2TemperatureMinChanged)
    Q_PROPERTY(double day2TemperatureMax  MEMBER day2TemperatureMax_  READ day2TemperatureMax  NOTIFY day2TemperatureMaxChanged)
    Q_PROPERTY(QDateTime day3Time         MEMBER day3Time_            READ day3Time            NOTIFY day3TimeChanged)
    Q_PROPERTY(QString day3IconKey        MEMBER day3IconKey_         READ day3IconKey         NOTIFY day3IconKeyChanged)
    Q_PROPERTY(double day3TemperatureMin  MEMBER day3TemperatureMin_  READ day3TemperatureMin  NOTIFY day3TemperatureMinChanged)
    Q_PROPERTY(double day3TemperatureMax  MEMBER day3TemperatureMax_  READ day3TemperatureMax  NOTIFY day3TemperatureMaxChanged)
    Q_PROPERTY(QDateTime day4Time         MEMBER day4Time_            READ day4Time            NOTIFY day4TimeChanged)
    Q_PROPERTY(QString day4IconKey        MEMBER day4IconKey_         READ day4IconKey         NOTIFY day4IconKeyChanged)
    Q_PROPERTY(double day4TemperatureMin  MEMBER day4TemperatureMin_  READ day4TemperatureMin  NOTIFY day4TemperatureMinChanged)
    Q_PROPERTY(double day4TemperatureMax  MEMBER day4TemperatureMax_  READ day4TemperatureMax  NOTIFY day4TemperatureMaxChanged)
    Q_PROPERTY(QDateTime day5Time         MEMBER day5Time_            READ day5Time            NOTIFY day5TimeChanged)
    Q_PROPERTY(QString day5IconKey        MEMBER day5IconKey_         READ day5IconKey         NOTIFY day5IconKeyChanged)
    Q_PROPERTY(double day5TemperatureMin  MEMBER day5TemperatureMin_  READ day5TemperatureMin  NOTIFY day5TemperatureMinChanged)
    Q_PROPERTY(double day5TemperatureMax  MEMBER day5TemperatureMax_  READ day5TemperatureMax  NOTIFY day5TemperatureMaxChanged)
    Q_PROPERTY(QString apiKey             MEMBER apiKey_                                       NOTIFY apiKeyChanged)
    // clang-format on

 public:
    explicit VCWeather(const QString& name, QObject* parent = nullptr);

    double latitude() const { return latitude_; }
    double longitude() const { return longitude_; }
    double currentTemperature() const { return currentTemperature_; }
    const QUrl& currentIconURL() const { return currentIconURL_; }
    const QString& currentCondition() const { return currentCondition_; }
    double currentFeelsLike() const { return currentFeelsLike_; }
    int currentHumidity() const { return currentHumidity_; }
    double currentWindSpeed() const { return currentWindSpeed_; }
    const QString& currentIconKey() const { return currentIconKey_; }
    const QDateTime& sunriseTime() const { return sunriseTime_; }
    const QDateTime& sunsetTime() const { return sunsetTime_; }
    const QDateTime& hour1Time() const { return hour1Time_; }
    const QString& hour1IconKey() const { return hour1IconKey_; }
    double hour1Temperature() const { return hour1Temperature_; }
    const QDateTime& hour2Time() const { return hour2Time_; }
    const QString& hour2IconKey() const { return hour2IconKey_; }
    double hour2Temperature() const { return hour2Temperature_; }
    const QDateTime& hour3Time() const { return hour3Time_; }
    const QString& hour3IconKey() const { return hour3IconKey_; }
    double hour3Temperature() const { return hour3Temperature_; }
    const QDateTime& hour4Time() const { return hour4Time_; }
    const QString& hour4IconKey() const { return hour4IconKey_; }
    double hour4Temperature() const { return hour4Temperature_; }
    const QDateTime& hour5Time() const { return hour5Time_; }
    const QString& hour5IconKey() const { return hour5IconKey_; }
    double hour5Temperature() const { return hour5Temperature_; }
    const QDateTime& hour6Time() const { return hour6Time_; }
    const QString& hour6IconKey() const { return hour6IconKey_; }
    double hour6Temperature() const { return hour6Temperature_; }
    const QDateTime& day1Time() const { return day1Time_; }
    const QString& day1IconKey() const { return day1IconKey_; }
    double day1TemperatureMin() const { return day1TemperatureMin_; }
    double day1TemperatureMax() const { return day1TemperatureMax_; }
    const QDateTime& day2Time() const { return day2Time_; }
    const QString& day2IconKey() const { return day2IconKey_; }
    double day2TemperatureMin() const { return day2TemperatureMin_; }
    double day2TemperatureMax() const { return day2TemperatureMax_; }
    const QDateTime& day3Time() const { return day3Time_; }
    const QString& day3IconKey() const { return day3IconKey_; }
    double day3TemperatureMin() const { return day3TemperatureMin_; }
    double day3TemperatureMax() const { return day3TemperatureMax_; }
    const QDateTime& day4Time() const { return day4Time_; }
    const QString& day4IconKey() const { return day4IconKey_; }
    double day4TemperatureMin() const { return day4TemperatureMin_; }
    double day4TemperatureMax() const { return day4TemperatureMax_; }
    const QDateTime& day5Time() const { return day5Time_; }
    const QString& day5IconKey() const { return day5IconKey_; }
    double day5TemperatureMin() const { return day5TemperatureMin_; }
    double day5TemperatureMax() const { return day5TemperatureMax_; }

    Q_INVOKABLE QString localHour(const QDateTime& dateTime) const;
    Q_INVOKABLE QUrl iconURL(const QString& key) const;

 public slots:
    void refresh() override;

 signals:
    void latitudeChanged();
    void longitudeChanged();
    void currentTemperatureChanged();
    void currentIconURLChanged();
    void currentConditionChanged();
    void currentFeelsLikeChanged();
    void currentHumidityChanged();
    void currentWindSpeedChanged();
    void currentIconKeyChanged();
    void sunriseTimeChanged();
    void sunsetTimeChanged();
    void hour1TimeChanged();
    void hour1IconKeyChanged();
    void hour1TemperatureChanged();
    void hour2TimeChanged();
    void hour2IconKeyChanged();
    void hour2TemperatureChanged();
    void hour3TimeChanged();
    void hour3IconKeyChanged();
    void hour3TemperatureChanged();
    void hour4TimeChanged();
    void hour4IconKeyChanged();
    void hour4TemperatureChanged();
    void hour5TimeChanged();
    void hour5IconKeyChanged();
    void hour5TemperatureChanged();
    void hour6TimeChanged();
    void hour6IconKeyChanged();
    void hour6TemperatureChanged();
    void day1TimeChanged();
    void day1IconKeyChanged();
    void day1TemperatureMinChanged();
    void day1TemperatureMaxChanged();
    void day2TimeChanged();
    void day2IconKeyChanged();
    void day2TemperatureMinChanged();
    void day2TemperatureMaxChanged();
    void day3TimeChanged();
    void day3IconKeyChanged();
    void day3TemperatureMinChanged();
    void day3TemperatureMaxChanged();
    void day4TimeChanged();
    void day4IconKeyChanged();
    void day4TemperatureMinChanged();
    void day4TemperatureMaxChanged();
    void day5TimeChanged();
    void day5IconKeyChanged();
    void day5TemperatureMinChanged();
    void day5TemperatureMaxChanged();
    void apiKeyChanged();

 private slots:
    void handleNetworkReply(int statusCode, QObject* sender, const QJsonDocument& body);
    void updateDestinationURL();

 private:
    double latitude_;
    double longitude_;
    double currentTemperature_;
    QUrl currentIconURL_;
    QString currentCondition_;
    double currentFeelsLike_;
    int currentHumidity_;
    double currentWindSpeed_;
    QString currentIconKey_;
    QDateTime sunriseTime_;
    QDateTime sunsetTime_;
    QDateTime hour1Time_;
    QString hour1IconKey_;
    double hour1Temperature_;
    QDateTime hour2Time_;
    QString hour2IconKey_;
    double hour2Temperature_;
    QDateTime hour3Time_;
    QString hour3IconKey_;
    double hour3Temperature_;
    QDateTime hour4Time_;
    QString hour4IconKey_;
    double hour4Temperature_;
    QDateTime hour5Time_;
    QString hour5IconKey_;
    double hour5Temperature_;
    QDateTime hour6Time_;
    QString hour6IconKey_;
    double hour6Temperature_;
    QDateTime day1Time_;
    QString day1IconKey_;
    double day1TemperatureMin_;
    double day1TemperatureMax_;
    QDateTime day2Time_;
    QString day2IconKey_;
    double day2TemperatureMin_;
    double day2TemperatureMax_;
    QDateTime day3Time_;
    QString day3IconKey_;
    double day3TemperatureMin_;
    double day3TemperatureMax_;
    QDateTime day4Time_;
    QString day4IconKey_;
    double day4TemperatureMin_;
    double day4TemperatureMax_;
    QDateTime day5Time_;
    QString day5IconKey_;
    double day5TemperatureMin_;
    double day5TemperatureMax_;
    QString apiKey_;

    QUrl destination_;

    void processHourlyObject(int index, const QJsonObject& data);
    void processDailyObject(int index, const QJsonObject& data);

    Q_DISABLE_COPY_MOVE(VCWeather)
};

#endif  // VCWEATHER_H_
