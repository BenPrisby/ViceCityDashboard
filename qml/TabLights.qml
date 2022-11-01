import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0
import com.benprisby.vc.vchub 1.0

ColumnLayout {
    id: root

    spacing: VCMargin.medium
    onVisibleChanged: {
        if (!visible) {
            // Reset the tab bar index when switching away from this tab.
            tabBar.currentIndex = 0;
        }
    }

    TabBar {
        id: tabBar

        Layout.fillWidth: true
        Layout.preferredHeight: height
        spacing: VCMargin.tiny

        TabButton {
            id: lightsTabButton

            text: qsTr("Lights")
            font.pixelSize: VCFont.paragraph

            background: Rectangle {
                color: lightsTabButton.checked ? VCColor.white : VCColor.grayDarker
                radius: 6
            }

        }

        TabButton {
            id: scenesTabButton

            text: qsTr("Scenes")
            font.pixelSize: VCFont.paragraph

            background: Rectangle {
                color: scenesTabButton.checked ? VCColor.white : VCColor.grayDarker
                radius: 6
            }

        }

        background: Rectangle {
            color: VCColor.black
        }

    }

    StackLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        currentIndex: tabBar.currentIndex

        TabLightsMap {
        }

        TabScenes {
        }

    }

}
