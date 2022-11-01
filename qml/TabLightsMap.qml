import QtQuick 2.15
import VCStyles 1.0
import com.benprisby.vc.vchub 1.0

Item {
    id: root

    onVisibleChanged: {
        if (visible) {
            VCHub.hue.refreshGroups();
            VCHub.nanoleaf.refreshEffects();
        } else {
            // Clear out any selections when navigating away from this tab.
            hueDevicesRepeater.selectedIndex = -1;
            nanoleafDot.selected = false;

            // Reset opacity for the animation.
            floorPlanMap.opacity = 0;
        }
    }

    Image {
        id: floorPlanMap

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: selectionPrompt.top
        anchors.bottomMargin: VCMargin.medium
        fillMode: Image.Stretch  // BDP: Seems to look the best as long as distortion is minimal
        sourceSize: Qt.size(width, height)
        source: VCHub.localFileToURL(VCHub.homeMap)

        MouseArea {
            id: floorPlanMouseArea

            anchors.fill: parent
            onClicked: {
                // Clear out any selections.
                hueDevicesRepeater.selectedIndex = -1;
                nanoleafDot.selected = false;
            }
        }

        Repeater {
            id: hueDevicesRepeater

            property int selectedIndex: -1

            anchors.fill: parent
            model: VCHub.hue.devices

            delegate: DeviceDot {
                id: hueDeviceDot

                selected: index == hueDevicesRepeater.selectedIndex
                color: {
                    if ((!modelData["isOn"]) || (!modelData["isReachable"])) {
                        return VCColor.grayLighter;
                    }
                    if (modelData["color"]) {
                        return modelData["color"];
                    }
                    if (modelData["ambientColor"]) {
                        return modelData["ambientColor"];
                    }
                    if (modelData["productName"].toLowerCase().includes("filament")) {
                        return "#ffbf00";  // Amber
                    }
                    if (modelData["brightness"]) {
                        return "#f6e9b0";  // Soft white (2800K)
                    }
                    return VCColor.green;
                }
                x: (VCHub.hue.mapModel[modelData["name"]]
                    ? (VCHub.hue.mapModel[modelData["name"]][0] * floorPlanMap.width) : 0)
                   - (width / 2)
                y: (VCHub.hue.mapModel[modelData["name"]]
                    ? (VCHub.hue.mapModel[modelData["name"]][1] * floorPlanMap.height) : 0)
                   - (height / 2)
                onClicked: {
                    nanoleafDot.selected = false;
                    hueDevicesRepeater.selectedIndex = index;
                }
            }

        }

        DeviceDot {
            id: nanoleafDot

            property int colorsIndex: -1
            readonly property var colors: {
                for (var i = 0; i < VCHub.nanoleaf.effects.length; i++) {
                    var effect = VCHub.nanoleaf.effects[i];
                    if ((effect.name === VCHub.nanoleaf.selectedEffect) && effect.colors) {
                        return effect.colors;
                    }
                }
                return [];
            }

            x: VCHub.nanoleaf.mapPoint[0] * floorPlanMap.width
            y: VCHub.nanoleaf.mapPoint[1] * floorPlanMap.height
            color: {
                if (VCHub.nanoleaf.isOn) {
                    if (colorsIndex >= 0) {
                        return colors[colorsIndex];
                    }
                    return VCColor.green;
                }
                return VCColor.grayLighter;
            }
            onColorsChanged: {
                if (nanoleafColorRotationTimer.running) {
                    nanoleafColorRotationTimer.restart();
                    colorsIndex = 0;
                } else {
                    colorsIndex = -1;
                }
            }
            onClicked: {
                hueDevicesRepeater.selectedIndex = -1;
                selected = true;
            }

            Timer {
                id: nanoleafColorRotationTimer

                running: visible && (nanoleafDot.colors.length > 0)
                repeat: true
                interval: 5000
                onTriggered: {
                    if (nanoleafDot.colorsIndex < (nanoleafDot.colors.length - 1)) {
                        nanoleafDot.colorsIndex++;
                    } else {
                        nanoleafDot.colorsIndex = 0;
                    }
                }
                onRunningChanged: nanoleafDot.colorsIndex = running ? 0 : -1
            }

        }

        SequentialAnimation {
            id: introAnimation

            running: root.visible

            PauseAnimation {
                duration: 100
            }

            OpacityAnimator {
                target: floorPlanMap
                from: floorPlanMap.opacity
                to: 1
                duration: 500
                easing.type: Easing.InOutQuad
            }

        }

    }

    Text {
        id: selectionPrompt

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 190
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: VCFont.body
        color: VCColor.white
        text: qsTr("Tap a dot above for more information and controls.")
        visible: (hueDevicesRepeater.selectedIndex < 0) && !nanoleafDot.selected
    }

    TileHueDevice {
        id: selectedHueDeviceInfo

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: selectionPrompt.height
        visible: (hueDevicesRepeater.selectedIndex >= 0) && !nanoleafDot.selected
        device: visible ? VCHub.hue.devices[hueDevicesRepeater.selectedIndex] : null
    }

    TileNanoleaf {
        id: nanoleafInfo

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: selectionPrompt.height
        visible: nanoleafDot.selected
    }

}
