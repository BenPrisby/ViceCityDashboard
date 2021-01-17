import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    Flickable {
        id: flickableWrapper
        anchors.fill: parent
        anchors.margins: VCMargin.small
        contentWidth: width
        contentHeight: Math.max( height, fact.implicitHeight )
        interactive: contentHeight > height
        ScrollBar.vertical: ScrollBar { policy: flickableWrapper.interactive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff }
        clip: true

        Text {
            id: fact
            anchors.fill: parent
            rightPadding: flickableWrapper.interactive ? VCMargin.medium : 0  // Leave room for the scrollbar
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: VCColor.white
            font.pixelSize: VCFont.paragraph
            wrapMode: Text.WordWrap
            text: VCHub.facts.fact
        }
    }
}
