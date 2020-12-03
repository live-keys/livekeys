import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import editqml 1.0

import workspace 1.0 as Workspace

Item{
    id: propertyItem
    property string propertyName : ''
    
    property QtObject theme: lk.layers.workspace.themes.current

    property Item inPort : null
    property Item outPort : null
    property QtObject node : null
    property var objectGraph: node ? node.item.objectGraph : null
    property var editingFragment: null
    property var documentHandler: null
    property alias propertyTitle: propertyTitle

    property var isForObject: editingFragment && editingFragment.location === QmlEditFragment.Object
    property var editor: null

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls

    signal propertyToBeDestroyed(var name)

    anchors.left: parent.left
    anchors.leftMargin: isForObject ? 30 : 0
    width: 360
    height: propertyTitle.height + paletteContainer.height

    property int contentWidth: 0

    property QtObject defaultStyle : QtObject{
        property color background: "#333"
        property double radius: 5
        property QtObject textStyle: Workspace.TextStyle{}
    }

    property QtObject style: defaultStyle
    
    Rectangle {
        id: propertyTitle
        radius: propertyItem.style.radius
        color: propertyItem.style.background
        width: parent.width
        height: 30

        Workspace.Label{
            anchors.verticalCenter : parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            text: propertyItem.propertyName
            textStyle: propertyItem.style.textStyle
        }

        Item{
            visible:!isForObject
            id: paletteAddButton
            anchors.right: parent.right
            anchors.rightMargin: 25
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-add.png"
            }
            MouseArea{
                id: paletteAddMouse
                anchors.fill: parent
                onClicked: {
                    var paletteList = paletteControls.addPaletteList(
                        propertyItem,
                        paletteContainer,
                        {"x": 0, "y": 0},
                        PaletteControls.PaletteListMode.NodeEditor,
                        PaletteControls.PaletteListSwap.NoSwap,
                        propertyItem
                    )

                    if (paletteList){
                        paletteList.anchors.topMargin = propertyTitle.height
                        paletteList.width = Qt.binding(function(){return propertyItem.width})
                    }
                }
            }
        }

        Item{
            visible: !isForObject
            id: closeObjectItem
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.top: parent.top
            width: 20
            height: 20
            Text{
                text: 'x'
                color: '#ffffff'
                font.pixelSize: 18
                font.family: "Open Sans"
                font.weight: Font.Light
            }
            MouseArea{
                id: paletteCloseArea
                anchors.fill: parent
                onClicked: {
                    documentHandler.codeHandler.removeConnection(editingFragment)
                    if (editingFragment.refCount > 0)
                    {
                        propertyItem.propertyToBeDestroyed(propertyName)
                        var graph = node.graph
                        if (propertyItem.inPort) {

                            if (propertyItem.inPort.inEdge)
                                graph.removeEdge(propertyItem.inPort.inEdge)

                            graph.removePort(node, propertyItem.inPort)
                        }
                        if (propertyItem.outPort) {
                            for (var i=0; i< propertyItem.outPort.outEdges.length; ++i)
                                graph.removeEdge(propertyItem.outPort.outEdges[i])

                            graph.removePort(node, propertyItem.outPort)
                        }
                        propertyItem.destroy()
                    }
                }
            }
        }
    }

    PaletteGroup {
        anchors.top: parent.top
        anchors.topMargin: propertyTitle.height
        id: paletteContainer
        function updateContentWidth(){
            var max = 360
            for (var i=0; i<children.length; ++i)
                if (children.width > max)
                    max = children.width
            propertyItem.contentWidth = max + 25
            node.item.resizeNode()
        }
        onChildrenChanged: {
            updateContentWidth()
        }
        onWidthChanged: {
            updateContentWidth()
        }
        editingFragment: propertyItem.editingFragment

    }
    
    Connections {
        target: editingFragment
        onAboutToBeRemoved: {
            propertyItem.propertyToBeDestroyed(propertyName)
            var graph = node.graph
            if (propertyItem.inPort) {

                if (propertyItem.inPort.inEdge)
                    graph.removeEdge(propertyItem.inPort.inEdge)

                graph.removePort(node, propertyItem.inPort)
            }
            if (propertyItem.outPort) {
                for (var i=0; i< propertyItem.outPort.outEdges.length; ++i)
                    graph.removeEdge(propertyItem.outPort.outEdges[i])

                graph.removePort(node, propertyItem.outPort)
            }
            propertyItem.destroy()
        }
        ignoreUnknownSignals: true
    }

}
