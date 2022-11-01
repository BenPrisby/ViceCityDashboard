import QtQuick 2.12
import QtQuick.Layouts 1.12
import VCStyles 1.0
import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    ColumnLayout {
        id: contentLayout

        anchors.fill: parent
        anchors.margins: VCMargin.small
        spacing: VCMargin.small

        RowLayout {
            id: currentConditionLayout

            Layout.fillWidth: true
            Layout.maximumHeight: 50
            spacing: 0

            Text {
                id: currentTemperature

                Layout.fillHeight: true
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.header
                color: VCColor.white
                text: Math.round(VCHub.weather.currentTemperature) + "°F"
            }

            Image {
                id: currentConditionIcon

                Layout.fillWidth: true
                Layout.fillHeight: true
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(width, height)
                source: VCHub.weather.iconURL(VCHub.weather.currentIconKey)
            }

            Text {
                id: currentCondition

                Layout.fillHeight: true
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.subHeader
                color: VCColor.white
                text: VCHub.weather.currentCondition
            }

        }

        RowLayout {
            id: currentConditionDetailsLayout

            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: VCMargin.tiny

            ColumnLayout {
                id: feelsLikeLayout

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: feelsLikeLabel

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr("FEELS LIKE")
                }

                Text {
                    id: feelsLikeValue

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.paragraph
                    font.bold: true
                    color: VCColor.white
                    text: Math.round(VCHub.weather.currentFeelsLike) + "°F"
                }

            }

            ColumnLayout {
                id: humidityLayout

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: humidityLabel

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr("HUMIDITY")
                }

                Text {
                    id: humidityValue

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.paragraph
                    font.bold: true
                    color: VCColor.white
                    text: VCHub.weather.currentHumidity + "%"
                }

            }

            ColumnLayout {
                id: windLayout

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: windLabel

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr("WIND")
                }

                Text {
                    id: windValue

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.paragraph
                    font.bold: true
                    color: VCColor.white
                    text: Math.round(VCHub.weather.currentWindSpeed) + " mph"
                }

            }

            ColumnLayout {
                id: sunriseSunsetLayout

                property bool showSunrise: {
                    var now = new Date().getTime();
                    if ((VCHub.weather.sunriseTime.getTime() < now) && (VCHub.weather.sunsetTime.getTime() > now)) {
                        // Sunrise passed, but not sunset.
                        return false;
                    }
                    return true;
                }

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: sunriseSunsetLabel

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr(parent.showSunrise ? "SUNRISE" : "SUNSET")
                }

                Text {
                    id: sunriseSunsetTime

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.paragraph
                    font.bold: true
                    color: VCColor.white
                    text: VCHub.formatTime(parent.showSunrise ? VCHub.weather.sunriseTime : VCHub.weather.sunsetTime)
                }

            }

        }

        Rectangle {
            id: upperSeperator

            Layout.preferredHeight: 1
            Layout.fillWidth: true
            color: VCColor.grayLight
        }

        RowLayout {
            id: hourlyForecastLayout

            Layout.fillWidth: true
            Layout.maximumHeight: 80
            spacing: VCMargin.tiny

            ColumnLayout {
                id: hour1Layout

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: hour1Time

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    color: VCColor.white
                    text: VCHub.weather.localHour(VCHub.weather.hour1Time)
                }

                Image {
                    id: hour1ConditionIcon

                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignHCenter
                    fillMode: Image.PreserveAspectFit
                    sourceSize: Qt.size(width, height)
                    source: VCHub.weather.iconURL(VCHub.weather.hour1IconKey)
                }

                Text {
                    id: hour1Temperature

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.bold: true
                    color: VCColor.white
                    text: Math.round(VCHub.weather.hour1Temperature) + "°F"
                }

            }

            ColumnLayout {
                id: hour2Layout

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: hour2Time

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    color: VCColor.white
                    text: VCHub.weather.localHour(VCHub.weather.hour2Time)
                }

                Image {
                    id: hour2ConditionIcon

                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignHCenter
                    sourceSize: Qt.size(width, height)
                    source: VCHub.weather.iconURL(VCHub.weather.hour2IconKey)
                }

                Text {
                    id: hour2Temperature

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.bold: true
                    color: VCColor.white
                    text: Math.round(VCHub.weather.hour2Temperature) + "°F"
                }

            }

            ColumnLayout {
                id: hour3Layout

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: hour3Time

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    color: VCColor.white
                    text: VCHub.weather.localHour(VCHub.weather.hour3Time)
                }

                Image {
                    id: hour3ConditionIcon

                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignHCenter
                    fillMode: Image.PreserveAspectFit
                    sourceSize: Qt.size(width, height)
                    source: VCHub.weather.iconURL(VCHub.weather.hour3IconKey)
                }

                Text {
                    id: hour3Temperature

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.bold: true
                    color: VCColor.white
                    text: Math.round(VCHub.weather.hour3Temperature) + "°F"
                }

            }

            ColumnLayout {
                id: hour4Layout

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: hour4Time

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    color: VCColor.white
                    text: VCHub.weather.localHour(VCHub.weather.hour4Time)
                }

                Image {
                    id: hour4ConditionIcon

                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignHCenter
                    fillMode: Image.PreserveAspectFit
                    sourceSize: Qt.size(width, height)
                    source: VCHub.weather.iconURL(VCHub.weather.hour4IconKey)
                }

                Text {
                    id: hour4Temperature

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.bold: true
                    color: VCColor.white
                    text: Math.round(VCHub.weather.hour4Temperature) + "°F"
                }

            }

            ColumnLayout {
                id: hour5Layout

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: hour5Time

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    color: VCColor.white
                    text: VCHub.weather.localHour(VCHub.weather.hour5Time)
                }

                Image {
                    id: hour5ConditionIcon

                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignHCenter
                    fillMode: Image.PreserveAspectFit
                    sourceSize: Qt.size(width, height)
                    source: VCHub.weather.iconURL(VCHub.weather.hour5IconKey)
                }

                Text {
                    id: hour5Temperature

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.bold: true
                    color: VCColor.white
                    text: Math.round(VCHub.weather.hour5Temperature) + "°F"
                }

            }

            ColumnLayout {
                id: hour6Layout

                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: hour6Time

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    color: VCColor.white
                    text: VCHub.weather.localHour(VCHub.weather.hour6Time)
                }

                Image {
                    id: hour6ConditionIcon

                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignHCenter
                    fillMode: Image.PreserveAspectFit
                    sourceSize: Qt.size(width, height)
                    source: VCHub.weather.iconURL(VCHub.weather.hour6IconKey)
                }

                Text {
                    id: hour6Temperature

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.bold: true
                    color: VCColor.white
                    text: Math.round(VCHub.weather.hour6Temperature) + "°F"
                }

            }

        }

        Rectangle {
            id: lowerSeperator

            Layout.preferredHeight: 1
            Layout.fillWidth: true
            color: VCColor.grayLight
        }

        GridLayout {
            id: dailyForecastLayout

            Layout.fillWidth: true
            columns: 6
            columnSpacing: VCMargin.medium
            rowSpacing: 0

            Text {
                id: day1Time

                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: qsTr(VCHub.dayOfWeek(VCHub.weather.day1Time))
            }

            Item {
                Layout.fillWidth: true
            }

            Image {
                id: day1ConditionIcon

                Layout.preferredWidth: 30
                Layout.preferredHeight: width
                Layout.alignment: Qt.AlignVCenter
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
                source: VCHub.weather.iconURL(VCHub.weather.day1IconKey)
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                id: day1TemperatureMax

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: Math.round(VCHub.weather.day1TemperatureMax) + "°F"
            }

            Text {
                id: day1TemperatureMin

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.grayLightest
                text: Math.round(VCHub.weather.day1TemperatureMin) + "°F"
            }

            Text {
                id: day2Time

                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: qsTr(VCHub.dayOfWeek(VCHub.weather.day2Time))
            }

            Item {
                Layout.fillWidth: true
            }

            Image {
                id: day2ConditionIcon

                Layout.preferredWidth: 30
                Layout.preferredHeight: width
                Layout.alignment: Qt.AlignVCenter
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
                source: VCHub.weather.iconURL(VCHub.weather.day2IconKey)
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                id: day2TemperatureMax

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: Math.round(VCHub.weather.day2TemperatureMax) + "°F"
            }

            Text {
                id: day2TemperatureMin

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.grayLightest
                text: Math.round(VCHub.weather.day2TemperatureMin) + "°F"
            }

            Text {
                id: day3Time

                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: qsTr(VCHub.dayOfWeek(VCHub.weather.day3Time))
            }

            Item {
                Layout.fillWidth: true
            }

            Image {
                id: day3ConditionIcon

                Layout.preferredWidth: 30
                Layout.preferredHeight: width
                Layout.alignment: Qt.AlignVCenter
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
                source: VCHub.weather.iconURL(VCHub.weather.day3IconKey)
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                id: day3TemperatureMax

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: Math.round(VCHub.weather.day3TemperatureMax) + "°F"
            }

            Text {
                id: day3TemperatureMin

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.grayLightest
                text: Math.round(VCHub.weather.day3TemperatureMin) + "°F"
            }

            Text {
                id: day4Time

                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: qsTr(VCHub.dayOfWeek(VCHub.weather.day4Time))
            }

            Item {
                Layout.fillWidth: true
            }

            Image {
                id: day4ConditionIcon

                Layout.preferredWidth: 30
                Layout.preferredHeight: width
                Layout.alignment: Qt.AlignVCenter
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
                source: VCHub.weather.iconURL(VCHub.weather.day4IconKey)
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                id: day4TemperatureMax

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: Math.round(VCHub.weather.day4TemperatureMax) + "°F"
            }

            Text {
                id: day4TemperatureMin

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.grayLightest
                text: Math.round(VCHub.weather.day4TemperatureMin) + "°F"
            }

            Text {
                id: day5Time

                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: qsTr(VCHub.dayOfWeek(VCHub.weather.day5Time))
            }

            Item {
                Layout.fillWidth: true
            }

            Image {
                id: day5ConditionIcon

                Layout.preferredWidth: 30
                Layout.preferredHeight: width
                Layout.alignment: Qt.AlignVCenter
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
                source: VCHub.weather.iconURL(VCHub.weather.day5IconKey)
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                id: day5TemperatureMax

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.white
                text: Math.round(VCHub.weather.day5TemperatureMax) + "°F"
            }

            Text {
                id: day5TemperatureMin

                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.grayLightest
                text: Math.round(VCHub.weather.day5TemperatureMin) + "°F"
            }

        }

    }

}
