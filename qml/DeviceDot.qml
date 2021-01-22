import QtQuick 2.15
import QtQuick.Controls 2.15
import VCStyles 1.0

Rectangle {
    id: root
    width: 30
    height: width
    radius: width / 2
    border.width: selected ? 6 : 3
    border.color: VCColor.grayLighter

    property bool selected: false

    signal clicked()

    SequentialAnimation {
        id: selectionAnimation
        running: root.selected
        loops: Animation.Infinite

        PauseAnimation {
            duration: 2000
        }

        OpacityAnimator {
            target: root
            from: 1
            to: 0.5
            duration: 1000
            easing.type: Easing.Linear
        }

        OpacityAnimator {
            target: root
            from: 0.5
            to: 1
            duration: 1000
            easing.type: Easing.Linear
        }

        onStopped: root.opacity = 1
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: root.clicked()
    }

    Rectangle {
        id: pressedIndicator
        anchors.fill: parent
        color: VCColor.white
        opacity: 0.3
        radius: parent.radius
        visible: mouseArea.pressed
    }

    Behavior on color {
        ColorAnimation {
            duration: 250
            easing.type: Easing.Linear
        }
    }
}
