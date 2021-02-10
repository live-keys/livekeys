import QtQuick 2.3
import paper 1.0

Item{
    id: root
    width: 25
    height: 25

    property Component content: null
    property Tool tool: null

    property bool containsMouse: loaderArea.containsMouse

    Loader{
        id: loader
        anchors.fill: parent
        sourceComponent: root.content
    }

    MouseArea{
        id: loaderArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            if ( root.parent ){
                root.parent.activateTool(root.tool)
            }
        }
    }
}
