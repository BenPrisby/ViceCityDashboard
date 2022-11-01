import QtQuick 2.15
import QtQuick.Controls 2.15
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
        text: qsTr("Nanoleaf")
    }

    Text {
        id: deviceName

        anchors.top: productName.bottom
        anchors.topMargin: VCMargin.tiny
        anchors.left: productName.left
        font.pixelSize: VCFont.label
        color: VCColor.white
        text: VCHub.nanoleaf.selectedEffect ? VCHub.nanoleaf.selectedEffect : qsTr("No Effect Selected")
    }

    VCSwitch {
        id: powerSwitch

        property bool on: VCHub.nanoleaf.isOn

        anchors.top: parent.top
        anchors.topMargin: VCMargin.small
        anchors.right: parent.right
        anchors.rightMargin: anchors.topMargin
        checked: false
        onOnChanged: checked = on
        onClicked: VCHub.nanoleaf.commandPower(checked)
    }

    ListView {
        id: effectsView

        anchors.top: deviceName.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: VCMargin.small
        orientation: ListView.Horizontal
        spacing: VCMargin.tiny
        cacheBuffer: Math.max(0, contentWidth)
        interactive: contentWidth > width
        snapMode: ListView.SnapToItem
        clip: true
        model: VCHub.nanoleaf.effects

        ScrollBar.horizontal: ScrollBar {
            policy: effectsView.interactive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }

        delegate: Rectangle {
            width: (effectsView.width / 4) - effectsView.spacing
            height: effectsView.height - (effectsView.interactive ? VCMargin.medium : 0)  // Leave room for the scrollbar
            color: (modelData["name"] === VCHub.nanoleaf.selectedEffect) ? VCColor.green : VCColor.grayLight
            radius: 4

            Text {
                id: effectName

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: VCMargin.tiny
                anchors.rightMargin: anchors.leftMargin
                anchors.right: parent.right
                anchors.bottom: colorPalette.top
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                font.pixelSize: VCFont.paragraph
                font.bold: modelData["name"] === VCHub.nanoleaf.selectedEffect
                color: VCColor.white
                text: modelData["name"]
            }

            Rectangle {
                id: colorPalette

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: VCMargin.tiny
                height: 12
                color: VCColor.gray

                ListView {
                    id: colorsView

                    anchors.fill: parent
                    orientation: ListView.Horizontal
                    interactive: false
                    model: modelData["colors"]

                    delegate: Rectangle {
                        width: colorsView.width / colorsView.count
                        height: colorsView.height
                        color: modelData
                    }

                }

            }

            Rectangle {
                id: pressedIndicator

                anchors.fill: parent
                color: VCColor.white
                opacity: 0.3
                radius: parent.radius
                visible: effectsDelegateMouseArea.pressed
            }

            MouseArea {
                id: effectsDelegateMouseArea

                anchors.fill: parent
                onClicked: VCHub.nanoleaf.selectEffect(modelData["name"])
            }

        }

    }

}
