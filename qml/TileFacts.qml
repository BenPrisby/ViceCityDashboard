import QtQuick 2.12
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    Flickable {
        anchors.fill: parent
        anchors.margins: VCMargin.small
        contentWidth: width
        contentHeight: Math.max( height, fact.implicitHeight )
        interactive: contentHeight > height
        clip: true

        Text {
            id: fact
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: VCColor.white
            font.pixelSize: VCFont.paragraph
            wrapMode: Text.WordWrap
            text: VCHub.facts.fact
        }
    }
}
