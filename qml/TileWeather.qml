import QtQuick 2.12
import QtQuick.Controls 2.15
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

        ListView {
            id: hourlyForecast

            Layout.fillWidth: true
            Layout.preferredHeight: 80
            orientation: ListView.Horizontal
            spacing: VCMargin.tiny
            clip: true
            interactive: false  // Not scrollable
            model: VCHub.weather.hourlyForecast

            delegate: ColumnLayout {
                id: hourLayout

                width: 63
                height: hourlyForecast.height

                Text {
                    id: hourTime

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    color: VCColor.white
                    text: VCHub.weather.localHour(modelData["time"])
                }

                Image {
                    id: hourConditionIcon

                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignHCenter
                    fillMode: Image.PreserveAspectFit
                    sourceSize: Qt.size(width, height)
                    source: VCHub.weather.iconURL(modelData["iconKey"])
                }

                Text {
                    id: hourTemperature

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.bold: true
                    color: VCColor.white
                    text: Math.round(modelData["temperature"]) + "°F"
                }

            }

        }

        Rectangle {
            id: lowerSeperator

            Layout.preferredHeight: 1
            Layout.fillWidth: true
            color: VCColor.grayLight
        }

        ListView {
            id: dailyForecast

            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            interactive: contentHeight > height
            model: VCHub.weather.dailyForecast

            ScrollBar.vertical: ScrollBar {
                policy: dailyForecast.interactive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
            }

            delegate: RowLayout {
                width: dailyForecast.width - (dailyForecast.interactive ? VCMargin.medium : 0)  // Leave room for the scrollbar
                spacing: VCMargin.medium

                Text {
                    id: dayTime

                    Layout.preferredWidth: 100
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: VCFont.paragraph
                    color: VCColor.white
                    text: qsTr(VCHub.dayOfWeek(modelData["time"]))
                }

                Image {
                    id: dayConditionIcon

                    Layout.fillWidth: true
                    Layout.preferredHeight: 30
                    Layout.alignment: Qt.AlignVCenter
                    fillMode: Image.PreserveAspectFit
                    sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
                    source: VCHub.weather.iconURL(modelData["iconKey"])
                }

                Text {
                    id: dayTemperatureMax

                    Layout.alignment: Qt.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: VCFont.paragraph
                    color: VCColor.white
                    text: Math.round(modelData["maxTemperature"]) + "°F"
                }

                Text {
                    id: dayTemperatureMin

                    Layout.alignment: Qt.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: VCFont.paragraph
                    color: VCColor.grayLightest
                    text: Math.round(modelData["minTemperature"]) + "°F"
                }

            }

        }

    }

}
