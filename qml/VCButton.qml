import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

Button {
    id: control

    property alias iconSource: buttonIcon.source
    property bool outline: false

    width: 120
    height: 60
    font.pixelSize: VCFont.body

    contentItem: RowLayout {
        spacing: buttonIcon.visible ? VCMargin.medium : 0

        Image {
            id: buttonIcon

            Layout.preferredHeight: control.height / 2
            Layout.preferredWidth: height
            Layout.leftMargin: control.text ? VCMargin.small : 0
            Layout.alignment: control.text ? Qt.AlignLeft : Qt.AlignHCenter
            fillMode: Image.PreserveAspectFit
            sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
            visible: "" != source
        }

        Text {
            id: buttonText

            Layout.fillWidth: true
            Layout.fillHeight: true
            horizontalAlignment: buttonIcon.visible ? Text.AlignLeft : Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: control.text
            font: control.font
            color: control.outline ? VCColor.blue : VCColor.white
            elide: Text.ElideRight
            visible: text
        }

    }

    background: Rectangle {
        color: {
            if (control.enabled) {
                if (control.checkable) {
                    if (control.checked) {
                        return VCColor.green;
                    }
                    return VCColor.transparent;
                }
                if (control.outline) {
                    return VCColor.transparent;
                }
                return VCColor.blue;
            }
            return VCColor.grayLighter;
        }
        radius: 6
        border.width: ((control.checkable && (!control.checked)) || control.outline) ? 2 : 0
        border.color: {
            if (control.enabled) {
                if (control.checkable && (!control.checked)) {
                    return VCColor.white;
                }
                return VCColor.blue;
            }
            return VCColor.grayLighter;
        }
        clip: true

        Rectangle {
            id: pressedIndicator

            anchors.fill: parent
            color: VCColor.white
            opacity: 0.3
            radius: parent.radius
            visible: control.down
        }

    }

}
