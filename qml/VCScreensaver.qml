import QtQuick 2.15
import com.benprisby.vc.vchub 1.0

Item {
    id: root

    Timer {
        id: screensaverTimer

        running: VCHub.screensaverEnabled
        repeat: false
        interval: 2 * 60 * 1000  // 2 minutes
        onTriggered: screensaver.visible = true
    }

    MouseArea {
        id: screensaverResetter

        anchors.fill: parent
        propagateComposedEvents: true
        // Pass all events though.
        onClicked: {
            screensaverTimer.restart();
            mouse.accepted = false;
        }
        onPressed: {
            screensaverTimer.restart();
            mouse.accepted = false;
        }
        onReleased: {
            screensaverTimer.restart();
            mouse.accepted = false;
        }
        onDoubleClicked: {
            screensaverTimer.restart();
            mouse.accepted = false;
        }
        onPositionChanged: {
            screensaverTimer.restart();
            mouse.accepted = false;
        }
        onPressAndHold: {
            screensaverTimer.restart();
            mouse.accepted = false;
        }
    }

    Rectangle {
        id: screensaver

        anchors.fill: parent
        color: "black"
        visible: false
        // Drive the active state based on the screensaver being visible.
        onVisibleChanged: VCHub.isActive = !visible

        // Consume the mouse event to not affect whatever is underneath the screensaver.
        MouseArea {
            id: screensaverDismisser

            anchors.fill: parent
            onClicked: {
                screensaver.visible = false;
                screensaverTimer.restart();
            }
        }

        Image {
            id: screensaverImage

            x: 0
            y: 0
            width: 400
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/vice-city.png"

            SequentialAnimation on x {
                id: xAnimation

                property int from: 0
                property int to: screensaver.width - screensaverImage.width
                property int duration: 15 * 1000

                loops: Animation.Infinite
                running: screensaver.visible

                PropertyAnimation {
                    from: xAnimation.from
                    to: xAnimation.to
                    duration: xAnimation.duration
                    easing.type: Easing.Linear
                }

                PropertyAnimation {
                    from: xAnimation.to
                    to: xAnimation.from
                    duration: xAnimation.duration
                    easing.type: Easing.Linear
                }

            }

            SequentialAnimation on y {
                id: yAnimation

                property int from: 0
                property int to: screensaver.height - screensaverImage.height
                property int duration: 9 * 1000

                loops: Animation.Infinite
                running: screensaver.visible

                PropertyAnimation {
                    from: yAnimation.from
                    to: yAnimation.to
                    duration: yAnimation.duration
                    easing.type: Easing.Linear
                }

                PropertyAnimation {
                    from: yAnimation.to
                    to: yAnimation.from
                    duration: yAnimation.duration
                    easing.type: Easing.Linear
                }

            }

        }

    }

}
