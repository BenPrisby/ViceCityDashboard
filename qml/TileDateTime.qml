import QtQuick 2.12
import QtQuick.Layouts 1.12
import VCStyles 1.0

import com.benprisby.vc.vchub 1.0

Tile {
    id: root

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: VCMargin.small
        spacing: VCMargin.small

        Text {
            id: dayOfWeek
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            color: VCColor.white
            font.pixelSize: VCFont.body
            text: {
                switch ( VCHub.currentDateTime.getDay() )
                {
                case 0:   return qsTr( "Sunday" )
                case 1:   return qsTr( "Monday" )
                case 2:   return qsTr( "Tuesday" )
                case 3:   return qsTr( "Wednesday" )
                case 4:   return qsTr( "Thursday" )
                case 5:   return qsTr( "Friday" )
                case 6:   return qsTr( "Saturday" )
                default:  return ""
                }
            }
        }

        Text {
            id: time
            Layout.fillWidth: true
            Layout.fillHeight: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: VCColor.white
            font.pixelSize: VCFont.header
            font.bold: true
            text: VCHub.formatTime( VCHub.currentDateTime )
        }

        Text {
            id: monthAndYear
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            color: VCColor.white
            font.pixelSize: VCFont.body
            text: {
                var month = ""
                switch ( VCHub.currentDateTime.getMonth() )
                {
                case 0:   month = qsTr( "January" );     break;
                case 1:   month = qsTr( "February" );    break;
                case 2:   month = qsTr( "March" );       break;
                case 3:   month = qsTr( "April" );       break;
                case 4:   month = qsTr( "May" );         break;
                case 5:   month = qsTr( "June" );        break;
                case 6:   month = qsTr( "July" );        break;
                case 7:   month = qsTr( "August" );      break;
                case 8:   month =  qsTr( "September" );  break;
                case 9:   month =  qsTr( "October" );    break;
                case 10:  month =  qsTr( "November" );   break;
                case 11:  month =  qsTr( "December" );   break;
                default:                                   break;
                }
                return month += ( " " + VCHub.currentDateTime.getDate() )
            }
        }
    }
}
