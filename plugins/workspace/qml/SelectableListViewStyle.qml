import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import workspace 1.0

QtObject{
    id: root

    property QtObject labelStyle: TextStyle{}
    property color backgroundColor: "#03070a"
    property color selectionBackgroundColor: "#091927"

    property double radius: 0
    property color borderColor: "#000"
    property double borderWidth: 0
    property double opacity: 0.95

    property color scrollBarColor: "#1f2227"

    property Component scrollStyle: ScrollViewStyle{
        transientScrollBars: false
        handle: Item {
            implicitWidth: 10
            implicitHeight: 10
            Rectangle {
                color: root.scrollBarColor
                anchors.fill: parent
            }
        }
        scrollBarBackground: Item{
            implicitWidth: 10
            implicitHeight: 10
            Rectangle{
                anchors.fill: parent
                color: root.backgroundColor
            }
        }
        decrementControl: null
        incrementControl: null
        frame: Item{}
        corner: Rectangle{color: root.backgroundColor}
    }
}
