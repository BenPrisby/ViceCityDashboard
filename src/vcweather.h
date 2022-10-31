#ifndef VCWEATHER_H_
#define VCWEATHER_H_

#include <QDateTime>
#include <QUrl>

#include "vcplugin.h"

class VCWeather final : public VCPlugin
{
    Q_OBJECT
    Q_PROPERTY( double latitude            READ latitude            MEMBER latitude_            NOTIFY latitudeChanged )
    Q_PROPERTY( double longitude           READ longitude           MEMBER longitude_           NOTIFY longitudeChanged )
    Q_PROPERTY( double currentTemperature  READ currentTemperature                                NOTIFY currentTemperatureChanged )
    Q_PROPERTY( QString currentCondition   READ currentCondition                                  NOTIFY currentConditionChanged )
    Q_PROPERTY( double currentFeelsLike    READ currentFeelsLike                                  NOTIFY currentFeelsLikeChanged )
    Q_PROPERTY( int currentHumidity        READ currentHumidity                                   NOTIFY currentHumidityChanged )
    Q_PROPERTY( double currentWindSpeed    READ currentWindSpeed                                  NOTIFY currentWindSpeedChanged )
    Q_PROPERTY( QString currentIconKey     READ currentIconKey                                    NOTIFY currentIconKeyChanged )
    Q_PROPERTY( QDateTime sunriseTime      READ sunriseTime                                       NOTIFY sunriseTimeChanged )
    Q_PROPERTY( QDateTime sunsetTime       READ sunsetTime                                        NOTIFY sunsetTimeChanged )
    Q_PROPERTY( QDateTime hour1Time        READ hour1Time           MEMBER hour1Time_            NOTIFY hour1TimeChanged )
    Q_PROPERTY( QString hour1IconKey       READ hour1IconKey        MEMBER hour1IconKey_         NOTIFY hour1IconKeyChanged )
    Q_PROPERTY( double hour1Temperature    READ hour1Temperature    MEMBER hour1Temperature_    NOTIFY hour1TemperatureChanged )
    Q_PROPERTY( QDateTime hour2Time        READ hour2Time           MEMBER hour2Time_            NOTIFY hour2TimeChanged )
    Q_PROPERTY( QString hour2IconKey       READ hour2IconKey        MEMBER hour2IconKey_         NOTIFY hour2IconKeyChanged )
    Q_PROPERTY( double hour2Temperature    READ hour2Temperature    MEMBER hour2Temperature_    NOTIFY hour2TemperatureChanged )
    Q_PROPERTY( QDateTime hour3Time        READ hour3Time           MEMBER hour3Time_            NOTIFY hour3TimeChanged )
    Q_PROPERTY( QString hour3IconKey       READ hour3IconKey        MEMBER hour3IconKey_         NOTIFY hour3IconKeyChanged )
    Q_PROPERTY( double hour3Temperature    READ hour3Temperature    MEMBER hour3Temperature_    NOTIFY hour3TemperatureChanged )
    Q_PROPERTY( QDateTime hour4Time        READ hour4Time           MEMBER hour4Time_            NOTIFY hour4TimeChanged )
    Q_PROPERTY( QString hour4IconKey       READ hour4IconKey        MEMBER hour4IconKey_         NOTIFY hour4IconKeyChanged )
    Q_PROPERTY( double hour4Temperature    READ hour4Temperature    MEMBER hour4Temperature_    NOTIFY hour4TemperatureChanged )
    Q_PROPERTY( QDateTime hour5Time        READ hour5Time           MEMBER hour5Time_            NOTIFY hour5TimeChanged )
    Q_PROPERTY( QString hour5IconKey       READ hour5IconKey        MEMBER hour5IconKey_         NOTIFY hour5IconKeyChanged )
    Q_PROPERTY( double hour5Temperature    READ hour5Temperature    MEMBER hour5Temperature_    NOTIFY hour5TemperatureChanged )
    Q_PROPERTY( QDateTime hour6Time        READ hour6Time           MEMBER hour6Time_            NOTIFY hour6TimeChanged )
    Q_PROPERTY( QString hour6IconKey       READ hour6IconKey        MEMBER hour6IconKey_         NOTIFY hour6IconKeyChanged )
    Q_PROPERTY( double hour6Temperature    READ hour6Temperature    MEMBER hour6Temperature_    NOTIFY hour6TemperatureChanged )
    Q_PROPERTY( QDateTime day1Time         READ day1Time            MEMBER day1Time_             NOTIFY day1TimeChanged )
    Q_PROPERTY( QString day1IconKey        READ day1IconKey         MEMBER day1IconKey_          NOTIFY day1IconKeyChanged )
    Q_PROPERTY( double day1TemperatureMin  READ day1TemperatureMin  MEMBER day1TemperatureMin_  NOTIFY day1TemperatureMinChanged )
    Q_PROPERTY( double day1TemperatureMax  READ day1TemperatureMax  MEMBER day1TemperatureMax_  NOTIFY day1TemperatureMaxChanged )
    Q_PROPERTY( QDateTime day2Time         READ day2Time            MEMBER day2Time_             NOTIFY day2TimeChanged )
    Q_PROPERTY( QString day2IconKey        READ day2IconKey         MEMBER day2IconKey_          NOTIFY day2IconKeyChanged )
    Q_PROPERTY( double day2TemperatureMin  READ day2TemperatureMin  MEMBER day2TemperatureMin_  NOTIFY day2TemperatureMinChanged )
    Q_PROPERTY( double day2TemperatureMax  READ day2TemperatureMax  MEMBER day2TemperatureMax_  NOTIFY day2TemperatureMaxChanged )
    Q_PROPERTY( QDateTime day3Time         READ day3Time            MEMBER day3Time_             NOTIFY day3TimeChanged )
    Q_PROPERTY( QString day3IconKey        READ day3IconKey         MEMBER day3IconKey_          NOTIFY day3IconKeyChanged )
    Q_PROPERTY( double day3TemperatureMin  READ day3TemperatureMin  MEMBER day3TemperatureMin_  NOTIFY day3TemperatureMinChanged )
    Q_PROPERTY( double day3TemperatureMax  READ day3TemperatureMax  MEMBER day3TemperatureMax_  NOTIFY day3TemperatureMaxChanged )
    Q_PROPERTY( QDateTime day4Time         READ day4Time            MEMBER day4Time_             NOTIFY day4TimeChanged )
    Q_PROPERTY( QString day4IconKey        READ day4IconKey         MEMBER day4IconKey_          NOTIFY day4IconKeyChanged )
    Q_PROPERTY( double day4TemperatureMin  READ day4TemperatureMin  MEMBER day4TemperatureMin_  NOTIFY day4TemperatureMinChanged )
    Q_PROPERTY( double day4TemperatureMax  READ day4TemperatureMax  MEMBER day4TemperatureMax_  NOTIFY day4TemperatureMaxChanged )
    Q_PROPERTY( QDateTime day5Time         READ day5Time            MEMBER day5Time_             NOTIFY day5TimeChanged )
    Q_PROPERTY( QString day5IconKey        READ day5IconKey         MEMBER day5IconKey_          NOTIFY day5IconKeyChanged )
    Q_PROPERTY( double day5TemperatureMin  READ day5TemperatureMin  MEMBER day5TemperatureMin_  NOTIFY day5TemperatureMinChanged )
    Q_PROPERTY( double day5TemperatureMax  READ day5TemperatureMax  MEMBER day5TemperatureMax_  NOTIFY day5TemperatureMaxChanged )
    Q_PROPERTY( QString apiKey             MEMBER apiKey_                                         NOTIFY apiKeyChanged )

public:
    explicit VCWeather( const QString & name, QObject * parent = nullptr );

