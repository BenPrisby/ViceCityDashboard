import QtQuick 2.12
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

        Image {
            id: icon
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 40
            Layout.preferredHeight: width
            fillMode: Image.PreserveAspectFit
            sourceSize: Qt.size( Layout.preferredWidth, Layout.preferredHeight )
            source: "qrc:/images/fire.svg"
        }

        Text {
            id: insult
            Layout.fillWidth: true
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            color: VCColor.white
            font.pixelSize: VCFont.paragraph
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
            text: VCHub.insults.insult
        }
    }
}
