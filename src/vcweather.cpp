#include <QJsonArray>
#include <QJsonObject>

#include "vcweather.h"
#include "networkinterface.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

VCWeather::VCWeather( const QString & Name, QObject * pParent) :
    VCPlugin( Name, pParent ),
    m_dLatitude( qQNaN() ),
    m_dLongitude( qQNaN() ),
    m_dCurrentTemperature( qQNaN() ),
    m_dCurrentFeelsLike( qQNaN() ),
    m_iCurrentHumidity( 0 ),
    m_dCurrentWindSpeed( qQNaN() ),
    m_dHour1Temperature( qQNaN() ),
    m_dHour2Temperature( qQNaN() ),
    m_dHour3Temperature( qQNaN() ),
    m_dHour4Temperature( qQNaN() ),
    m_dHour5Temperature( qQNaN() ),
    m_dHour6Temperature( qQNaN() ),
    m_dDay1TemperatureMin( qQNaN() ),
    m_dDay1TemperatureMax( qQNaN() ),
    m_dDay2TemperatureMin( qQNaN() ),
    m_dDay2TemperatureMax( qQNaN() ),
    m_dDay3TemperatureMin( qQNaN() ),
    m_dDay3TemperatureMax( qQNaN() ),
    m_dDay4TemperatureMin( qQNaN() ),
    m_dDay4TemperatureMax( qQNaN() ),
    m_dDay5TemperatureMin( qQNaN() ),
    m_dDay5TemperatureMax( qQNaN() )
{
    setUpdateInterval( 5 * 60 * 1000 );
    m_UpdateTimer.stop();

    // Handle network responses.
    connect( NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCWeather::handleNetworkReply );

    // Update the URL whenever dependent properties change.
    connect( this, &VCWeather::latitudeChanged, this , &VCWeather::updateDestinationURL );
    connect( this, &VCWeather::longitudeChanged, this , &VCWeather::updateDestinationURL );
    connect( this, &VCWeather::apiKeyChanged, this , &VCWeather::updateDestinationURL );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::refresh()
{
#ifndef QT_DEBUG
    // BDP: Be mindful of the API rate limits.
    NetworkInterface::instance()->sendJSONRequest( m_Destination, this );
#endif
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCWeather::localHour( const QDateTime & DateTime )
{
    return DateTime.toString( VCHub::instance()->use24HourClock() ? "hh:00" : "h AP" );
}
/*--------------------------------------------------------------------------------------------------------------------*/

QUrl VCWeather::iconURL( const QString & Key )
{
    if ( !Key.isEmpty() )
    {
        // Use our own variant of the relevant icon.
        return QUrl( QString( "qrc:/images/weather-%1.svg" ).arg( Key ) );
    }
    return QUrl();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body )
{
    if ( this == pSender )
    {
        if ( 200 == iStatusCode )
        {
            if ( Body.isObject() )
            {
                QJsonObject ResponseObject = Body.object();
                if ( ResponseObject.contains( "current" ) )
                {
                    QJsonObject CurrentObject = ResponseObject.value( "current" ).toObject();
                    if ( CurrentObject.contains( "temp" ) )
                    {
                        double dCurrentTemperature = CurrentObject.value( "temp" ).toDouble();
                        if ( m_dCurrentTemperature != dCurrentTemperature )
                        {
                            m_dCurrentTemperature = dCurrentTemperature;
                            emit currentTemperatureChanged();
                        }
                    }
                    if ( CurrentObject.contains( "feels_like" ) )
                    {
                        double dCurrentFeelsLike = CurrentObject.value( "feels_like" ).toDouble();
                        if ( m_dCurrentFeelsLike != dCurrentFeelsLike )
                        {
                            m_dCurrentFeelsLike = dCurrentFeelsLike;
                            emit currentFeelsLikeChanged();
                        }
                    }
                    if ( CurrentObject.contains( "humidity" ) )
                    {
                        int iCurrentHumidity = CurrentObject.value( "humidity" ).toInt();
                        if ( m_iCurrentHumidity != iCurrentHumidity )
                        {
                            m_iCurrentHumidity = iCurrentHumidity;
                            emit currentHumidityChanged();
                        }
                    }
                    if ( CurrentObject.contains( "wind_speed" ) )
                    {
                        double dCurrentWindSpeed = CurrentObject.value( "wind_speed" ).toDouble();
                        if ( m_dCurrentWindSpeed != dCurrentWindSpeed )
                        {
                            m_dCurrentWindSpeed = dCurrentWindSpeed;
                            emit currentWindSpeedChanged();
                        }
                    }
                    if ( CurrentObject.contains( "sunrise" ) )
                    {
                        QDateTime SunriseTime = QDateTime::fromSecsSinceEpoch( CurrentObject.value( "sunrise" ).toInt() );
                        if ( m_SunriseTime != SunriseTime )
                        {
                            m_SunriseTime = SunriseTime;
                            emit sunriseTimeChanged();
                        }
                    }
                    if ( CurrentObject.contains( "sunset" ) )
                    {
                        QDateTime SunsetTime = QDateTime::fromSecsSinceEpoch( CurrentObject.value( "sunset" ).toInt() );
                        if ( m_SunsetTime != SunsetTime )
                        {
                            m_SunsetTime = SunsetTime;
                            emit sunsetTimeChanged();
                        }
                    }
                    if ( CurrentObject.contains( "weather" ) )
                    {
                        // Only be concerned with the first value in the array.
                        QJsonArray CurrentWeatherArray = CurrentObject.value( "weather" ).toArray();
                        if ( !CurrentWeatherArray.isEmpty() )
                        {
                            QJsonObject CurrentWeatherObject = CurrentWeatherArray.first().toObject();
                            if ( CurrentWeatherObject.contains( "main" ) )
                            {
                                QString CurrentCondition = CurrentWeatherObject.value( "main" ).toString();
                                if ( m_CurrentCondition != CurrentCondition )
                                {
                                    m_CurrentCondition = CurrentCondition;
                                    emit currentConditionChanged();
                                }
                            }
                            if ( CurrentWeatherObject.contains( "icon" ) )
                            {
                                QString CurrentIconKey = CurrentWeatherObject.value( "icon" ).toString();
                                if ( m_CurrentIconKey != CurrentIconKey )
                                {
                                    m_CurrentIconKey = CurrentIconKey;
                                    emit currentIconKeyChanged();
                                }
                            }
                        }
                    }
                }
                if ( ResponseObject.contains( "hourly" ) )
                {
                    // Only be concerned with the first 6 hours of forecast.
                    QJsonArray HourlyArray = ResponseObject.value( "hourly" ).toArray();
                    for ( int i = 0; i < 6; i++ )
                    {
                        processHourlyObject( i + 1, HourlyArray.at( i ).toObject() );
                    }
                }
                if ( ResponseObject.contains( "daily" ) )
                {
                    // Only be concerned with the first 5 days of forecast.
                    QJsonArray DailyArray = ResponseObject.value( "daily" ).toArray();
                    for ( int i = 0; i < 5; i++ )
                    {
                        processDailyObject( i + 1, DailyArray.at( i ).toObject() );
                    }
                }
            }
            else
            {
                qDebug() << "Failed to parse response from weather server";
            }
        }
        else
        {
            qDebug() << "Ignoring unsuccessful reply from weather server with status code: " << iStatusCode;
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::updateDestinationURL()
{
    if ( ( !m_APIKey.isEmpty() ) && ( !qIsNaN( m_dLatitude ) ) && ( !qIsNaN( m_dLongitude ) ) )
    {
        m_Destination = QUrl( QString( "https://api.openweathermap.org/data/2.5/"
                                       "onecall?lat=%1&lon=%2&appid=%3&units=imperial&exclude=minutely" )
                                  .arg( m_dLatitude )
                                  .arg( m_dLongitude )
                                  .arg( m_APIKey ) );

        // With everything needed to make requests collected, start the update timer and refesh immediately.
        m_UpdateTimer.start();
        refresh();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::processHourlyObject( const int iIndex, const QJsonObject & Data )
{
    // Since the properties are named predictably, set them generically instead of being exhaustive and repetitive.
    if ( Data.contains( "dt" ) )
    {
        QDateTime Time = QDateTime::fromSecsSinceEpoch( Data.value( "dt" ).toInt() );
        QString PropertyName = QString( "hour%1Time" ).arg( iIndex );
        setProperty( PropertyName.toStdString().c_str(), Time );
    }
    if ( Data.contains( "temp" ) )
    {
        QString PropertyName = QString( "hour%1Temperature" ).arg( iIndex );
        setProperty( PropertyName.toStdString().c_str(), Data.value( "temp" ).toDouble() );
    }
    if ( Data.contains( "weather" ) )
    {
        QJsonArray WeatherArray = Data.value( "weather" ).toArray();
        if ( !WeatherArray.isEmpty() )
        {
            QJsonObject WeatherObject = WeatherArray.first().toObject();
            if ( WeatherObject.contains( "icon" ) )
            {
                QString PropertyName = QString( "hour%1IconKey" ).arg( iIndex );
                setProperty( PropertyName.toStdString().c_str(), WeatherObject.value( "icon" ).toString() );
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCWeather::processDailyObject( const int iIndex, const QJsonObject & Data )
{
    // Same rationale as the hourly object processing.
    if ( Data.contains( "dt" ) )
    {
        QDateTime Time = QDateTime::fromSecsSinceEpoch( Data.value( "dt" ).toInt() );
        QString PropertyName = QString( "day%1Time" ).arg( iIndex );
        setProperty( PropertyName.toStdString().c_str(), Time );
    }
    if ( Data.contains( "temp" ) )
    {
        QJsonObject TempObject = Data.value( "temp" ).toObject();
        QString BasePropertyName = QString( "day%1Temperature" ).arg( iIndex );
        if ( TempObject.contains( "min" ) )
        {
            setProperty( QString( "%1Min" ).arg( BasePropertyName ).toStdString().c_str(),
                         TempObject.value( "min" ).toDouble() );
        }
        if ( TempObject.contains( "max" ) )
        {
            setProperty( QString( "%1Max" ).arg( BasePropertyName ).toStdString().c_str(),
                         TempObject.value( "max" ).toDouble() );
        }
    }
    if ( Data.contains( "weather" ) )
    {
        QJsonArray WeatherArray = Data.value( "weather" ).toArray();
        if ( !WeatherArray.isEmpty() )
        {
            QJsonObject WeatherObject = WeatherArray.first().toObject();
            if ( WeatherObject.contains( "icon" ) )
            {
                QString PropertyName = QString( "day%1IconKey" ).arg( iIndex );
                setProperty( PropertyName.toStdString().c_str(), WeatherObject.value( "icon" ).toString() );
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
