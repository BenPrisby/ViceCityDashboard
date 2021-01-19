import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Item {
    id: root

    GridView {
        id: contentLayout
        anchors.fill: parent
        anchors.rightMargin: interactive ? VCMargin.medium : 0  // Leave room for the scrollbar
        cellWidth: width / 4
        cellHeight: height / 2
        cacheBuffer: Math.max( 0, contentHeight )
        interactive: contentHeight > height
        ScrollBar.vertical: ScrollBar {
            parent: root
            anchors.top: contentLayout.top
            anchors.bottom: contentLayout.bottom
            anchors.left: contentLayout.right
            anchors.leftMargin: root.width - contentLayout.width - width  // Appear to be anchored to root.right
            policy: contentLayout.interactive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }
        model: VCHub.scenes
        clip: true

        delegate: Item {
            width: contentLayout.cellWidth
            height: contentLayout.cellHeight

            Tile {
                anchors.fill: parent
                anchors.margins: VCMargin.tiny
                introAnimationDelay: ( index * 50 ) + ( ( index % 2 ) * 100 )  // Scatter

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: VCMargin.small
                    spacing: VCMargin.medium

                    Image {
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: width
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: VCMargin.tiny
                        sourceSize: Qt.size( width, height )
                        source: "qrc:/images/" + modelData[ "icon" ] + ".svg"
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: VCFont.subHeader
                        elide: Text.ElideRight
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        color: VCColor.white
                        text: modelData[ "name" ]
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 16
                        color: VCColor.gray

                        ListView {
                            id: colorsView
                            anchors.fill: parent
                            orientation: ListView.Horizontal
                            model: VCHub.parseSceneColors( modelData[ "name" ] )
                            delegate: Rectangle {
                                width: colorsView.width / colorsView.count
                                height: colorsView.height
                                color: modelData
                            }
                        }
                    }

                    VCButton {
                        id: playButton
                        Layout.preferredWidth: 80
                        Layout.preferredHeight: 40
                        Layout.alignment: Qt.AlignHCenter
                        iconSource: "qrc:/images/play.svg"
                        enabled: !VCHub.isRunningScene

                        onClicked: VCHub.runScene( modelData[ "name" ] )
                    }
                }
            }
        }
    }
}
