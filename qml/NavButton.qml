import QtQuick 2.15
import QtQuick.Controls 2.15
import VCStyles 1.0

Button {
    id: root

    property alias iconSource: icon.source
    property alias hasNotification: notificationIndicator.visible

    width: 80
    height: width
    checkable: true
    onDownChanged: {
        if (down) {
            pressedIndicator.opacity = 0.2;
        } else {
            outAnimation.start();
        }
    }

    Rectangle {
        id: pressedIndicator

        anchors.fill: parent
        color: VCColor.white
        opacity: 0

        OpacityAnimator on opacity {
            id: outAnimation

            from: pressedIndicator.opacity
            to: 0
            duration: 200
            easing.type: Easing.InOutQuad
            running: false
        }

    }

    Image {
        id: icon

        anchors.centerIn: parent
        width: parent.width / 2
        height: width
        fillMode: Image.PreserveAspectFit
        opacity: parent.checked ? 1 : 0.3
        sourceSize: Qt.size(width, height)
    }

    Rectangle {
        id: notificationIndicator

        anchors.top: parent.top
        anchors.topMargin: 6
        anchors.right: parent.right
        anchors.rightMargin: anchors.topMargin
        width: 10
        height: width
        radius: width / 2
        color: VCColor.red
        visible: false
    }

    background: Rectangle {
        color: VCColor.transparent
    }

}
