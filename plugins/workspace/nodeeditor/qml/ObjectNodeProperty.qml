import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import editqml 1.0

import workspace 1.0 as Workspace

Item{
    id: propertyItem
    property string propertyName : ''
    
    property Item inPort : null
    property Item outPort : null
    property QtObject node : null
    property var editingFragment: null
    property var documentHandler: null
    property alias propertyTitle: propertyTitle
    property Component paletteContainerFactory: Component{ PaletteContainer{} }

    property var isForObject: editingFragment && editingFragment.isForObject()
    property var editor: null

    signal propertyToBeDestroyed(var name)

    anchors.left: parent.left
    anchors.leftMargin: isForObject ? 30 : 0
    width: Math.max(340 - anchors.leftMargin, paletteContainer.width) + 20
    height: propertyTitle.height + paletteContainer.height

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
                    if (!propertyItem.editingFragment) return

                    var palettes = propertyItem.documentHandler.codeHandler.findPalettes(
                        propertyItem.editingFragment.position(), true)

                    if (palettes.size() ){
                        paletteHeaderList.forceActiveFocus()
                        paletteHeaderList.model = palettes

                        paletteHeaderList.cancelledHandler = function(){
                            paletteHeaderList.focus = false
                            paletteHeaderList.model = null
                        }
                        paletteHeaderList.selectedHandler = function(index){
                            paletteHeaderList.focus = false
                            paletteHeaderList.model = null



                            if ( paletteContainer &&
                                 paletteContainer.objectName === 'paletteGroup' )
                            {
                                var palette = documentHandler.codeHandler.openPalette(propertyItem.editingFragment, palettes, index)

                                var newPaletteBox = paletteContainerFactory.createObject(paletteContainer)

                                palette.item.x = 5
                                palette.item.y = 2

                                newPaletteBox.child = palette.item
                                newPaletteBox.palette = palette
                                newPaletteBox.moveEnabledSet = false
                                newPaletteBox.width = Qt.binding(function(){ return paletteContainer.width })

                                newPaletteBox.name = palette.name
                                newPaletteBox.type = palette.type
                                newPaletteBox.documentHandler = documentHandler
                                newPaletteBox.cursorRectangle = editor.getCursorRectangle()
                                newPaletteBox.editorPosition = editor.cursorWindowCoords()
                                newPaletteBox.paletteContainerFactory = function(arg){
                                    return propertyContainer.paletteContainerFactory.createObject(arg)
                                }

                            }


                        }
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
    }

    PaletteListView{
        id: paletteHeaderList
        visible: model ? true:false
        anchors.top: parent.top
        anchors.topMargin: propertyTitle.height
        width: parent.width
        color: "#0a141c"
        selectionColor: "#0d2639"
        fontSize: 10
        fontFamily: "Open Sans, sans-serif"
        onFocusChanged : if ( !focus ) model = null

        property var selectedHandler : function(){}
        property var cancelledHandler : function(index){}

        onPaletteSelected: selectedHandler(index)
        onCancelled : cancelledHandler()
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
