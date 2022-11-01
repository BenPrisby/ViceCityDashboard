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

        SceneShortcutButton {
            id: daytimeSceneButton

            Layout.preferredWidth: (parent.width / 2) - (parent.columnSpacing / 2)
            Layout.fillHeight: true
            index: 0
        }

        SceneShortcutButton {
            id: viceCitySceneButton

            Layout.preferredWidth: (parent.width / 2) - (parent.columnSpacing / 2)
            Layout.fillHeight: true
            index: 1
        }

        SceneShortcutButton {
            id: nighttimeSceneButton

            Layout.preferredWidth: (parent.width / 2) - (parent.columnSpacing / 2)
            Layout.fillHeight: true
            index: 2
        }

        SceneShortcutButton {
            id: movieSceneButton

            Layout.preferredWidth: (parent.width / 2) - (parent.columnSpacing / 2)
            Layout.fillHeight: true
            index: 3
        }

    }

}
