import QtQuick 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0
import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    RowLayout {
        id: contentLayout

        anchors.fill: parent
        anchors.margins: VCMargin.small
        spacing: VCMargin.medium
        visible: VCHub.spotify.isActive

        TrackAlbumArt {
            id: albumArt

            Layout.fillHeight: true
            Layout.maximumHeight: parent.height
            Layout.preferredWidth: height
            Layout.alignment: Qt.AlignVCenter
        }

        ColumnLayout {
            id: trackInfoLayout

            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: VCMargin.tiny

            Text {
                id: trackName

                Layout.fillWidth: true
                font.pixelSize: VCFont.body
                color: VCColor.white
                wrapMode: Text.WordWrap
                text: VCHub.spotify.isActive ? VCHub.spotify.trackName : ""
            }

            Text {
                id: trackArtist

                Layout.fillWidth: true
                font.pixelSize: VCFont.label
                color: VCColor.white
                wrapMode: Text.WordWrap
                text: VCHub.spotify.isActive ? VCHub.spotify.trackArtist : ""
            }

            Text {
                id: trackAlbum

                Layout.fillWidth: true
                font.pixelSize: VCFont.label
                color: VCColor.white
                wrapMode: Text.WordWrap
                text: VCHub.spotify.isActive ? VCHub.spotify.trackAlbum : ""
                visible: (trackName.implicitHeight < (2 * trackName.font.pixelSize))
                         && (implicitHeight < (3 * font.pixelSize))  // Prevent spilling off the tile for longer text
            }

        }

    }

    Text {
        id: idleIndicator

        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: VCFont.body
        color: VCColor.grayLightest
        text: qsTr("Not Playing Music")
        visible: !VCHub.spotify.isActive
    }

}
