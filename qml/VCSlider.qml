import QtQuick 2.12
import QtQuick.Controls 2.12
import VCStyles 1.0

Slider {
    id: control
    from: 0
    to: 100

    property alias backgroundImage: backgroundImage.source

    background: Rectangle {
        id: backgroundBase
        x: control.leftPadding
        y: control.topPadding + ( control.availableHeight / 2 ) - ( height / 2 )
        implicitWidth: 200
        implicitHeight: 6
        width: control.availableWidth
        height: implicitHeight
        radius: 2
        color: control.enabled ? VCColor.grayLighter : VCColor.grayLight
        clip: true

        Rectangle {
            id: progressIndicator
            width: control.visualPosition * parent.width
            height: parent.height
            color: control.enabled ? VCColor.green : VCColor.grayLightest
            radius: 2
        }

        Image {
            id: backgroundImage
            anchors.fill: parent
            fillMode: Image.Stretch
            visible: source
        }
    }

    handle: Rectangle {
        x: control.leftPadding + ( control.visualPosition * ( control.availableWidth - width ) )
        y: control.topPadding + ( control.availableHeight / 2 ) - ( height / 2 )
        implicitWidth: 26
        implicitHeight: implicitWidth
        radius: implicitWidth / 2
        color: control.enabled ? ( control.pressed ? VCColor.grayLighter : VCColor.white ) : VCColor.grayLighter
    }
}
