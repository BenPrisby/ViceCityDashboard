import QtQuick 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    Image {
        id: networkIcon
        anchors.top: parent.top
        anchors.topMargin: VCMargin.small
        anchors.horizontalCenter: parent.horizontalCenter
        height: 30
        width: height
        fillMode: Image.PreserveAspectFit
        sourceSize: Qt.size( width, height )
        source: "qrc:/images/network.svg"
    }

    Rectangle {
        id: totalQueriesBar
        anchors.top: networkIcon.bottom
        anchors.topMargin: VCMargin.medium
        anchors.left: parent.left
        anchors.leftMargin: VCMargin.small
        anchors.bottom: parent.bottom
        anchors.bottomMargin: VCMargin.small
        width: 20
        radius: 2
        color: VCColor.green
        clip: true

        Rectangle {
            id: blockedQueriesBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: parent.height * ( VCHub.piHole.percentBlocked / 100.0 )
            radius: parent.radius
            color: VCColor.red
        }
    }

    ColumnLayout {
        anchors.top: totalQueriesBar.top
        anchors.left: totalQueriesBar.right
        anchors.leftMargin: VCMargin.medium
        anchors.right: parent.right
        anchors.rightMargin: VCMargin.small
        anchors.bottom: totalQueriesBar.bottom
        spacing: VCMargin.tiny

        Text {
            id: queriesLabel
            color: VCColor.grayLightest
            font.pixelSize: VCFont.label
            font.capitalization: Font.AllUppercase
            text: qsTr( "DNS QUERIES" )
        }

        Text {
            id: sentQueries
            color: VCColor.green
            font.pixelSize: VCFont.body
            text: qsTr( VCHub.formatInt( VCHub.piHole.totalQueries - VCHub.piHole.blockedQueries ) + " sent" )
        }

        Text {
            id: blockedQueries
            color: VCColor.red
            font.pixelSize: VCFont.body
            text: qsTr( VCHub.formatInt( VCHub.piHole.blockedQueries ) + " blocked" )
        }

        Rectangle {
            id: seperator
            Layout.preferredHeight: 1
            Layout.fillWidth: true
            color: VCColor.grayLight
        }

        Text {
            id: blocklistLabel
            color: VCColor.grayLightest
            font.pixelSize: VCFont.label
            font.capitalization: Font.AllUppercase
            text: qsTr( "BLOCKLIST" )
        }

        Text {
            id: blockedDomains
            color: VCColor.white
            font.pixelSize: VCFont.body
            text: qsTr( VCHub.formatInt( VCHub.piHole.blockedDomains ) + " domains" )
        }
    }
}
