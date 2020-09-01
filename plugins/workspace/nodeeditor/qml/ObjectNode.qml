import QtQuick 2.3
import QtQuick.Controls 2.1
import editor 1.0
import editor.private 1.0
import editqml 1.0

import workspace 1.0 as Workspace
import workspace.quickqanava 2.0 as Qan

Qan.NodeItem{
    id: root
    
    width: 370
    height: wrapper.height
    
    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    property string label: ''
    property var properties: []
    property var propertiesOpened: []
    property alias propertyContainer: propertyContainer
    property var nodeParent: null
    property var editingFragment: null
    property var removeNode: null
    property var addSubobject: null
    property string id: ""
    property var documentHandler: null
    property var editor: null

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls

    resizable: false

    function removePropertyName(name){
        var idx = propertiesOpened.find(function(str){ return str === name })
        if (idx !== -1){
            propertiesOpened.splice(idx, 1)
        }
    }

    function addPropertyToNodeByName(name){
        for (var i = 0; i < propertiesOpened.length; ++i){
            if (propertiesOpened[i] === name){
                return
            }
        }

        var codeHandler = documentHandler.codeHandler

        var position = editingFragment.valuePosition() +
                       editingFragment.valueLength() - 1

        var addContainer = codeHandler.getAddOptions(position)
        if ( !addContainer )
            return

        addContainer.activeIndex = 1
        addContainer.model.setFilter(name)
        if (addContainer.model.rowCount() !== 1) return

        var type = addContainer.model.data(addContainer.model.index(0, 0), 256 + 3/*QmlSuggestionModel.Type*/)

        for (var i=0; i < propertiesOpened.length; ++i){
            if (!propertiesOpened[i].toString().localeCompare(name)) return
        }

        addSubobject(nodeParent, name, 2, null)
    }

    property QtObject defaultStyle : QtObject{
        property color background: "#112"
        property double radius: 15
        property color borderColor: "#555"
        property double borderWidth: 1

        property color titleBackground: "#666"
        property double titleRadius: 5
        property QtObject titleTextStyle : Workspace.TextStyle{}
    }

    property QtObject nodeStyle : defaultStyle

    Rectangle{
        id: wrapper
        width: parent.width
        height: nodeTitle.height + paletteContainer.height + propertyContainer.height + 40
        color: root.nodeStyle.background
        radius: root.nodeStyle.radius
        border.color: root.nodeStyle.borderColor
        border.width: root.nodeStyle.borderWidth
        
        Rectangle{
            id: nodeTitle
            width: parent.width
            height: 30
            anchors.top: parent.top

            color: root.nodeStyle.titleBackground
            radius: root.nodeStyle.titleRadius
            
            Workspace.Label{
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 15
                textStyle: root.nodeStyle.titleTextStyle
                text: root.label
            }

            Item{
                id: eraseButton
                anchors.right: parent.right
                anchors.rightMargin: 40
                anchors.verticalCenter: parent.verticalCenter
                width: 15
                height: 20
                Image{
                    anchors.centerIn: parent
                    source: "qrc:/images/palette-erase-object.png"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        documentHandler.codeHandler.deleteObject(editingFragment)
                    }
                }
            }

            Item{
                id: composeButton
                anchors.right: parent.right
                anchors.rightMargin: 22
                anchors.verticalCenter: parent.verticalCenter
                width: 15
                height: 20
                Image{
                    anchors.centerIn: parent
                    source: "qrc:/images/palette-add-property.png"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        paletteControls.compose(root, true, root.paletteStyle)
                    }
                }
            }

            Item{
                id: paletteAddButton
                anchors.right: parent.right
                anchors.rightMargin: 4
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
                        root.selected = false
                        var palettes = documentHandler.codeHandler.findPalettes(editingFragment.position(), true)
                        if (palettes.size() ){
                            var paletteList = paletteControls.createPaletteListView(wrapper, paletteStyle.selectableListView)
                            paletteList.forceActiveFocus()
                            paletteList.model = palettes

                            paletteList.anchors.topMargin = nodeTitle.height
                            paletteList.width = Qt.binding(function(){ return wrapper.width })
                            paletteList.cancelledHandler = function(){
                                paletteList.focus = false
                                paletteList.model = null
                                paletteList.destroy()
                            }
                            paletteList.selectedHandler = function(index){
                                paletteList.focus = false
                                paletteList.model = null
                                var palette = editor.documentHandler.codeHandler.openPalette(editingFragment, palettes, index)

                                var paletteBox = paletteControls.openPalette(palette,
                                                                             editingFragment,
                                                                             editor,
                                                                             paletteContainer)
                                if (paletteBox) paletteBox.moveEnabledSet = false


                                paletteList.destroy()
                            }
                        }

                    }
                }
            }
        }
        
        Column{
            id: propertyContainer
            spacing: 10
            anchors.top: parent.top
            anchors.topMargin: 50 + paletteContainer.height
            anchors.left: parent.left
            anchors.leftMargin: 5
        }

        PaletteGroup {
            anchors.top: parent.top
            anchors.topMargin: nodeTitle.height
            id: paletteContainer
        }
    }

    Connections {
        target: editingFragment
        ignoreUnknownSignals: true
        onAboutToBeRemoved: {
            if (removeNode)
                removeNode(nodeParent)
        }
        onObjectAdded: {
            if (!addSubobject) return

            documentHandler.codeHandler.populateObjectInfoForFragment(obj)

            var object = obj.objectInfo()
            addSubobject(nodeParent, object.name + (object.id ? ("#" + object.id) : ""), 0, object.connection)
        }
        onPropertyAdded: {
            documentHandler.codeHandler.populatePropertyInfoForFragment(ef)

            var prop = ef.objectInfo()
            var name = prop.name.toString()
            for (var i=0; i < propertiesOpened.length; ++i){
                if (!propertiesOpened[i].toString().localeCompare(name)) return
            }

            var portState = 2

            if (prop.isWritable) ++portState
            addSubobject(nodeParent, prop.name, portState, prop.connection)
        }

    }
}
