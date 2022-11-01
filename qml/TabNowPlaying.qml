import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0
import com.benprisby.vc.vchub 1.0

GridLayout {
    id: root

    columns: 2
    columnSpacing: VCMargin.medium
    rowSpacing: columnSpacing
    onVisibleChanged: {
        if (visible) {
            VCHub.spotify.refreshUserProfile();
        } else {
            selectionMask.visible = false;
        }
    }

    Tile {
        id: playerTile

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.columnSpan: 2

        ColumnLayout {
            id: playerLayout

            anchors.fill: parent
            anchors.topMargin: VCMargin.medium
            anchors.leftMargin: VCMargin.big
            anchors.rightMargin: anchors.leftMargin
            anchors.bottomMargin: anchors.topMargin
            spacing: VCMargin.medium

            RowLayout {
                id: trackLayout

                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: VCMargin.bigger

                TrackAlbumArt {
                    id: albumArt

                    Layout.preferredHeight: 180
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignVCenter
                }

                ColumnLayout {
                    id: trackInfoLayout

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: VCMargin.small

                    Text {
                        id: trackName

                        Layout.fillWidth: true
                        font.pixelSize: VCFont.subHeader
                        color: VCColor.white
                        wrapMode: Text.WordWrap
                        text: VCHub.spotify.isActive ? VCHub.spotify.trackName : ""
                    }

                    Text {
                        id: trackArtist

                        Layout.fillWidth: true
                        font.pixelSize: VCFont.body
                        color: VCColor.white
                        wrapMode: Text.WordWrap
                        text: VCHub.spotify.isActive ? VCHub.spotify.trackArtist : ""
                    }

                    Text {
                        id: trackAlbum

                        Layout.fillWidth: true
                        font.pixelSize: VCFont.body
                        color: VCColor.white
                        wrapMode: Text.WordWrap
                        text: VCHub.spotify.isActive ? VCHub.spotify.trackAlbum : ""
                    }

                    Text {
                        id: playlistName

                        Layout.fillWidth: true
                        font.pixelSize: VCFont.body
                        color: VCColor.grayLighter
                        wrapMode: Text.WordWrap
                        text: VCHub.spotify.playlistName
                        visible: text
                    }

                }

            }

            ColumnLayout {
                id: trackPositionLayout

                Layout.fillWidth: true
                spacing: 0

                VCSlider {
                    id: trackPositionSlider

                    property double trackProgress: VCHub.spotify.trackPosition / VCHub.spotify.trackDuration

                    Layout.fillWidth: true
                    Layout.preferredHeight: height
                    from: 0
                    to: 1
                    enabled: VCHub.spotify.isActive
                    onTrackProgressChanged: {
                        if (!pressed) {
                            value = trackProgress;
                        }
                    }
                    onPressedChanged: {
                        // Ensure the final chosen value is commanded.
                        if (!pressed) {
                            VCHub.spotify.seek(value * VCHub.spotify.trackDuration);
                        }
                    }
                    onMoved: {
                        // Throttle requests to keep the number of requests reasonable.
                        if (!trackPositionSliderHoldOffTimer.running) {
                            VCHub.spotify.seek(value * VCHub.spotify.trackDuration);
                            trackPositionSliderHoldOffTimer.start();
                        }
                    }

                    Timer {
                        id: trackPositionSliderHoldOffTimer

                        running: false
                        repeat: false
                        interval: 50
                    }

                    // Animate value changes to eliminate chopiness in the slider.
                    Behavior on value {
                        enabled: !trackPositionSlider.pressed && (trackPositionSlider.value < targetValue)

                        PropertyAnimation {
                            duration: 1000
                            easing.type: Easing.Linear
                        }

                    }

                }

                RowLayout {
                    id: trackPositionLabelsLayout

                    Layout.fillWidth: true

                    Text {
                        id: currentTrackPosition

                        font.pixelSize: VCFont.label
                        color: VCColor.white
                        text: VCHub.spotify.formatDuration(VCHub.spotify.trackPosition)
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Text {
                        id: trackDuration

                        horizontalAlignment: Text.AlignRight
                        font.pixelSize: VCFont.label
                        color: VCColor.white
                        text: VCHub.spotify.formatDuration(VCHub.spotify.trackDuration)
                    }

                }

            }

            RowLayout {
                id: controlButtonsLayout

                Layout.fillWidth: true
                Layout.preferredHeight: 50
                spacing: VCMargin.big

                VCButton {
                    id: shuffleEnabledButton

                    Layout.preferredWidth: 100
                    Layout.fillHeight: true
                    iconSource: "qrc:/images/shuffle.svg"
                    checkable: true
                    checked: true
                    enabled: VCHub.spotify.isActive
                    onClicked: VCHub.spotify.enableShuffle(checked)

                    Connections {
                        function onShuffleEnabledChanged() {
                            shuffleEnabledButton.checked = VCHub.spotify.shuffleEnabled;
                        }

                        target: VCHub.spotify
                    }

                }

                Item {
                    Layout.fillWidth: true
                }

                VCButton {
                    id: previousButton

                    Layout.preferredWidth: 100
                    Layout.fillHeight: true
                    iconSource: "qrc:/images/previous.svg"
                    enabled: VCHub.spotify.isActive
                    onClicked: {
                        // Restart the song if are at least 10 seconds in, otherwise go to the previous.
                        if (VCHub.spotify.trackPosition >= 10) {
                            VCHub.spotify.seek(0);
                        } else {
                            VCHub.spotify.previous();
                        }
                    }
                }

                VCButton {
                    id: playPauseButton

                    Layout.preferredWidth: 100
                    Layout.fillHeight: true
                    iconSource: VCHub.spotify.isPlaying ? "qrc:/images/pause.svg" : "qrc:/images/play.svg"
                    enabled: VCHub.spotify.isActive
                    onClicked: VCHub.spotify.isPlaying ? VCHub.spotify.pause() : VCHub.spotify.play()
                }

                VCButton {
                    id: nextButton

                    Layout.preferredWidth: 100
                    Layout.fillHeight: true
                    iconSource: "qrc:/images/next.svg"
                    enabled: VCHub.spotify.isActive
                    onClicked: VCHub.spotify.next()
                }

                Item {
                    Layout.fillWidth: true
                }

                VCButton {
                    id: repeatEnabledButton

                    Layout.preferredWidth: 100
                    Layout.fillHeight: true
                    iconSource: (repeatHiddenCheckBox.checkState == Qt.Checked) ? "qrc:/images/repeat-one.svg"
                                                                                : "qrc:/images/repeat.svg"
                    checkable: true
                    checked: repeatHiddenCheckBox.checkState != Qt.Unchecked
                    enabled: VCHub.spotify.isActive

                    // Map off to unchecked, repeat all to partially checked, and repeat one to fully checked.
                    CheckBox {
                        id: repeatHiddenCheckBox

                        function assessState() {
                            if (VCHub.spotify.repeatAllEnabled) {
                                checkState = Qt.PartiallyChecked;
                            } else if (VCHub.spotify.repeatOneEnabled) {
                                checkState = Qt.Checked;
                            } else {
                                checkState = Qt.Unchecked;
                            }
                        }

                        anchors.fill: parent
                        opacity: 0
                        tristate: true
                        onClicked: VCHub.spotify.enableRepeat(checkState != Qt.Unchecked,
                                                              checkState == Qt.PartiallyChecked)

                        Connections {
                            function onRepeatAllEnabledChanged() {
                                repeatHiddenCheckBox.assessState();
                            }

                            function onRepeatOneEnabledChanged() {
                                repeatHiddenCheckBox.assessState();
                            }

                            target: VCHub.spotify
                        }

                    }

                }

            }

        }

    }

    Tile {
        id: deviceTile

        readonly property double inactiveOpacity: 0.3

        Layout.fillWidth: true
        Layout.preferredHeight: 120
        Layout.preferredWidth: Layout.columnSpan
        introAnimationDelay: 200

        ColumnLayout {
            id: deviceLayout

            anchors.fill: parent
            anchors.margins: VCMargin.small
            spacing: VCMargin.tiny

            Text {
                id: deviceName

                Layout.fillWidth: true
                font.pixelSize: VCFont.body
                color: VCColor.white
                text: VCHub.spotify.deviceName
                opacity: VCHub.spotify.isActive ? 1 : deviceTile.inactiveOpacity
            }

            Text {
                id: deviceType

                Layout.fillWidth: true
                font.pixelSize: VCFont.label
                color: VCColor.white
                text: VCHub.spotify.deviceType
                opacity: VCHub.spotify.isActive ? 1 : deviceTile.inactiveOpacity
            }

            RowLayout {
                id: volumeControlLayout

                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: VCMargin.medium

                Image {
                    id: volumeIcon

                    Layout.preferredHeight: 30
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignVCenter
                    sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
                    source: "qrc:/images/volume.svg"
                    opacity: VCHub.spotify.isActive ? 1 : deviceTile.inactiveOpacity
                }

                VCSlider {
                    id: volumeSlider

                    Layout.fillWidth: true
                    Layout.preferredHeight: height
                    Layout.alignment: Qt.AlignVCenter
                    enabled: VCHub.spotify.isActive
                    onPressedChanged: {
                        // Ensure the final chosen value is commanded.
                        if (!pressed) {
                            VCHub.spotify.commandDeviceVolume(value);
                        }
                    }
                    onMoved: {
                        // Throttle requests to keep the number of requests reasonable.
                        if (!volumeSliderHoldOffTimer.running) {
                            VCHub.spotify.commandDeviceVolume(value);
                            volumeSliderHoldOffTimer.start();
                        }
                    }

                    Connections {
                        function onDeviceVolumeChanged() {
                            volumeSlider.value = VCHub.spotify.deviceVolume;
                        }

                        target: VCHub.spotify
                    }

                    Timer {
                        id: volumeSliderHoldOffTimer

                        running: false
                        repeat: false
                        interval: 50
                    }

                }

            }

        }

        VCButton {
            id: transferButton

            anchors.top: parent.top
            anchors.topMargin: VCMargin.small
            anchors.right: parent.right
            anchors.rightMargin: anchors.topMargin
            width: 40
            height: width
            iconSource: "qrc:/images/transfer.svg"
            visible: VCHub.spotify.isActive && (VCHub.spotify.devices.length > 0)
            onClicked: {
                VCHub.spotify.refreshDevices();
                selectionMask.visible = true;
            }
        }

        Rectangle {
            id: selectionMask

            anchors.fill: parent
            color: parent.color
            radius: parent.radius
            visible: false

            MouseArea {
                id: mouseTrap

                anchors.fill: parent
            }

            ListView {
                id: devicesList

                anchors.fill: parent
                anchors.margins: VCMargin.small
                spacing: VCMargin.small
                cacheBuffer: Math.max(0, contentHeight)
                interactive: contentHeight > height
                model: VCHub.spotify.devices

                ScrollBar.vertical: ScrollBar {
                    policy: devicesList.interactive ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
                }

                delegate: RowLayout {
                    width: parent.width
                    height: 40
                    spacing: VCMargin.small

                    VCButton {
                        id: selectionBackButton

                        Layout.fillHeight: true
                        Layout.preferredWidth: height
                        iconSource: "qrc:/images/back.svg"
                        outline: true
                        visible: index == 0
                        onClicked: selectionMask.visible = false
                    }

                    Text {
                        id: deviceDelegateName

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: VCFont.body
                        color: VCColor.white
                        elide: Text.ElideRight
                        text: modelData["name"]
                    }

                    Image {
                        id: preferredDeviceIndicator

                        Layout.preferredHeight: 20
                        Layout.preferredWidth: height
                        Layout.alignment: Qt.AlignVCenter
                        fillMode: Image.PreserveAspectFit
                        sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
                        source: "qrc:/images/star.svg"
                        visible: modelData["name"] === VCHub.spotify.preferredDevice
                    }

                    VCButton {
                        id: deviceDelegateTransferButton

                        Layout.fillHeight: true
                        Layout.preferredWidth: height
                        Layout.rightMargin: devicesList.interactive ? VCMargin.medium : 0  // Leave room for the scrollbar
                        iconSource: "qrc:/images/transfer.svg"
                        enabled: modelData["name"] !== VCHub.spotify.deviceName
                        onClicked: {
                            VCHub.spotify.transfer(modelData["id"]);
                            selectionMask.visible = false;
                        }
                    }

                }

            }

        }

    }

    Tile {
        id: userAccountTile

        Layout.fillWidth: true
        Layout.preferredHeight: 120
        Layout.preferredWidth: Layout.columnSpan
        introAnimationDelay: 100

        RowLayout {
            id: userAccountLayout

            anchors.fill: parent
            anchors.margins: VCMargin.small
            spacing: VCMargin.medium

            Image {
                id: userImage

                Layout.fillHeight: true
                Layout.preferredWidth: height
                sourceSize: Qt.size(Layout.preferredWidth, Layout.preferredHeight)
                source: VCHub.spotify.userImage
            }

            ColumnLayout {
                id: userInfoLayout

                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: VCMargin.tiny

                Text {
                    id: userDisplayName

                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: VCHub.spotify.userName
                }

                Text {
                    id: userEmailAddress

                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: VCFont.label
                    color: VCColor.white
                    text: VCHub.spotify.userEmail
                }

                Text {
                    id: userSubscriptionType

                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr(VCHub.spotify.userSubscription + " SUBSCRIPTION")
                }

            }

            Item {
                Layout.fillWidth: true
            }

        }

    }

}