    double latitude() const { return latitude_; }
    double longitude() const { return longitude_; }
    double currentTemperature() const { return currentTemperature_; }
    const QUrl & currentIconURL() const { return currentIconURL_; }
    const QString & currentCondition() const { return currentCondition_; }
    double currentFeelsLike() const { return currentFeelsLike_; }
    int currentHumidity() const { return currentHumidity_; }
    double currentWindSpeed() const { return currentWindSpeed_; }
    const QString & currentIconKey() const { return currentIconKey_; }
    const QDateTime & sunriseTime() const { return sunriseTime_; }
    const QDateTime & sunsetTime() const { return sunsetTime_; }
    const QDateTime & hour1Time() const { return hour1Time_; }
    const QString & hour1IconKey() const { return hour1IconKey_; }
    double hour1Temperature() const { return hour1Temperature_; }
    const QDateTime & hour2Time() const { return hour2Time_; }
    const QString & hour2IconKey() const { return hour2IconKey_; }
    double hour2Temperature() const { return hour2Temperature_; }
    const QDateTime & hour3Time() const { return hour3Time_; }
    const QString & hour3IconKey() const { return hour3IconKey_; }
    double hour3Temperature() const { return hour3Temperature_; }
    const QDateTime & hour4Time() const { return hour4Time_; }
    const QString & hour4IconKey() const { return hour4IconKey_; }
    double hour4Temperature() const { return hour4Temperature_; }
    const QDateTime & hour5Time() const { return hour5Time_; }
    const QString & hour5IconKey() const { return hour5IconKey_; }
    double hour5Temperature() const { return hour5Temperature_; }
    const QDateTime & hour6Time() const { return hour6Time_; }
    const QString & hour6IconKey() const { return hour6IconKey_; }
    double hour6Temperature() const { return hour6Temperature_; }
    const QDateTime & day1Time() const { return day1Time_; }
    const QString & day1IconKey() const { return day1IconKey_; }
    double day1TemperatureMin() const { return day1TemperatureMin_; }
    double day1TemperatureMax() const { return day1TemperatureMax_; }
    const QDateTime & day2Time() const { return day2Time_; }
    const QString & day2IconKey() const { return day2IconKey_; }
    double day2TemperatureMin() const { return day2TemperatureMin_; }
    double day2TemperatureMax() const { return day2TemperatureMax_; }
    const QDateTime & day3Time() const { return day3Time_; }
    const QString & day3IconKey() const { return day3IconKey_; }
    double day3TemperatureMin() const { return day3TemperatureMin_; }
    double day3TemperatureMax() const { return day3TemperatureMax_; }
    const QDateTime & day4Time() const { return day4Time_; }
    const QString & day4IconKey() const { return day4IconKey_; }
    double day4TemperatureMin() const { return day4TemperatureMin_; }
    double day4TemperatureMax() const { return day4TemperatureMax_; }
    const QDateTime & day5Time() const { return day5Time_; }
    const QString & day5IconKey() const { return day5IconKey_; }
    double day5TemperatureMin() const { return day5TemperatureMin_; }
    double day5TemperatureMax() const { return day5TemperatureMax_; }

public slots:
    void refresh() override;
    QString localHour( const QDateTime & dateTime ) const;
    QUrl iconURL( const QString & key ) const;

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
    void handleNetworkReply( int statusCode, QObject * sender, const QJsonDocument & body );
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

    void processHourlyObject( int index, const QJsonObject & data );
    void processDailyObject( int index, const QJsonObject & data );

    Q_DISABLE_COPY_MOVE( VCWeather )
};

#endif  // VCWEATHER_H_
