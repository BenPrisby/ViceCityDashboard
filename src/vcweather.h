#ifndef VCWEATHER_H_
#define VCWEATHER_H_

#include <QDateTime>
#include <QUrl>

#include "vcplugin.h"

class VCWeather : public VCPlugin
{
    Q_OBJECT
    Q_PROPERTY( double latitude            READ latitude            MEMBER m_dLatitude            NOTIFY latitudeChanged )
    Q_PROPERTY( double longitude           READ longitude           MEMBER m_dLongitude           NOTIFY longitudeChanged )
    Q_PROPERTY( double currentTemperature  READ currentTemperature                                NOTIFY currentTemperatureChanged )
    Q_PROPERTY( QString currentCondition   READ currentCondition                                  NOTIFY currentConditionChanged )
    Q_PROPERTY( double currentFeelsLike    READ currentFeelsLike                                  NOTIFY currentFeelsLikeChanged )
    Q_PROPERTY( int currentHumidity        READ currentHumidity                                   NOTIFY currentHumidityChanged )
    Q_PROPERTY( double currentWindSpeed    READ currentWindSpeed                                  NOTIFY currentWindSpeedChanged )
    Q_PROPERTY( QString currentIconKey     READ currentIconKey                                    NOTIFY currentIconKeyChanged )
    Q_PROPERTY( QDateTime sunriseTime      READ sunriseTime                                       NOTIFY sunriseTimeChanged )
    Q_PROPERTY( QDateTime sunsetTime       READ sunsetTime                                        NOTIFY sunsetTimeChanged )
    Q_PROPERTY( QDateTime hour1Time        READ hour1Time           MEMBER m_Hour1Time            NOTIFY hour1TimeChanged )
    Q_PROPERTY( QString hour1IconKey       READ hour1IconKey        MEMBER m_Hour1IconKey         NOTIFY hour1IconKeyChanged )
    Q_PROPERTY( double hour1Temperature    READ hour1Temperature    MEMBER m_dHour1Temperature    NOTIFY hour1TemperatureChanged )
    Q_PROPERTY( QDateTime hour2Time        READ hour2Time           MEMBER m_Hour2Time            NOTIFY hour2TimeChanged )
    Q_PROPERTY( QString hour2IconKey       READ hour2IconKey        MEMBER m_Hour2IconKey         NOTIFY hour2IconKeyChanged )
    Q_PROPERTY( double hour2Temperature    READ hour2Temperature    MEMBER m_dHour2Temperature    NOTIFY hour2TemperatureChanged )
    Q_PROPERTY( QDateTime hour3Time        READ hour3Time           MEMBER m_Hour3Time            NOTIFY hour3TimeChanged )
    Q_PROPERTY( QString hour3IconKey       READ hour3IconKey        MEMBER m_Hour3IconKey         NOTIFY hour3IconKeyChanged )
    Q_PROPERTY( double hour3Temperature    READ hour3Temperature    MEMBER m_dHour3Temperature    NOTIFY hour3TemperatureChanged )
    Q_PROPERTY( QDateTime hour4Time        READ hour4Time           MEMBER m_Hour4Time            NOTIFY hour4TimeChanged )
    Q_PROPERTY( QString hour4IconKey       READ hour4IconKey        MEMBER m_Hour4IconKey         NOTIFY hour4IconKeyChanged )
    Q_PROPERTY( double hour4Temperature    READ hour4Temperature    MEMBER m_dHour4Temperature    NOTIFY hour4TemperatureChanged )
    Q_PROPERTY( QDateTime hour5Time        READ hour5Time           MEMBER m_Hour5Time            NOTIFY hour5TimeChanged )
    Q_PROPERTY( QString hour5IconKey       READ hour5IconKey        MEMBER m_Hour5IconKey         NOTIFY hour5IconKeyChanged )
    Q_PROPERTY( double hour5Temperature    READ hour5Temperature    MEMBER m_dHour5Temperature    NOTIFY hour5TemperatureChanged )
    Q_PROPERTY( QDateTime hour6Time        READ hour6Time           MEMBER m_Hour6Time            NOTIFY hour6TimeChanged )
    Q_PROPERTY( QString hour6IconKey       READ hour6IconKey        MEMBER m_Hour6IconKey         NOTIFY hour6IconKeyChanged )
    Q_PROPERTY( double hour6Temperature    READ hour6Temperature    MEMBER m_dHour6Temperature    NOTIFY hour6TemperatureChanged )
    Q_PROPERTY( QDateTime day1Time         READ day1Time            MEMBER m_Day1Time             NOTIFY day1TimeChanged )
    Q_PROPERTY( QString day1IconKey        READ day1IconKey         MEMBER m_Day1IconKey          NOTIFY day1IconKeyChanged )
    Q_PROPERTY( double day1TemperatureMin  READ day1TemperatureMin  MEMBER m_dDay1TemperatureMin  NOTIFY day1TemperatureMinChanged )
    Q_PROPERTY( double day1TemperatureMax  READ day1TemperatureMax  MEMBER m_dDay1TemperatureMax  NOTIFY day1TemperatureMaxChanged )
    Q_PROPERTY( QDateTime day2Time         READ day2Time            MEMBER m_Day2Time             NOTIFY day2TimeChanged )
    Q_PROPERTY( QString day2IconKey        READ day2IconKey         MEMBER m_Day2IconKey          NOTIFY day2IconKeyChanged )
    Q_PROPERTY( double day2TemperatureMin  READ day2TemperatureMin  MEMBER m_dDay2TemperatureMin  NOTIFY day2TemperatureMinChanged )
    Q_PROPERTY( double day2TemperatureMax  READ day2TemperatureMax  MEMBER m_dDay2TemperatureMax  NOTIFY day2TemperatureMaxChanged )
    Q_PROPERTY( QDateTime day3Time         READ day3Time            MEMBER m_Day3Time             NOTIFY day3TimeChanged )
    Q_PROPERTY( QString day3IconKey        READ day3IconKey         MEMBER m_Day3IconKey          NOTIFY day3IconKeyChanged )
    Q_PROPERTY( double day3TemperatureMin  READ day3TemperatureMin  MEMBER m_dDay3TemperatureMin  NOTIFY day3TemperatureMinChanged )
    Q_PROPERTY( double day3TemperatureMax  READ day3TemperatureMax  MEMBER m_dDay3TemperatureMax  NOTIFY day3TemperatureMaxChanged )
    Q_PROPERTY( QDateTime day4Time         READ day4Time            MEMBER m_Day4Time             NOTIFY day4TimeChanged )
    Q_PROPERTY( QString day4IconKey        READ day4IconKey         MEMBER m_Day4IconKey          NOTIFY day4IconKeyChanged )
    Q_PROPERTY( double day4TemperatureMin  READ day4TemperatureMin  MEMBER m_dDay4TemperatureMin  NOTIFY day4TemperatureMinChanged )
    Q_PROPERTY( double day4TemperatureMax  READ day4TemperatureMax  MEMBER m_dDay4TemperatureMax  NOTIFY day4TemperatureMaxChanged )
    Q_PROPERTY( QDateTime day5Time         READ day5Time            MEMBER m_Day5Time             NOTIFY day5TimeChanged )
    Q_PROPERTY( QString day5IconKey        READ day5IconKey         MEMBER m_Day5IconKey          NOTIFY day5IconKeyChanged )
    Q_PROPERTY( double day5TemperatureMin  READ day5TemperatureMin  MEMBER m_dDay5TemperatureMin  NOTIFY day5TemperatureMinChanged )
    Q_PROPERTY( double day5TemperatureMax  READ day5TemperatureMax  MEMBER m_dDay5TemperatureMax  NOTIFY day5TemperatureMaxChanged )
    Q_PROPERTY( QString apiKey             MEMBER m_APIKey                                        NOTIFY apiKeyChanged )

public:
    explicit VCWeather( const QString & Name, QObject * pParent = nullptr );

