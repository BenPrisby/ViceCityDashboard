import QtQuick 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

Tile {
    id: root

    property var device: null

    Text {
        id: deviceName
        anchors.top: parent.top
        anchors.topMargin: VCMargin.small
        anchors.left: parent.left
        anchors.leftMargin: anchors.topMargin
        font.pixelSize: VCFont.subHeader
        color: VCColor.white
        text: root.device ? root.device[ "name" ] : ""
    }

    Text {
        id: productName
        anchors.top: deviceName.bottom
        anchors.topMargin: VCMargin.tiny
        anchors.left: deviceName.left
        font.pixelSize: VCFont.label
        color: VCColor.white
        text: root.device ? root.device[ "productName" ] : ""
    }

    VCSwitch {
        id: powerSwitch
        anchors.top: parent.top
        anchors.topMargin: VCMargin.small
        anchors.right: parent.right
        anchors.rightMargin: anchors.topMargin
        enabled: root.device

        property bool on: enabled && root.device[ "isOn" ]
        onOnChanged: checked = on

        onClicked: root.device.commandPower( checked )
    }

    GridLayout {
        id: controlsLayout
        anchors.top: productName.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: VCMargin.small
        columns: 3
        columnSpacing: VCMargin.small
        rowSpacing: VCMargin.tiny

        Text {
            id: brightnessLabel
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: VCFont.body
            color: VCColor.white
            text: qsTr( "Brightness" )
            visible: brightnessSlider.visible
        }

        VCSlider {
            id: brightnessSlider
            Layout.fillWidth: true
            Layout.preferredHeight: height
            Layout.alignment: Qt.AlignVCenter
            visible: root.device && ( undefined !== root.device[ "brightness" ] )

            property int currentValue: visible ? root.device[ "brightness" ] : 0
            onCurrentValueChanged: {
                if ( !pressed )
                {
                    value = currentValue
                }
            }

            onPressedChanged: {
                if ( !pressed )
                {
                    // Ensure the final chosen value is commanded.
                    root.device.commandBrightness( Math.round( value ) )
                }
            }

            onMoved: {
                // Throttle requests to ensure the Bridge is not overloaded.
                if ( !brightnessSliderHoldOffTimer.running )
                {
                    root.device.commandBrightness( Math.round( value ) )
                    brightnessSliderHoldOffTimer.start()
                }
            }

            Timer {
                id: brightnessSliderHoldOffTimer
                running: false
                repeat: false
                interval: 50
            }
        }

        Text {
            id: brightnessSliderValue
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: VCFont.label
            font.bold: true
            color: VCColor.white
            text: Math.round( brightnessSlider.value ) + "%"
            visible: brightnessSlider.visible
        }

        Text {
            id: colorTemperatureLabel
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: VCFont.body
            color: VCColor.white
            text: qsTr( "Temperature" )
            visible: colorTemperatureSlider.visible
        }

        VCSlider {
            id: colorTemperatureSlider
            Layout.fillWidth: true
            Layout.preferredHeight: height
            Layout.alignment: Qt.AlignVCenter
            visible: root.device && ( undefined !== root.device[ "colorTemperature" ] )
            from: visible ? root.device[ "minColorTemperature" ] : 0
            to: visible ? root.device[ "maxColorTemperature" ] : 0
            stepSize: 1

            property int currentValue: visible ? root.device[ "colorTemperature" ] : 0
            onCurrentValueChanged: {
                if ( !pressed )
                {
                    value = currentValue
                }
            }

            onPressedChanged: {
                if ( !pressed )
                {
                    // Ensure the final chosen value is commanded.
                    root.device.commandColorTemperature( value )
                }
            }

            onMoved: {
                // Throttle requests to ensure the Bridge is not overloaded.
                if ( !colorTemperatureSliderHoldOffTimer.running )
                {
                    root.device.commandColorTemperature( value )
                    colorTemperatureSliderHoldOffTimer.start()
                }
            }

            Timer {
                id: colorTemperatureSliderHoldOffTimer
                running: false
                repeat: false
                interval: 50
            }
        }

        Text {
            id: colorTemperatureSliderValue
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: VCFont.label
            font.bold: true
            color: VCColor.white
            text: colorTemperatureSlider.value + "K"
            visible: colorTemperatureSlider.visible
        }

        Text {
            id: colorLabel
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: VCFont.body
            color: VCColor.white
            text: qsTr( "Color Hue" )
            visible: colorSlider.visible
        }

        VCSlider {
            id: colorSlider
            Layout.fillWidth: true
            Layout.preferredHeight: height
            Layout.alignment: Qt.AlignVCenter
            visible: root.device && ( undefined !== root.device[ "color" ] )
            from: 0
            to: 359
            stepSize: 1
            backgroundImage: "qrc:/images/color-scale.png"

            property int currentValue: visible ? root.device[ "hue" ] : 0
            onCurrentValueChanged: {
                if ( !pressed )
                {
                    value = currentValue
                }
            }

            onPressedChanged: {
                if ( !pressed )
                {
                    // Ensure the final chosen value is commanded.
                    root.device.commandColor( value )
                }
            }

            onMoved: {
                // Throttle requests to ensure the Bridge is not overloaded.
                if ( !colorSliderHoldOffTimer.running )
                {
                    root.device.commandColor( value )
                    colorSliderHoldOffTimer.start()
                }
            }

            Timer {
                id: colorSliderHoldOffTimer
                running: false
                repeat: false
                interval: 50
            }
        }

        Text {
            id: colorSliderValue
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: VCFont.label
            font.bold: true
            color: VCColor.white
            text: colorSlider.value
            visible: colorSlider.visible
        }
    }
}
