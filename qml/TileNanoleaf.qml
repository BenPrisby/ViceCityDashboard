import QtQuick 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    Text {
        id: productName
        anchors.top: parent.top
        anchors.topMargin: VCMargin.small
        anchors.left: parent.left
        anchors.leftMargin: anchors.topMargin
        font.pixelSize: VCFont.subHeader
        color: VCColor.white
        text: qsTr( "Nanoleaf" )
    }

    Text {
        id: deviceName
        anchors.top: productName.bottom
        anchors.topMargin: VCMargin.tiny
        anchors.left: productName.left
        font.pixelSize: VCFont.label
        color: VCColor.white
        text: VCHub.nanoleaf.name
    }

    VCSwitch {
        id: powerSwitch
        anchors.top: parent.top
        anchors.topMargin: VCMargin.small
        anchors.right: parent.right
        anchors.rightMargin: anchors.topMargin
        checked: false

        property bool on: VCHub.nanoleaf.isOn
        onOnChanged: checked = on

        onClicked: VCHub.nanoleaf.commandPower( checked )
    }

    Rectangle {
        anchors.top: deviceName.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: VCMargin.small
        color: VCColor.transparent
        border.width: 2
        border.color: VCColor.grayLighter
        clip: true

        GridView {
            id: effectsDisplay
            anchors.fill: parent
            cellWidth: width / 3
            cellHeight: height / 3
            interactive: 9 < count
            model: VCHub.nanoleaf.effects
            delegate: Rectangle {
                id: effectsDelegate
                width: effectsDisplay.cellWidth
                height: effectsDisplay.cellHeight
                color: ( VCHub.nanoleaf.selectedEffect === modelData ) ? VCColor.green : VCColor.transparent
                border.width: 1
                border.color: VCColor.grayLighter

                Text {
                    id: effectName
                    width: parent.width
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: VCFont.label
                    font.bold: ( VCHub.nanoleaf.selectedEffect === modelData )
                    color: VCColor.white
                    text: modelData
                }

                Rectangle {
                    id: pressedIndicator
                    anchors.fill: parent
                    color: VCColor.white
                    opacity: 0.3
                    visible: effectsDelegateMouseArea.pressed
                }

                MouseArea {
                    id: effectsDelegateMouseArea
                    anchors.fill: parent

                    onClicked: VCHub.nanoleaf.selectEffect( modelData )
                }
            }
        }
    }
}
