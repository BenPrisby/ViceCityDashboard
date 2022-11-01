import QtQuick 2.15
import VCStyles 1.0
import com.benprisby.vc.vchub 1.0

Rectangle {
    id: root

    color: ("" == albumArt.source) ? VCColor.grayLight : VCColor.transparent

    Image {
        id: placeholderImage

        anchors.centerIn: parent
        width: parent.width / 3
        height: width
        opacity: 0.7
        sourceSize: Qt.size(width, height)
        source: "qrc:/images/music.svg"
        visible: "" == albumArt.source
    }

    Image {
        id: albumArt

        anchors.fill: parent
        sourceSize: Qt.size(width, height)
        source: VCHub.spotify.isActive ? VCHub.spotify.trackAlbumArt : ""
    }

}
