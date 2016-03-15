import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

ScrollViewStyle {
    transientScrollBars: false
    handle: Item {
        implicitWidth: 10
        implicitHeight: 10
        Rectangle {
            color: "#ff0000"
            anchors.fill: parent
        }
    }
    scrollBarBackground: Item{
        implicitWidth: 10
        implicitHeight: 10
        Rectangle{
            anchors.fill: parent
            color: "#ffff00"
        }
    }
    decrementControl: null
    incrementControl: null
    frame: Rectangle{color: "#ff00ff"}
    corner: Rectangle{color: "#00f000"}
}
