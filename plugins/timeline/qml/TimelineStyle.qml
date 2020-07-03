import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1
import workspace 1.0 as Workspace

QtObject{
    id: root

    property color borderColor: "#21252a"
    property color rowBackground: "#0c0d0f"

    property color topHeaderBackgroundColor: "#0b1015"

    property color headerBorderColor: "#1f2227"
    property color headerRowBackground: "#0b1015"

    property color scrollBackground: "#03090d"
    property color scrollHandleBackgronud: "#1f2227"

    property color iconColor: "#a0a6b1"
    property color cursorColor: "#a0a6b1"
    property color markerColor: "#a0a6b1"
    property color markerLabelColor: "#999"

    property QtObject inputStyle: QtObject{
        property QtObject textStyle: Workspace.TextStyle{}
        property QtObject hintTextStyle: Workspace.TextStyle{}
        property color backgroundColor: '#070b0f'
        property color borderColor: '#323232'
        property double borderThickness: 1
        property color textSelectionColor: '#3d4856'
        property double radius: 3
    }

    property QtObject timeLabelStyle: Workspace.TextStyle{}

    property Component scrollStyle: ScrollViewStyle {
        transientScrollBars: false
        handle: Item {
            implicitWidth: 10
            implicitHeight: 10
            Rectangle {
                color: "#1f2227"
                anchors.fill: parent
            }
        }
        scrollBarBackground: Item{
            implicitWidth: 10
            implicitHeight: 10
            Rectangle{
                anchors.fill: parent
                color: 'transparent'
            }
        }
        decrementControl: null
        incrementControl: null
        frame: Item{}
        corner: Rectangle{color: 'transparent'}
    }

    property color segmentBackground: "#33333a"
    property color segmentBorder : "#444"
    property color segmentBorderFocus: "#aaa"
}
