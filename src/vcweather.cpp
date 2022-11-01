#include "vcweather.h"

#include <QJsonArray>
#include <QJsonObject>

#include "networkinterface.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

VCWeather::VCWeather(const QString& name, QObject* parent)
    : VCPlugin(name, parent),
      latitude_(qQNaN()),
      longitude_(qQNaN()),
      currentTemperature_(qQNaN()),
      currentFeelsLike_(qQNaN()),
      currentHumidity_(0),
      currentWindSpeed_(qQNaN()),
      hour1Temperature_(qQNaN()),
      hour2Temperature_(qQNaN()),
      hour3Temperature_(qQNaN()),
      hour4Temperature_(qQNaN()),
      hour5Temperature_(qQNaN()),
      hour6Temperature_(qQNaN()),
      day1TemperatureMin_(qQNaN()),
      day1TemperatureMax_(qQNaN()),
      day2TemperatureMin_(qQNaN()),
      day2TemperatureMax_(qQNaN()),
      day3TemperatureMin_(qQNaN()),
      day3TemperatureMax_(qQNaN()),
      day4TemperatureMin_(qQNaN()),
      day4TemperatureMax_(qQNaN()),
      day5TemperatureMin_(qQNaN()),
      day5TemperatureMax_(qQNaN()) {
    setUpdateInterval(5 * 60 * 1000);
    updateTimer_.stop();

    // Handle network responses.
    connect(NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCWeather::handleNetworkReply);

    // Update the URL whenever dependent properties change.
    connect(this, &VCWeather::latitudeChanged, this, &VCWeather::updateDestinationURL);
    connect(this, &VCWeather::longitudeChanged, this, &VCWeather::updateDestinationURL);
    connect(this, &VCWeather::apiKeyChanged, this, &VCWeather::updateDestinationURL);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::refresh() {
#ifndef QT_DEBUG
    // BDP: Be mindful of the API rate limits.
    NetworkInterface::instance()->sendJSONRequest(destination_, this);
#endif
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCWeather::localHour(const QDateTime& dateTime) const {
    return dateTime.toString(VCHub::instance()->use24HourClock() ? "hh:00" : "h AP");
}
/*--------------------------------------------------------------------------------------------------------------------*/

QUrl VCWeather::iconURL(const QString& key) const {
    if (!key.isEmpty()) {
        // Use our own variant of the relevant icon.
        return QUrl(QString("qrc:/images/weather-%1.svg").arg(key));
    }
    return QUrl();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::handleNetworkReply(int statusCode, QObject* sender, const QJsonDocument& body) {
    if (sender == this) {
        if (statusCode == 200) {
            if (body.isObject()) {
                QJsonObject responseObject = body.object();
                if (responseObject.contains("current")) {
                    QJsonObject currentObject = responseObject.value("current").toObject();
                    if (currentObject.contains("temp")) {
                        double currentTemperature = currentObject.value("temp").toDouble();
                        if (currentTemperature_ != currentTemperature) {
                            currentTemperature_ = currentTemperature;
                            emit currentTemperatureChanged();
                        }
                    }
                    if (currentObject.contains("feels_like")) {
                        double currentFeelsLike = currentObject.value("feels_like").toDouble();
                        if (currentFeelsLike_ != currentFeelsLike) {
                            currentFeelsLike_ = currentFeelsLike;
                            emit currentFeelsLikeChanged();
                        }
                    }
                    if (currentObject.contains("humidity")) {
                        int currentHumidity = currentObject.value("humidity").toInt();
                        if (currentHumidity_ != currentHumidity) {
                            currentHumidity_ = currentHumidity;
                            emit currentHumidityChanged();
                        }
                    }
                    if (currentObject.contains("wind_speed")) {
                        double currentWindSpeed = currentObject.value("wind_speed").toDouble();
                        if (currentWindSpeed_ != currentWindSpeed) {
                            currentWindSpeed_ = currentWindSpeed;
                            emit currentWindSpeedChanged();
                        }
                    }
                    if (currentObject.contains("sunrise")) {
                        QDateTime sunriseTime = QDateTime::fromSecsSinceEpoch(currentObject.value("sunrise").toInt());
                        if (sunriseTime_ != sunriseTime) {
                            sunriseTime_ = sunriseTime;
                            emit sunriseTimeChanged();
                        }
                    }
                    if (currentObject.contains("sunset")) {
                        QDateTime sunsetTime = QDateTime::fromSecsSinceEpoch(currentObject.value("sunset").toInt());
                        if (sunsetTime_ != sunsetTime) {
                            sunsetTime_ = sunsetTime;
                            emit sunsetTimeChanged();
                        }
                    }
                    if (currentObject.contains("weather")) {
                        // Only be concerned with the first value in the array.
                        QJsonArray currentWeatherArray = currentObject.value("weather").toArray();
                        if (!currentWeatherArray.isEmpty()) {
                            QJsonObject currentWeatherObject = currentWeatherArray.first().toObject();
                            if (currentWeatherObject.contains("main")) {
                                QString currentCondition = currentWeatherObject.value("main").toString();
                                if (currentCondition_ != currentCondition) {
                                    currentCondition_ = currentCondition;
                                    emit currentConditionChanged();
                                }
                            }
                            if (currentWeatherObject.contains("icon")) {
                                QString currentIconKey = currentWeatherObject.value("icon").toString();
                                if (currentIconKey_ != currentIconKey) {
                                    currentIconKey_ = currentIconKey;
                                    emit currentIconKeyChanged();
                                }
                            }
                        }
                    }
                }
                if (responseObject.contains("hourly")) {
                    // Only be concerned with the first 6 hours of forecast.
                    QJsonArray hourlyArray = responseObject.value("hourly").toArray();
                    for (int i = 0; i < 6; i++) {
                        processHourlyObject(i + 1, hourlyArray.at(i).toObject());
                    }
                }
                if (responseObject.contains("daily")) {
                    // Only be concerned with the first 5 days of forecast.
                    QJsonArray dailyArray = responseObject.value("daily").toArray();
                    for (int i = 0; i < 5; i++) {
                        processDailyObject(i + 1, dailyArray.at(i).toObject());
                    }
                }
            } else {
                qDebug() << "Failed to parse response from weather server";
            }
        } else {
            qDebug() << "Ignoring unsuccessful reply from weather server with status code: " << statusCode;
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::updateDestinationURL() {
    if (!apiKey_.isEmpty() && !qIsNaN(latitude_) && !qIsNaN(longitude_)) {
        destination_ = QUrl(QString("https://api.openweathermap.org/data/2.5/"
                                    "onecall?lat=%1&lon=%2&appid=%3&units=imperial&exclude=minutely")
                                .arg(latitude_)
                                .arg(longitude_)
                                .arg(apiKey_));

        // With everything needed to make requests collected, start the update timer and refesh immediately.
        updateTimer_.start();
        refresh();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::processHourlyObject(const int index, const QJsonObject& data) {
    // Since the properties are named predictably, set them generically instead of being exhaustive and repetitive.
    if (data.contains("dt")) {
        QDateTime time = QDateTime::fromSecsSinceEpoch(data.value("dt").toInt());
        QString propertyName = QString("hour%1Time").arg(index);
        setProperty(propertyName.toStdString().c_str(), time);
    }
    if (data.contains("temp")) {
        QString propertyName = QString("hour%1Temperature").arg(index);
        setProperty(propertyName.toStdString().c_str(), data.value("temp").toDouble());
    }
    if (data.contains("weather")) {
        QJsonArray weatherArray = data.value("weather").toArray();
        if (!weatherArray.isEmpty()) {
            QJsonObject weatherObject = weatherArray.first().toObject();
            if (weatherObject.contains("icon")) {
                QString propertyName = QString("hour%1IconKey").arg(index);
                setProperty(propertyName.toStdString().c_str(), weatherObject.value("icon").toString());
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::processDailyObject(const int index, const QJsonObject& data) {
    // Same rationale as the hourly object processing.
    if (data.contains("dt")) {
        QDateTime time = QDateTime::fromSecsSinceEpoch(data.value("dt").toInt());
        QString propertyName = QString("day%1Time").arg(index);
        setProperty(propertyName.toStdString().c_str(), time);
    }
    if (data.contains("temp")) {
        QJsonObject tempObject = data.value("temp").toObject();
        QString basePropertyName = QString("day%1Temperature").arg(index);
        if (tempObject.contains("min")) {
            setProperty(QString("%1Min").arg(basePropertyName).toStdString().c_str(),
                        tempObject.value("min").toDouble());
        }
        if (tempObject.contains("max")) {
            setProperty(QString("%1Max").arg(basePropertyName).toStdString().c_str(),
                        tempObject.value("max").toDouble());
        }
    }
    if (data.contains("weather")) {
        QJsonArray weatherArray = data.value("weather").toArray();
        if (!weatherArray.isEmpty()) {
            QJsonObject weatherObject = weatherArray.first().toObject();
            if (weatherObject.contains("icon")) {
                QString propertyName = QString("day%1IconKey").arg(index);
                setProperty(propertyName.toStdString().c_str(), weatherObject.value("icon").toString());
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