    double latitude() const { return m_dLatitude; }
    double longitude() const { return m_dLongitude; }
    double currentTemperature() const { return m_dCurrentTemperature; }
    const QUrl & currentIconURL() const { return m_CurrentIconURL; }
    const QString & currentCondition() const { return m_CurrentCondition; }
    double currentFeelsLike() const { return m_dCurrentFeelsLike; }
    int currentHumidity() const { return m_iCurrentHumidity; }
    double currentWindSpeed() const { return m_dCurrentWindSpeed; }
    const QString & currentIconKey() const { return m_CurrentIconKey; }
    const QDateTime & sunriseTime() const { return m_SunriseTime; }
    const QDateTime & sunsetTime() const { return m_SunsetTime; }
    const QDateTime & hour1Time() const { return m_Hour1Time; }
    const QString & hour1IconKey() const { return m_Hour1IconKey; }
    double hour1Temperature() const { return m_dHour1Temperature; }
    const QDateTime & hour2Time() const { return m_Hour2Time; }
    const QString & hour2IconKey() const { return m_Hour2IconKey; }
    double hour2Temperature() const { return m_dHour2Temperature; }
    const QDateTime & hour3Time() const { return m_Hour3Time; }
    const QString & hour3IconKey() const { return m_Hour3IconKey; }
    double hour3Temperature() const { return m_dHour3Temperature; }
    const QDateTime & hour4Time() const { return m_Hour4Time; }
    const QString & hour4IconKey() const { return m_Hour4IconKey; }
    double hour4Temperature() const { return m_dHour4Temperature; }
    const QDateTime & hour5Time() const { return m_Hour5Time; }
    const QString & hour5IconKey() const { return m_Hour5IconKey; }
    double hour5Temperature() const { return m_dHour5Temperature; }
    const QDateTime & hour6Time() const { return m_Hour6Time; }
    const QString & hour6IconKey() const { return m_Hour6IconKey; }
    double hour6Temperature() const { return m_dHour6Temperature; }
    const QDateTime & day1Time() const { return m_Day1Time; }
    const QString & day1IconKey() const { return m_Day1IconKey; }
    double day1TemperatureMin() const { return m_dDay1TemperatureMin; }
    double day1TemperatureMax() const { return m_dDay1TemperatureMax; }
    const QDateTime & day2Time() const { return m_Day2Time; }
    const QString & day2IconKey() const { return m_Day2IconKey; }
    double day2TemperatureMin() const { return m_dDay2TemperatureMin; }
    double day2TemperatureMax() const { return m_dDay2TemperatureMax; }
    const QDateTime & day3Time() const { return m_Day3Time; }
    const QString & day3IconKey() const { return m_Day3IconKey; }
    double day3TemperatureMin() const { return m_dDay3TemperatureMin; }
    double day3TemperatureMax() const { return m_dDay3TemperatureMax; }
    const QDateTime & day4Time() const { return m_Day4Time; }
    const QString & day4IconKey() const { return m_Day4IconKey; }
    double day4TemperatureMin() const { return m_dDay4TemperatureMin; }
    double day4TemperatureMax() const { return m_dDay4TemperatureMax; }
    const QDateTime & day5Time() const { return m_Day5Time; }
    const QString & day5IconKey() const { return m_Day5IconKey; }
    double day5TemperatureMin() const { return m_dDay5TemperatureMin; }
    double day5TemperatureMax() const { return m_dDay5TemperatureMax; }

public slots:
    void refresh() override;
    QString localHour( const QDateTime & DateTime );
    QUrl iconURL( const QString & Key );

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
    void handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body );
    void updateDestinationURL();

