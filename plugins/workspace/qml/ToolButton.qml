import QtQuick 2.3
import workspace 1.0

Item{
    id: root
    width: 20
    height: 20

    property Component content: null
    property Tool tool: null

    property bool containsMouse: mouseArea.containsMouse

    Loader{
        id: loader
        anchors.fill: parent
        sourceComponent: root.content
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            if ( root.parent ){
                root.parent.activateTool(root.tool)
            }
        }
    }
}
