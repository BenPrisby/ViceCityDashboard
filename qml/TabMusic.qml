import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

ColumnLayout {
    id: root
    spacing: VCMargin.medium

    onVisibleChanged: {
        if ( !visible )
        {
            // Reset the tab bar index when switching away from this tab.
            tabBar.currentIndex = 0
        }
    }

    TabBar {
        id: tabBar
        Layout.fillWidth: true
        Layout.preferredHeight: height
        spacing: VCMargin.tiny
        background: Rectangle {
            color: VCColor.black
        }

        TabButton {
            id: nowPlayingTabButton
            text: qsTr( "Now Playing" )
            font.pixelSize: VCFont.paragraph
            background: Rectangle {
                color: nowPlayingTabButton.checked ? VCColor.white : VCColor.grayDarker
                radius: 6
            }
        }

        TabButton {
            id: searchTabButton
            text: qsTr( "Search" )
            font.pixelSize: VCFont.paragraph
            background: Rectangle {
                color: searchTabButton.checked ? VCColor.white : VCColor.grayDarker
                radius: 6
            }
        }

        TabButton {
            id: playlistsTabButton
            text: qsTr( "Playlists" )
            font.pixelSize: VCFont.paragraph
            background: Rectangle {
                color: playlistsTabButton.checked ? VCColor.white : VCColor.grayDarker
                radius: 6
            }
        }
    }

    StackLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        currentIndex: tabBar.currentIndex

        TabNowPlaying {}

        TileMusicSearch {}

        TilePlaylists {
            onPlayed: tabBar.currentIndex = 0
        }
    }
}
