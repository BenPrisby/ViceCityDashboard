import QtQuick 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    GridLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: VCMargin.small
        columns: 2
        columnSpacing: VCMargin.medium
        rowSpacing: VCMargin.medium

        VCButton {
            id: daytimeSceneButton
            Layout.preferredWidth: ( parent.width / 2 ) - ( parent.columnSpacing / 2 )
            Layout.fillHeight: true
            text: qsTr( "Daytime" )
            iconSource: "qrc:/images/sun.svg"
            enabled: !VCHub.isRunningScene

            onClicked: VCHub.runScene( "Daytime" )
        }

        VCButton {
            id: viceCitySceneButton
            Layout.preferredWidth: ( parent.width / 2 ) - ( parent.columnSpacing / 2 )
            Layout.fillHeight: true
            text: qsTr( "Vice City" )
            iconSource: "qrc:/images/beer.svg"
            enabled: !VCHub.isRunningScene

            onClicked: VCHub.runScene( "Vice City" )
        }

        VCButton {
            id: nighttimeSceneButton
            Layout.preferredWidth: ( parent.width / 2 ) - ( parent.columnSpacing / 2 )
            Layout.fillHeight: true
            text: qsTr( "Nighttime" )
            iconSource: "qrc:/images/moon.svg"
            enabled: !VCHub.isRunningScene

            onClicked: VCHub.runScene( "Nighttime" )
        }

        VCButton {
            id: movieSceneButton
            Layout.preferredWidth: ( parent.width / 2 ) - ( parent.columnSpacing / 2 )
            Layout.fillHeight: true
            text: qsTr( "Movie" )
            iconSource: "qrc:/images/movie.svg"
            enabled: !VCHub.isRunningScene

            onClicked: VCHub.runScene( "Movie" )
        }
    }
}
