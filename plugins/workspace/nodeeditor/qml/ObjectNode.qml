import QtQuick 2.3
import QtQuick.Controls 2.1
import editor 1.0
import editor.private 1.0
import editqml 1.0

import workspace.quickqanava 2.0 as Qan

Qan.NodeItem{
    id: root
    
    width: 370
    height: wrapper.height
    
    property string label: ''
    property var properties: []
    property var propertyNames: []
    property alias propertyContainer: propertyContainer
    property var nodeParent: null
    property var editingFragment: null
    property var removeNode: null
    property var addSubobject: null
    property string id: ""
    property var documentHandler: null
    property var editor: null
    property Component paletteContainerFactory: Component{ PaletteContainer{} }
    property Component addBoxFactory: Component{ AddQmlBox{} }

    resizable: false
    selectable: false

    function removePropertyName(name){
        var idx = propertyNames.find(function(str){ return str === name })
        if (idx !== -1){
            propertyNames.splice(idx, 1)
        }
    }

    Rectangle{
        id: wrapper
        width: parent.width
        height: nodeTitle.height + paletteContainer.height + propertyContainer.height + 40
        color: "#112"
        radius: 15
        border.color: "#555"
        border.width: 1
        
        Rectangle{
            id: nodeTitle
            width: parent.width
            height: 30
            anchors.top: parent.top
            color: '#666'
            radius: 5
            
            Text{
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 15
                color: 'white'
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
                        var codeHandler = documentHandler.codeHandler

                        var position = editingFragment.valuePosition() +
                                       editingFragment.valueLength() - 1

                        var addContainer = codeHandler.getAddOptions(position)
                        if ( !addContainer )
                            return

                        var addBoxItem = root.addBoxFactory.createObject()
                        addBoxItem.isForNode = true
                        addBoxItem.addContainer = addContainer
                        addBoxItem.codeQmlHandler = codeHandler

                        addBoxItem.assignFocus()

                        var oct = root.parent

                        // capture y
                        var octit = root
                        var octY = 0
                        while ( octit && octit.objectName !== 'editorBox' ){
                            octY += octit.y
                            octit = octit.parent
                        }
                        if ( octit.objectName === 'editorBox' ){
                            octY += octit.y
                        }

                        var rect = Qt.rect(oct.x + 150, octY, oct.width, 25)
                        var cursorCoords = editor.cursorWindowCoords()

                        var addBox = lk.layers.editor.environment.createEditorBox(
                            addBoxItem, rect, cursorCoords, lk.layers.editor.environment.placement.bottom
                        )
                        addBox.color = 'transparent'
                        addBoxItem.cancel = function(){
                            addBox.destroy()
                        }
                        addBoxItem.accept = function(type, data){
                            if ( addBoxItem.activeIndex === 0){
                                for (var i = 0; i < propertyNames.length; ++i){
                                    if (propertyNames[i] === data){
                                        addBox.destroy()
                                        return
                                    }
                                }

                                var ppos = codeHandler.addProperty(
                                    addContainer.propertyModel.addPosition, addContainer.objectType, type, data, true
                                )

                                var ef = codeHandler.openNestedConnection(
                                    editingFragment, ppos, project.appRoot()
                                )

                                if (ef) {
                                    editingFragment.signalPropertyAdded(ef)
                                }

                                if (!ef) {
                                    lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
                                    console.error("Error: Can't create a palette in a non-compiled program")
                                    return
                                }
                            } else if ( addBoxItem.activeIndex === 1 ){
                                var opos = codeHandler.addItem(addContainer.itemModel.addPosition, addContainer.objectType, data)
                                codeHandler.addItemToRuntime(editingFragment, data, project.appRoot())

                                var ef = codeHandler.openNestedConnection(
                                    editingFragment, opos, project.appRoot()
                                )

                                if (ef)
                                    editingFragment.signalObjectAdded(ef)

                            } else if ( addBoxItem.activeIndex === 2 ){
                                for (var i = 0; i < propertyNames.length; ++i){
                                    if (propertyNames[i] === data){
                                        addBox.destroy()
                                        return
                                    }
                                }

                                var ppos = codeHandler.addEvent(
                                    addContainer.propertyModel.addPosition, addContainer.objectType, type, data, true
                                )

                                var ef = codeHandler.openNestedConnection(
                                    editingFragment, ppos, project.appRoot()
                                )

                                if (ef) {
                                    editingFragment.signalPropertyAdded(ef)
                                }

                                if (!ef) {
                                    lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
                                    console.error("Error: Can't create a palette in a non-compiled program")
                                    return
                                }
                            } else if ( addBoxItem.activeIndex === 3){
                                addSubobject(nodeParent, data, nodeParent.item.id ? 1 : 0, null)

                            }
                            addBox.destroy()
                        }

                        addBoxItem.assignFocus()
                        lk.layers.workspace.panes.setActiveItem(addBox, editor)
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
                            paletteHeaderList.forceActiveFocus()
                            paletteHeaderList.model = palettes
                            paletteHeaderList.cancelledHandler = function(){
                                paletteHeaderList.focus = false
                                paletteHeaderList.model = null
                            }
                            paletteHeaderList.selectedHandler = function(index){
                                paletteHeaderList.focus = false
                                paletteHeaderList.model = null

                                var palette = editor.documentHandler.codeHandler.openPalette(editingFragment, palettes, index)

                                if (palette.type === "qml/Object")
                                {
                                    palette.documentHandler = editor.documentHandler
                                    palette.editor = editor
                                    editor.documentHandler.codeHandler.populateNestedObjectsForFragment(editingFragment)
                                    palette.editingFragment = editingFragment

                                }

                                var newPaletteBox = paletteContainerFactory.createObject(paletteContainer)

                                palette.item.x = 5
                                palette.item.y = 7

                                newPaletteBox.child = palette.item
                                newPaletteBox.palette = palette

                                newPaletteBox.name = palette.name
                                newPaletteBox.type = palette.type
                                newPaletteBox.moveEnabledSet = false
                                newPaletteBox.documentHandler = editor.documentHandler
                                newPaletteBox.cursorRectangle = editor.getCursorRectangle()
                                newPaletteBox.editorPosition = editor.cursorWindowCoords()
                                newPaletteBox.paletteContainerFactory = function(arg){
                                    return objectContainer.paletteContainerFactory.createObject(arg)
                                }
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

        PaletteListView{
            id: paletteHeaderList
            visible: model ? true:false
            anchors.top: parent.top
            anchors.topMargin: nodeTitle.height
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
            for (var i=0; i < propertyNames.length; ++i){
                if (!propertyNames[i].toString().localeCompare(name)) return
            }

            var portState = 2

            if (prop.isWritable) ++portState
            addSubobject(nodeParent, prop.name, portState, prop.connection)
        }

    }
}
