pragma Singleton
import QtQuick 2.12

import com.benprisby.vc.vchub 1.0

QtObject {
    readonly property color black: VCHub.darkerBackground ? "black" : "#1c1c1e"
    readonly property color grayDarker: "#2c2c2e"
    readonly property color grayDark: "#3a3a3c"
    readonly property color gray: "#48484a"
    readonly property color grayLight: "#636366"
    readonly property color grayLighter: "#8e8e93"
    readonly property color grayLightest: "#b1b1b4"
    readonly property color white: "white"
    readonly property color transparent: "transparent"

    readonly property color blue: "#007bff"
    readonly property color green: "#34c759"
    readonly property color indigo: "#5856d6"
    readonly property color orange: "#ff9500"
    readonly property color pink: "#ff2d54"
    readonly property color purple: "#af52de"
    readonly property color red: "#ff3a30"
    readonly property color teal: "#5ac7fa"
    readonly property color yellow: "#ffcc00"
}
