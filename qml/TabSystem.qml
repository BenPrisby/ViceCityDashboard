import QtQuick 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

GridLayout {
    id: root
    columns: 2
    rows: 4
    columnSpacing: VCMargin.medium
    rowSpacing: columnSpacing

    Tile {
        id: systemInfoTile
        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        Layout.rowSpan: 2
        introAnimationDelay: 100

        ColumnLayout {
            id: systemInfoLayout
            anchors.fill: parent
            anchors.margins: VCMargin.small
            spacing: VCMargin.medium

            RowLayout {
                id: systemInfoHeaderLayout
                Layout.fillWidth: true
                Layout.maximumHeight: 30
                spacing: VCMargin.medium

                Image {
                    id: systemInfoIcon
                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    sourceSize: Qt.size( Layout.preferredWidth, Layout.preferredHeight )
                    source: "qrc:/images/processor.svg"
                }

                Text {
                    id: systemInfoHeader
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.subHeader
                    color: VCColor.white
                    text: qsTr( "System Information" )
                }
            }

            GridLayout {
                id: systemInfoContentLayout
                Layout.fillWidth: true
                Layout.fillHeight: true
                columns: 2
                columnSpacing: VCMargin.medium
                rowSpacing: ( ethernetIPAddressValue.visible && wifiIPAddressValue.visible ) ? VCMargin.tiny
                                                                                             : VCMargin.small

                Text {
                    id: hostnameLabel
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr( "HOSTNAME" )
                }

                Text {
                    id: hostnameValue
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: VCHub.hostname
                }

                Text {
                    id: ethernetIPAddressLabel
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr( "IP ADDRESS (ETH)" )
                    visible: ethernetIPAddressValue.visible
                }

                Text {
                    id: ethernetIPAddressValue
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: VCHub.ethernetIPAddress
                    visible: text
                }

                Text {
                    id: wifiIPAddressLabel
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr( "IP ADDRESS (WI-FI)" )
                    visible: wifiIPAddressValue.visible
                }

                Text {
                    id: wifiIPAddressValue
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: VCHub.wifiIPAddress
                    visible: text
                }

                Text {
                    id: platformLabel
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr( "PLATFORM" )
                }

                Text {
                    id: platformValue
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: VCHub.platform
                }

                Text {
                    id: architectureLabel
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr( "ARCHITECTURE" )
                }

                Text {
                    id: architectureValue
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: VCHub.architecture
                }

                Text {
                    id: qtVersionLabel
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr( "QT VERSION" )
                }

                Text {
                    id: qtVersionValue
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: VCHub.qtVersion
                }
            }
        }
    }

    Tile {
        id: softwareUpdateTile
        Layout.row: 2
        Layout.column: 0
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        Layout.rowSpan: 2

        ColumnLayout {
            id: softwareUpdateLayout
            anchors.fill: parent
            anchors.margins: VCMargin.small
            spacing: VCMargin.medium

            RowLayout {
                id: softwareUpdateHeaderLayout
                Layout.fillWidth: true
                Layout.maximumHeight: 30
                spacing: VCMargin.medium

                Image {
                    id: softwareUpdateIcon
                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    sourceSize: Qt.size( width, height )
                    source: "qrc:/images/update.svg"
                }

                Text {
                    id: softwareUpdateHeader
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.subHeader
                    color: VCColor.white
                    text: qsTr( "Software Update" )
                }
            }

            Text {
                id: noUpdatesIndicator
                Layout.fillWidth: true
                font.pixelSize: VCFont.body
                color: VCColor.white
                text: qsTr( "No update available" )
            }

            VCButton {
                id: checkButton
                Layout.preferredWidth: 220
                Layout.preferredHeight: height
                text: qsTr( "Check for Update" )
                enabled: false
            }

            Rectangle {
                id: softwareUpdateSeperator
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: VCColor.grayLight
            }

            RowLayout {
                id: currentVersionLayout
                Layout.fillWidth: true

                Text {
                    id: currentVersionLabel
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr( "CURRENT DASHBOARD VERSION" )
                }

                Text {
                    id: currentVersionValue
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: Qt.application.version
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }

    Tile {
        id: settingsTile
        Layout.row: 0
        Layout.column: 1
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        Layout.rowSpan: 3
        introAnimationDelay: 50

        ColumnLayout {
            id: settingsLayout
            anchors.fill: parent
            anchors.margins: VCMargin.small
            spacing: VCMargin.big

            RowLayout {
                id: settingsHeaderLayout
                Layout.fillWidth: true
                Layout.maximumHeight: 30
                spacing: VCMargin.medium

                Image {
                    id: settingsIcon
                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    sourceSize: Qt.size( width, height )
                    source: "qrc:/images/settings.svg"
                }

                Text {
                    id: settingsHeader
                    Layout.fillWidth: true
                    font.pixelSize: VCFont.subHeader
                    color: VCColor.white
                    text: qsTr( "Settings" )
                }
            }

            GridLayout {
                id: settingsControlsLayout
                Layout.fillWidth: true
                columns: 2
                columnSpacing: VCMargin.medium
                rowSpacing: VCMargin.medium

                Text {
                    id: use24HourClockLabel
                    Layout.fillWidth: true
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: qsTr( "24-Hour Clock" )
                }

                VCSwitch {
                    id: use24HourClockSwitch
                    Layout.preferredWidth: width
                    Layout.maximumHeight: 30

                    Component.onCompleted: checked = VCHub.use24HourClock

                    onClicked: VCHub.use24HourClock = checked
                }

                Text {
                    id: darkerBackgroundLabel
                    Layout.fillWidth: true
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: qsTr( "Darker Background" )
                }

                VCSwitch {
                    id: darkerBackgroundSwitch
                    Layout.preferredWidth: width
                    Layout.maximumHeight: 30

                    Component.onCompleted: checked = VCHub.darkerBackground

                    onClicked: VCHub.darkerBackground = checked
                }

                Text {
                    id: screensaverEnabledLabel
                    Layout.fillWidth: true
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: VCFont.body
                    color: VCColor.white
                    text: qsTr( "Screensaver" )
                }

                VCSwitch {
                    id: screensaverEnabledSwitch
                    Layout.preferredWidth: width
                    Layout.maximumHeight: 30

                    Component.onCompleted: checked = VCHub.screensaverEnabled

                    onClicked: VCHub.screensaverEnabled = checked
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }

    Tile {
        id: aboutInfoTile
        Layout.row: 3
        Layout.column: 1
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        introAnimationDelay: 100

        ColumnLayout {
            id: aboutInfoLayout
            anchors.fill: parent
            anchors.margins: VCMargin.medium
            spacing: VCMargin.tiny

            Text {
                id: tagline
                Layout.fillWidth: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: VCFont.paragraph
                color: VCColor.grayLightest
                text: qsTr( "Designed and developed by Ben Prisby" )
            }

            Text {
                id: domain
                Layout.fillWidth: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: VCFont.label
                color: VCColor.grayLightest
                text: qsTr( "benprisby.com" )
            }

            Text {
                id: copyright
                Layout.fillWidth: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: VCFont.label
                color: VCColor.grayLightest
                text: qsTr( "Copyright © " + new Date().getFullYear() )
            }
        }
    }
}
