import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.1

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
    property color textColor: "#a0a6b1"
    property color cursorColor: "#a0a6b1"
    property color markerColor: "#a0a6b1"
    property color markerLabelColor: "#999"

    property Component scrollStyleHandle : Item{
        implicitWidth: 10
        implicitHeight: 10
        Rectangle {
            color: root.scrollHandleBackgronud
            anchors.fill: parent
        }
    }

    property Component scrollStyleBackground : Item{
        implicitWidth: 10
        implicitHeight: 10
        Rectangle{
            anchors.fill: parent
            color: root.scrollBackground
        }
    }

    property Component scrollStyleFrame : Rectangle{
        color: root.scrollBackground
    }
    property Component scrollStyleCorner : Rectangle{
        color: root.scrollHandleBackgronud
    }

    property color segmentBackground: "#33333a"
    property color segmentBorder : "#444"
    property color segmentBorderFocus: "#aaa"
}
