import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QtQuick.VirtualKeyboard 2.15
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Window {
    id: mainWindow
    objectName: "mainWindow"
    minimumWidth: 1024
    minimumHeight: 600
    visible: true
    title: Qt.application.name + " " + Qt.application.version

    Rectangle {
        id: background
        anchors.fill: parent
        color: VCColor.black

        Rectangle {
            id: navBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: 80
            color: VCColor.grayDarker

            ColumnLayout {
                id: navBarLayout
                anchors.fill: parent

                NavButton {
                    id: homeButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: width
                    iconSource: "qrc:/images/home.svg"
                    checked: true

                    onClicked: mainContent.currentIndex = 0
                }

                NavButton {
                    id: lightsButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: width
                    iconSource: "qrc:/images/light.svg"

                    onClicked: mainContent.currentIndex = 1
                }

                NavButton {
                    id: musicButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: width
                    iconSource: "qrc:/images/music.svg"

                    onClicked: mainContent.currentIndex = 2
                }

                NavButton {
                    id: networkButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: width
                    iconSource: "qrc:/images/network.svg"

                    onClicked: mainContent.currentIndex = 3
                }

                Item {
                    Layout.fillHeight: true
                }

                NavButton {
                    id: systemButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: width
                    iconSource: "qrc:/images/system.svg"

                    onClicked: mainContent.currentIndex = 4
                }
            }

            ButtonGroup {
                id: navButtonGroup
                buttons: navBarLayout.children
            }
        }

        StackLayout {
            id: mainContent
            anchors.top: parent.top
            anchors.left: navBar.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: VCMargin.medium

            TabHome {}

            TabLights {}

            TabMusic {}

            TabNetwork {}

            TabSystem {}
        }

        // Animate background color changes.
        Behavior on color {
            ColorAnimation {
                duration: 1000
                easing.type: Easing.Linear
            }
        }

        // Virtual Keyboard
        InputPanel {
            id: inputPanel
            z: 99
            x: 0
            y: background.height
            width: background.width

            // Force focus away when the keyboard dismisses.
            onStateChanged: {
                if (  "" == state )
                {
                    navBar.forceActiveFocus()
                }

            }

            states: State {
                name: "visible"
                when: inputPanel.active
                PropertyChanges {
                    target: inputPanel
                    y: background.height - inputPanel.height
                }
            }

            transitions: Transition {
                from: ""
                to: "visible"
                reversible: true
                ParallelAnimation {
                    NumberAnimation {
                        properties: "y"
                        duration: 250
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }

        VCScreensaver {
            id: screensaver
            anchors.fill: parent
        }
    }

    Shortcut {
        id: screenshotShortcut
        sequence: StandardKey.Save

        onActivated: {
            var filename = VCHub.screenshotPath()
            console.log( "Screenshot: " + filename )
            background.grabToImage( function( result ) { result.saveToFile( filename ) } )
        }
    }
}
