import QtQuick 2.3
import visual.shapes 1.0
import workspace.icons 1.0 as Icons

Rectangle{
    id: root
    color: '#333'

    property bool isVertical: true
    property color iconColor: '#ccc'

    signal expand()
    signal close()

    Item{
        id: minimizeButton
        width: 10
        height: 15
        anchors.top: root.isVertical ? closeButton.bottom : parent.top
        anchors.topMargin: 3
        anchors.left: parent.left
        anchors.leftMargin: 5

        Triangle{
            width: 7
            height: 7
            color: root.iconColor
            anchors.centerIn: parent

            rotation: Triangle.Top
        }

        MouseArea{
            id: switchMinimized
            anchors.fill: parent
            onClicked: root.expand()
        }
    }

    Item{
        id: closeButton
        width: 10
        height: 15
        anchors.top: parent.top
        anchors.topMargin: 3
        anchors.left: parent.left
        anchors.leftMargin: root.isVertical ? 5 : 20

        Icons.XIcon{
            anchors.centerIn: parent
            width: 6
            height: 6
            color: root.iconColor
            strokeWidth: 1
        }
        MouseArea{
            id: paletteCloseArea
            anchors.fill: parent
            onClicked: root.close()
        }
    }
}
