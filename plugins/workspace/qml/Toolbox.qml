import QtQuick 2.3
import QtQuick.Layouts 1.12
import workspace 1.0

Rectangle{
    id: root
    width: 50
    height: 300
    color: style.background
    border.width: style.borderWidth
    border.color: style.borderColor

    default property alias contents: layout.children

    property QtObject defaultStyle: QtObject{
        property color background: "#333"
        property color borderColor: "#555"
        property double borderWidth: 1
    }
    property QtObject style: defaultStyle

    function activateTool(tool){
        if ( root.selectedTool ){
            root.selectedTool.deactivate()
            if ( root.selectedTool.infoBarContent ){
                root.selectedTool.infoBarContent.parent = null
            }
        }

        root.selectedTool = tool
//        root.infoBar = tool.infoBar
        if ( tool ){
            if ( tool.infoBarContent ){
                tool.infoBarContent.parent = root.infoBar
            }
            root.selectedTool.activate()
        }
    }

    property Tool selectedTool: null
    property QtObject canvas: null
    property QtObject infoBar: null
    property QtObject propertyBar: null

    GridLayout{
        id: layout
        anchors.margins: 3
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        columns: 2

        function activateTool(tool){
            root.activateTool(tool)
        }
    }
}
