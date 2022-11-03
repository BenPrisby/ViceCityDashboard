import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0
import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    onVisibleChanged: {
        if (!visible) {
            searchField.text = "";
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: VCMargin.small
        spacing: VCMargin.medium

        RowLayout {
            Layout.fillWidth: true
            Layout.maximumHeight: 60
            spacing: VCMargin.medium

            TextField {
                id: searchField

                Layout.fillWidth: true
                Layout.fillHeight: true
                font.pixelSize: VCFont.body
                placeholderText: qsTr("Queue a track...")
                selectByMouse: false
                onTextChanged: VCHub.spotify.search(text)

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: VCMargin.small
                    width: 30
                    height: width
                    fillMode: Image.PreserveAspectFit
                    sourceSize: Qt.size(width, height)
                    source: "qrc:/images/clear.svg"
                    visible: searchField.text && searchField.activeFocus

                    MouseArea {
                        anchors.fill: parent
                        onClicked: searchField.text = ""
                    }

                }

            }

            VCButton {
                Layout.preferredWidth: 120
                Layout.fillHeight: true
                text: qsTr("Cancel")
                outline: true
                visible: searchField.activeFocus
                onVisibleChanged: {
                    if (!visible) {
                        down = false;
                    }
                }
            }

        }

        ListView {
            id: searchResultsList

            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: VCMargin.small
            cacheBuffer: Math.max(0, contentHeight)
            interactive: contentHeight > height
            clip: true
            model: VCHub.spotify.searchResults
            onMovementStarted: forceActiveFocus()

            ScrollBar.vertical: ScrollBar {
                policy: searchResultsList.interactive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
            }

            delegate: RowLayout {
                width: parent.width
                height: 70
                spacing: VCMargin.medium

                Image {
                    id: trackImage

                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    fillMode: Image.PreserveAspectFit
                    source: modelData["image"]
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: VCMargin.tiny

                    Text {
                        id: trackName

                        Layout.fillWidth: true
                        font.pixelSize: VCFont.body
                        color: VCColor.white
                        text: modelData["name"]
                    }

                    Text {
                        id: trackArtistAndAlbum

                        Layout.fillWidth: true
                        font.pixelSize: VCFont.label
                        color: VCColor.white
                        elide: Text.ElideRight
                        text: modelData["artist"] + " — " + modelData["album"]
                    }

                }

                VCButton {
                    id: queueButton

                    property bool queued: false

                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 50
                    Layout.alignment: Qt.AlignVCenter
                    Layout.rightMargin: searchResultsList.interactive ? VCMargin.medium : 0  // Leave room for the scrollbar
                    text: qsTr("Queue")
                    enabled: VCHub.spotify.isPlayerActive && !queued
                    onClicked: {
                        VCHub.spotify.queue(modelData["uri"]);
                        queued = true;
                        text = qsTr("Queued");
                    }
                }

            }

        }

    }

}
