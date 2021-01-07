import QtQuick 2.12
import VCStyles 1.0

Rectangle {
    id: root
    radius: 6
    opacity: 0
    scale: 0.9
    clip: true
    color: VCColor.grayDark

    property bool introAnimationEnabled: true
    property int introAnimationDuration: 500
    property int introAnimationDelay: 0

    onVisibleChanged: {
        if ( !visible )
        {
            opacity = 0
            scale = 0.9
        }
    }

    SequentialAnimation {
        id: introAnimation
        running: root.introAnimationEnabled && root.visible

        PauseAnimation {
            duration: root.introAnimationDelay
        }

        ParallelAnimation {
            OpacityAnimator {
                target: root
                from: root.opacity
                to: 1
                duration: root.introAnimationDuration
                easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                target: root
                property: "scale"
                from: root.scale
                to: 1
                duration: root.introAnimationDuration
                easing.type: Easing.InOutQuad
            }
        }
    }
}
