import QtQuick 2.15
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

VCButton {
    id: root
    text: ( scene && scene[ "name" ] ) ? qsTr( scene[ "name" ] ) : ""
    iconSource: ( scene && scene[ "icon" ] ) ? ( "qrc:/images/" + scene[ "icon" ] + ".svg" ) : ""
    enabled: ( !VCHub.isRunningScene ) && ( "" != text )
    visible: scene

    property int index: 0
    readonly property var scene: VCHub.scenes[ index ]

    onClicked: VCHub.runScene( text )
}
