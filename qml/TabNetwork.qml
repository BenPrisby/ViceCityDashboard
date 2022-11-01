import QtCharts 2.15
import QtQuick 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0
import com.benprisby.vc.vchub 1.0

ColumnLayout {
    id: root

    readonly property int graphTimeRange: 60 * 60  // 1 hour

    spacing: VCMargin.medium
    onVisibleChanged: {
        if (visible) {
            VCHub.piHole.refreshHistoricalData();
        }
    }

    RowLayout {
        id: tilesLayout

        Layout.fillWidth: true
        Layout.maximumHeight: 100
        spacing: VCMargin.medium

        Tile {
            id: enabledTile

            Layout.fillHeight: true
            Layout.preferredWidth: height
            introAnimationDelay: 150

            ColumnLayout {
                id: enabledLayout

                anchors.fill: parent
                anchors.margins: VCMargin.small
                spacing: VCMargin.small

                Text {
                    id: enabledLabel

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr("PI-HOLE\nENABLED")
                }

                Rectangle {
                    id: enabledIndicator

                    Layout.fillHeight: true
                    Layout.preferredWidth: height
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    radius: width / 2
                    color: VCHub.piHole.isEnabled ? VCColor.green : VCColor.red
                }

            }

        }

        Tile {
            id: totalQueriesTile

            Layout.fillWidth: true
            Layout.fillHeight: true
            introAnimationDelay: 50

            ColumnLayout {
                id: totalQueriesLayout

                anchors.fill: parent
                anchors.margins: VCMargin.small

                Text {
                    id: totalQueriesLabel

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr("TOTAL QUERIES")
                }

                Text {
                    id: totalQueriesValue

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.header
                    font.bold: true
                    color: VCColor.white
                    text: VCHub.formatInt(VCHub.piHole.totalQueries)
                }

            }

        }

        Tile {
            id: blockedQueriesTile

            Layout.fillWidth: true
            Layout.fillHeight: true
            introAnimationDelay: 200

            ColumnLayout {
                id: blockedQueriesLayout

                anchors.fill: parent
                anchors.margins: VCMargin.small

                Text {
                    id: blockedQueriesLabel

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr("BLOCKED QUERIES")
                }

                Text {
                    id: blockedQueriesValue

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.header
                    font.bold: true
                    color: VCColor.white
                    text: VCHub.formatInt(VCHub.piHole.blockedQueries)
                }

            }

        }

        Tile {
            id: blockPercentageTile

            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                id: blockPercentageLayout

                anchors.fill: parent
                anchors.margins: VCMargin.small

                Text {
                    id: blockPercentageLabel

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr("BLOCK PERCENTAGE")
                }

                Text {
                    id: blockPercentageValue

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.header
                    font.bold: true
                    color: VCColor.white
                    text: VCHub.formatPercentage(VCHub.piHole.percentBlocked)
                }

            }

        }

        Tile {
            id: blockedDomainsTile

            Layout.fillWidth: true
            Layout.fillHeight: true
            introAnimationDelay: 100

            ColumnLayout {
                id: blockedDomainsLayout

                anchors.fill: parent
                anchors.margins: VCMargin.small

                Text {
                    id: blockedDomainsLabel

                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.label
                    font.capitalization: Font.AllUppercase
                    color: VCColor.grayLightest
                    text: qsTr("BLOCKED DOMAINS")
                }

                Text {
                    id: blockedDomainsValue

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: VCFont.header
                    font.bold: true
                    color: VCColor.white
                    text: VCHub.formatInt(VCHub.piHole.blockedDomains)
                }

            }

        }

    }

    Tile {
        id: queriesGraphTile

        Layout.fillWidth: true
        Layout.fillHeight: true
        introAnimationDelay: 250

        ChartView {
            id: queriesGraph

            anchors.fill: parent
            title: qsTr("Queries (Last 24h)")
            animationDuration: 200
            animationEasingCurve.type: Easing.InOutQuad
            legend.visible: false
            antialiasing: true
            theme: ChartView.ChartThemeDark
            margins.top: 0
            margins.left: 0
            margins.right: 0
            margins.bottom: 0
            backgroundColor: VCColor.transparent
            Component.onCompleted: titleFont.pixelSize = VCFont.body

            Connections {
                function onHistoricalDataChanged() {
                    barSeriesXAxis.categories = VCHub.piHole.historicalData["timestamps"];
                    blockedQueriesSet.values = VCHub.piHole.historicalData["blockedQueries"];
                    allowedQueriesSet.values = VCHub.piHole.historicalData["allowedQueries"];
                    barSeriesYAxis.max = VCHub.piHole.historicalData["maxTotalQueries"];
                }

                target: VCHub.piHole
            }

            StackedBarSeries {
                id: barSeries

                BarSet {
                    id: blockedQueriesSet

                    label: qsTr("Blocked")
                    borderWidth: 0
                    borderColor: VCColor.transparent
                    color: VCColor.red
                }

                BarSet {
                    id: allowedQueriesSet

                    label: qsTr("Allowed")
                    borderWidth: 0
                    borderColor: VCColor.transparent
                    color: VCColor.green
                }

                axisX: BarCategoryAxis {
                    id: barSeriesXAxis

                    visible: false
                }

                axisY: ValueAxis {
                    id: barSeriesYAxis

                    min: 0
                    max: 1000
                    labelFormat: "%i"
                    labelsFont.pixelSize: VCFont.label
                }

            }

        }

    }

    Tile {
        id: blockPercentageGraphTile

        Layout.fillWidth: true
        Layout.fillHeight: true
        introAnimationDelay: 300

        ChartView {
            id: blockPercentageGraph

            anchors.fill: parent
            title: qsTr("Block Percentage (Last 24h)")
            animationDuration: 200
            animationEasingCurve.type: Easing.InOutQuad
            legend.visible: false
            antialiasing: true
            theme: ChartView.ChartThemeDark
            margins.top: 0
            margins.left: 0
            margins.right: 0
            margins.bottom: 0
            backgroundColor: VCColor.transparent
            Component.onCompleted: titleFont.pixelSize = VCFont.body

            Connections {
                function onHistoricalDataChanged() {
                    lineSeries.removePoints(0, lineSeries.count);
                    var timestamps = VCHub.piHole.historicalData["timestamps"];
                    var blockPercentages = VCHub.piHole.historicalData["blockPercentages"];
                    for (var i = 0; i < timestamps.length; i++) {
                        lineSeries.append(timestamps[i], blockPercentages[i]);
                    }
                    lineSeriesXAxis.min = timestamps[0];
                    lineSeriesXAxis.max = timestamps[timestamps.length - 1];
                    lineSeriesYAxis.min = VCHub.piHole.historicalData["minBlockPercentage"];
                    lineSeriesYAxis.max = VCHub.piHole.historicalData["maxBlockPercentage"];
                }

                target: VCHub.piHole
            }

            LineSeries {
                id: lineSeries

                color: VCColor.red
                width: 2

                axisX: ValueAxis {
                    id: lineSeriesXAxis

                    visible: false
                }

                axisY: ValueAxis {
                    id: lineSeriesYAxis

                    labelsFont.pixelSize: VCFont.label
                }

            }

        }

    }

    Timer {
        id: historicalDataRefreshTimer

        running: root.visible
        repeat: true
        interval: 60 * 1000
        onTriggered: VCHub.piHole.refreshHistoricalData()
    }

}
