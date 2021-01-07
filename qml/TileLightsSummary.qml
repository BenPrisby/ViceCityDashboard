import QtQuick 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    GridLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: VCMargin.small
        columnSpacing: VCMargin.small
        columns: 2

        Image {
            id: huePowerIcon
            Layout.preferredWidth: 30
            Layout.preferredHeight: width
            fillMode: Image.PreserveAspectFit
            sourceSize: Qt.size( Layout.preferredWidth, Layout.preferredHeight )
            source: ( 0 < VCHub.hue.onDevicesCount ) ? "qrc:/images/light-on.svg" : "qrc:/images/light-off.svg"
        }

        Text {
            id: huePowerValue
            Layout.fillWidth: true
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            color: VCColor.white
            font.pixelSize: VCFont.body
            text: qsTr( VCHub.hue.onDevicesCount + " Hue lights on" )
        }

        Image {
            id: nanoleafPowerIcon
            Layout.preferredWidth: 30
            Layout.preferredHeight: width
            fillMode: Image.PreserveAspectFit
            sourceSize: Qt.size( Layout.preferredWidth, Layout.preferredHeight )
            source: VCHub.nanoleaf.isOn ? "qrc:/images/light-on.svg" : "qrc:/images/light-off.svg"
        }

        Text {
            id: nanoleafPowerValue
            Layout.fillWidth: true
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            color: VCColor.white
            font.pixelSize: VCFont.body
            text: qsTr( "Nanoleaf " + ( VCHub.nanoleaf.isOn ? "on" : "off" ) )
        }
    }
}
