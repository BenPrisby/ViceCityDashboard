#include "vcweather.h"

#include <QJsonArray>
#include <QJsonObject>

#include "networkinterface.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

namespace {
constexpr int MAX_FORECAST_HOURS = 6;
}  // namespace
/*--------------------------------------------------------------------------------------------------------------------*/

VCWeather::VCWeather(const QString& name, QObject* parent)
    : VCPlugin(name, parent),
      latitude_(qQNaN()),
      longitude_(qQNaN()),
      currentTemperature_(qQNaN()),
      currentFeelsLike_(qQNaN()),
      currentHumidity_(0),
      currentWindSpeed_(qQNaN()) {
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

void VCWeather::refresh() {
#ifndef QT_DEBUG
    // BDP: Be mindful of the API rate limits.
    NetworkInterface::instance()->sendJSONRequest(destination_, this);
#endif
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::handleNetworkReply(int statusCode, QObject* sender, const QJsonDocument& body) {
    if (sender != this) {
        // Not for us, ignore.
        return;
    }
    if (statusCode != 200) {
        qDebug() << "Ignoring unsuccessful reply from weather server with status code: " << statusCode;
        return;
    }
    if (!body.isObject()) {
        qDebug() << "Failed to parse response from weather server";
        return;
    }

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
        hourlyForecast_.clear();

        // Build the model with basic forecast information.
        const QJsonArray hourlyArray = responseObject.value("hourly").toArray();
        for (int i = 0, total = qMin(MAX_FORECAST_HOURS, hourlyArray.size()); i < total; i++) {
            QJsonObject hourObject = hourlyArray.at(i).toObject();
            QVariantMap hourForecast;

            if (hourObject.contains("dt")) {
                hourForecast.insert("time", QDateTime::fromSecsSinceEpoch(hourObject.value("dt").toInt()));
            }
            if (hourObject.contains("temp")) {
                hourForecast.insert("temperature", hourObject.value("temp").toDouble());
            }
            if (hourObject.contains("weather")) {
                QJsonArray weatherArray = hourObject.value("weather").toArray();
                if (!weatherArray.isEmpty()) {
                    QJsonObject weatherObject = weatherArray.first().toObject();
                    if (weatherObject.contains("icon")) {
                        hourForecast.insert("iconKey", weatherObject.value("icon").toString());
                    }
                }
            }

            hourlyForecast_.append(hourForecast);
        }

        qDebug() << "Collected " << hourlyForecast_.size() << " hours of forecast data";
        emit hourlyForecastChanged();
    }
    if (responseObject.contains("daily")) {
        dailyForecast_.clear();

        // Build the model with basic forecast information.
        const QJsonArray dailyArray = responseObject.value("daily").toArray();
        for (const auto& day : dailyArray) {
            QJsonObject dayObject = day.toObject();
            QVariantMap dayForecast;

            if (dayObject.contains("dt")) {
                dayForecast.insert("time", QDateTime::fromSecsSinceEpoch(dayObject.value("dt").toInt()));
            }
            if (dayObject.contains("temp")) {
                QJsonObject tempObject = dayObject.value("temp").toObject();
                if (tempObject.contains("min")) {
                    dayForecast.insert("minTemperature", tempObject.value("min").toDouble());
                }
                if (tempObject.contains("max")) {
                    dayForecast.insert("maxTemperature", tempObject.value("max").toDouble());
                }
            }
            if (dayObject.contains("weather")) {
                QJsonArray weatherArray = dayObject.value("weather").toArray();
                if (!weatherArray.isEmpty()) {
                    QJsonObject weatherObject = weatherArray.first().toObject();
                    if (weatherObject.contains("icon")) {
                        dayForecast.insert("iconKey", weatherObject.value("icon").toString());
                    }
                }
            }

            dailyForecast_.append(dayForecast);
        }

        qDebug() << "Collected " << dailyForecast_.size() << " days of forecast data";
        emit dailyForecastChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::updateDestinationURL() {
    if (apiKey_.isEmpty() || qIsNaN(latitude_) || qIsNaN(longitude_)) {
        // Not enough information to build the URL.
        return;
    }

    destination_ = QUrl(QString("https://api.openweathermap.org/data/2.5/"
                                "onecall?lat=%1&lon=%2&appid=%3&units=imperial&exclude=minutely")
                            .arg(latitude_)
                            .arg(longitude_)
                            .arg(apiKey_));

    // With everything needed to make requests collected, start the update timer and refesh immediately.
    updateTimer_.start();
    refresh();
}
/*--------------------------------------------------------------------------------------------------------------------*/
