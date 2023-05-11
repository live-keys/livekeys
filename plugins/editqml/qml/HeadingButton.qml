import QtQuick 2.3
import workspace 1.0 as Workspace

Item{
    id: root
    width: 15
    height: 15

    property string tooltip: ''

    property Component content: null
    property QtObject contentItem: icon.item
    signal clicked()

    Loader{
        id: icon
        anchors.verticalCenter: parent.verticalCenter
        sourceComponent: parent.content
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: { root.clicked() }
    }

    Workspace.Tooltip{
        mouseOver: root.tooltip ? mouseArea.containsMouse : null
        text: root.tooltip
    }
}
