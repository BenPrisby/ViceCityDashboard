import QtQuick 2.12
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    Text {
        id: fact
        anchors.fill: parent
        anchors.margins: VCMargin.small
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: VCColor.white
        font.pixelSize: VCFont.paragraph
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
        text: VCHub.facts.fact
    }
}
