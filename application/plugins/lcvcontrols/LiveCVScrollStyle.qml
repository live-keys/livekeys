import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

ScrollViewStyle {
    transientScrollBars: false
    handle: Item {
        implicitWidth: 10
        implicitHeight: 10
        Rectangle {
            color: "#0b1f2e"
            anchors.fill: parent
        }
    }
    scrollBarBackground: Item{
        implicitWidth: 10
        implicitHeight: 10
        Rectangle{
            anchors.fill: parent
            color: "#091823"
        }
    }
    decrementControl: null
    incrementControl: null
    frame: Rectangle{color: "transparent"}
    corner: Rectangle{color: "#091823"}
}
