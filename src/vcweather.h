#ifndef VCWEATHER_H_
#define VCWEATHER_H_

#include <QDateTime>
#include <QJsonDocument>
#include <QUrl>

#include "vcplugin.h"

class VCWeather final : public VCPlugin {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(double latitude              MEMBER latitude_   READ latitude            NOTIFY latitudeChanged)
    Q_PROPERTY(double longitude             MEMBER longitude_  READ longitude           NOTIFY longitudeChanged)
    Q_PROPERTY(double currentTemperature                       READ currentTemperature  NOTIFY currentTemperatureChanged)
    Q_PROPERTY(QString currentCondition                        READ currentCondition    NOTIFY currentConditionChanged)
    Q_PROPERTY(double currentFeelsLike                         READ currentFeelsLike    NOTIFY currentFeelsLikeChanged)
    Q_PROPERTY(int currentHumidity                             READ currentHumidity     NOTIFY currentHumidityChanged)
    Q_PROPERTY(double currentWindSpeed                         READ currentWindSpeed    NOTIFY currentWindSpeedChanged)
    Q_PROPERTY(QString currentIconKey                          READ currentIconKey      NOTIFY currentIconKeyChanged)
    Q_PROPERTY(QDateTime sunriseTime                           READ sunriseTime         NOTIFY sunriseTimeChanged)
    Q_PROPERTY(QDateTime sunsetTime                            READ sunsetTime          NOTIFY sunsetTimeChanged)
    Q_PROPERTY(QVariantList hourlyForecast                     READ hourlyForecast      NOTIFY hourlyForecastChanged)
    Q_PROPERTY(QVariantList dailyForecast                      READ dailyForecast       NOTIFY dailyForecastChanged)
    Q_PROPERTY(QString apiKey               MEMBER apiKey_                              NOTIFY apiKeyChanged)
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
    const QVariantList& hourlyForecast() const { return hourlyForecast_; }
    const QVariantList& dailyForecast() const { return dailyForecast_; }

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
    void hourlyForecastChanged();
    void dailyForecastChanged();
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
    QVariantList hourlyForecast_;  // List of maps containing: time, iconKey, temperate
    QVariantList dailyForecast_;   // List of maps containing: time, iconKey, minTemperature, maxTemperature
    QString apiKey_;

    QUrl destination_;

    Q_DISABLE_COPY_MOVE(VCWeather)
};

#endif  // VCWEATHER_H_
