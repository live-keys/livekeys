import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import editqml 1.0

import workspace 1.0 as Workspace
import visual.input 1.0 as Input

Item{
    id: propertyItem
    property string propertyName : ''
    
    property QtObject theme: lk.layers.workspace.themes.current
    objectName: "objectNodeProperty"

    property Item inPort : null
    property Item outPort : null
    property QtObject node : null
    property var objectGraph: node ? node.item.objectGraph : null
    property var editingFragment: null
    property var documentHandler: null
    property alias propertyTitle: propertyTitle
    property alias paletteContainer: paletteContainer

    property var isForObject: editingFragment && editingFragment.location === QmlEditFragment.Object
    property var editor: null

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls

    signal propertyToBeDestroyed(var name)

    signal updateContentWidth()

    onUpdateContentWidth:
        paletteContainer.updateContentWidth()

    anchors.left: parent.left
    anchors.leftMargin: isForObject ? 30 : 0
    height: propertyTitle.height + paletteContainer.height


    property int contentWidth: 355 - anchors.leftMargin

    property QtObject defaultStyle : QtObject{
        property color background: "#333"
        property double radius: 5
        property QtObject textStyle: Input.TextStyle{}
    }

    property QtObject style: defaultStyle
    
    z: -1

    function destroyObjectNodeProperty(){
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

        var children = propertyItem.paletteContainer.children
        for (var idx = 0; idx < children.length; ++idx)
            children[idx].destroy()

        propertyItem.destroy()
    }

    Rectangle {
        id: propertyTitle
        radius: propertyItem.style.radius
        color: propertyItem.style.background
        width: parent.width - 10
        height: 30

        Input.Label{
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
                    var coords = propertyItem.mapToItem(node.item, 0, 0)
                    var paletteList = paletteControls.addPaletteList(
                        propertyItem,
                        paletteContainer,
                        Qt.rect(coords.x, coords.y ,1,1),
                        PaletteControls.PaletteListMode.NodeEditor,
                        PaletteControls.PaletteListSwap.NoSwap,
                        node.item
                    )

                    if (paletteList){
                        paletteList.anchors.topMargin = coords.y + 30
                        paletteList.anchors.left = node.item.left
                        paletteList.anchors.leftMargin = coords.x
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
                        destroyObjectNodeProperty()
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
            var max = 355
            for (var i=0; i<children.length; ++i){
                if (children[i].width + 10 > max)
                    max = children[i].width + 10
            }
            propertyItem.contentWidth = max + propertyItem.anchors.leftMargin
            if (node)
                node.item.resizeNode()
        }
        onChildrenChanged: {
            updateContentWidth()
        }
        editingFragment: propertyItem.editingFragment

    }
    
    Connections {
        target: editingFragment
        function onAboutToBeRemoved(){
            destroyObjectNodeProperty()
        }
        ignoreUnknownSignals: true
    }

}
