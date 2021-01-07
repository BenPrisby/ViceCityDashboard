import QtQuick 2.12
import QtQuick.Controls 2.12
import VCStyles 1.0

Switch {
    id: control
    width: 120
    height: 50
    font.pixelSize: VCFont.label
    font.bold: true
    font.capitalization: Font.AllUppercase
    spacing: VCMargin.small
    text: qsTr( checked ? "ON" : "OFF" )

    indicator: Rectangle {
        implicitWidth: 60
        implicitHeight: 30
        x: control.leftPadding
        y: ( parent.height / 2 ) - ( height / 2 )
        radius: implicitHeight / 2
        color: control.checked ? VCColor.green : VCColor.grayLighter

        Rectangle {
            id: toggledIndicator
            x: control.checked ? parent.width - width : 0
            width: height
            height: parent.height
            radius: width / 2
            color: control.down ? VCColor.grayLighter : VCColor.white

            Behavior on x {
                PropertyAnimation {
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        verticalAlignment: Text.AlignVCenter
        opacity: enabled ? 1.0 : 0.3
        color: control.down ? VCColor.grayLighter : VCColor.white
        leftPadding: control.indicator.width + control.spacing
    }
}