private:
    double m_dLatitude;
    double m_dLongitude;
    double m_dCurrentTemperature;
    QUrl m_CurrentIconURL;
    QString m_CurrentCondition;
    double m_dCurrentFeelsLike;
    int m_iCurrentHumidity;
    double m_dCurrentWindSpeed;
    QString m_CurrentIconKey;
    QDateTime m_SunriseTime;
    QDateTime m_SunsetTime;
    QDateTime m_Hour1Time;
    QString m_Hour1IconKey;
    double m_dHour1Temperature;
    QDateTime m_Hour2Time;
    QString m_Hour2IconKey;
    double m_dHour2Temperature;
    QDateTime m_Hour3Time;
    QString m_Hour3IconKey;
    double m_dHour3Temperature;
    QDateTime m_Hour4Time;
    QString m_Hour4IconKey;
    double m_dHour4Temperature;
    QDateTime m_Hour5Time;
    QString m_Hour5IconKey;
    double m_dHour5Temperature;
    QDateTime m_Hour6Time;
    QString m_Hour6IconKey;
    double m_dHour6Temperature;
    QDateTime m_Day1Time;
    QString m_Day1IconKey;
    double m_dDay1TemperatureMin;
    double m_dDay1TemperatureMax;
    QDateTime m_Day2Time;
    QString m_Day2IconKey;
    double m_dDay2TemperatureMin;
    double m_dDay2TemperatureMax;
    QDateTime m_Day3Time;
    QString m_Day3IconKey;
    double m_dDay3TemperatureMin;
    double m_dDay3TemperatureMax;
    QDateTime m_Day4Time;
    QString m_Day4IconKey;
    double m_dDay4TemperatureMin;
    double m_dDay4TemperatureMax;
    QDateTime m_Day5Time;
    QString m_Day5IconKey;
    double m_dDay5TemperatureMin;
    double m_dDay5TemperatureMax;
    QString m_APIKey;

    QUrl m_Destination;

    void processHourlyObject( int iIndex, const QJsonObject & Data );
    void processDailyObject( int iIndex, const QJsonObject & Data );

    Q_DISABLE_COPY_MOVE( VCWeather )
};

#endif  // VCWEATHER_H_
