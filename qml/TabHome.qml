import QtQuick 2.15
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

GridLayout {
    id: root
    columns: 4
    rows: 4
    columnSpacing: VCMargin.medium
    rowSpacing: columnSpacing

    TileDateTime {
        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        introAnimationDelay: 200
    }

    TileLightsSummary {
        Layout.row: 1
        Layout.column: 0
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        introAnimationDelay: 100
    }

    TileNetworkSummary {
        Layout.row: 0
        Layout.column: 1
        Layout.rowSpan: 2
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
    }

    TileWeather {
        Layout.row: 0
        Layout.column: 2
        Layout.columnSpan: 2
        Layout.rowSpan: 3
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        introAnimationDelay: 50
    }

    TileInsults {
        Layout.row: 2
        Layout.column: 0
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        Layout.columnSpan: 2
        introAnimationDelay: 150
    }

    TileNowPlaying {
        Layout.row: 3
        Layout.column: 0
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        Layout.columnSpan: 2
        introAnimationDelay: 300
    }

    TileScenes {
        Layout.row: 3
        Layout.column: 2
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: Layout.columnSpan
        Layout.preferredHeight: Layout.rowSpan
        Layout.columnSpan: 2
        introAnimationDelay: 250
    }
}
