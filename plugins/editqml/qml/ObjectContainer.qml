import QtQuick 2.0
import editor 1.0
import editor.private 1.0

Item{
    id: objectContainer

    property string title: "Object"
    objectName: "objectContainer"

    property Item paletteGroup : null
    property alias groupsContainer: container
    property QtObject editingFragment : null
    property Item editor: null
    property Component addBoxFactory: Component{ AddQmlBox{} }
    property Component propertyContainerFactory: Component{ PropertyContainer{} }
    property Component paletteContainerFactory: Component{ PaletteContainer{} }
    property Component paletteGroupFactory : Component{ PaletteGroup{} }

    property int titleHeight: 30
    property bool compact: true

    function expand(){
        var connections = editor.documentHandler.codeHandler.openNestedObjects(editingFragment)
        for ( var i = 0; i < connections.length; ++i ){
            var ef = connections[i]
            var childObjectContainer = objectContainerFactory.createObject(container)

            childObjectContainer.editor = objectContainer.editor
            childObjectContainer.editingFragment = ef
            childObjectContainer.title = ef.type()
            childObjectContainer.x = 50

            var paletteBoxGroup = objectContainer.paletteGroupFactory.createObject(childObjectContainer.groupsContainer)
            paletteBoxGroup.editingFragment = ef
            paletteBoxGroup.codeHandler = editor.documentHandler.codeHandler
            ef.visualParent = paletteBoxGroup

            childObjectContainer.paletteGroup = paletteBoxGroup
            paletteBoxGroup.x = 5
        }

        compact = false
    }

    function collapse(){
        var editingFragmentChildren = editingFragment.getChildFragments()
        for ( var i = 0; i < editingFragmentChildren.length; ++i ){
            var edit = editingFragmentChildren[i]
            editor.documentHandler.codeHandler.removeConnection(edit)
        }

        compact = true
    }

    property Connections editingFragmentRemovals: Connections{
        target: editingFragment
        onAboutToBeRemoved : {
            var p = objectContainer.parent
            if ( p.objectName === 'editorBox' ){ // if this is root for the editor box
                p.destroy()
            } else { // if this is nested
                //TODO: Check if this is nested within a property container
                objectContainer.destroy()
            }
        }
    }

    width: container.width < 200 ? 200 : container.width + 50
    height: container.height < 10 || compact ? 30 : container.height + 20

    Rectangle{
        id: objectContainerTitle
        height: titleHeight
        width: parent.width + 10
        color: '#062945'

        Item{
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            Text{
                anchors.verticalCenter: parent.verticalCenter
                text: compact ? '>' : 'v'
                color: '#ffffff'

                font.pixelSize: 15
            }
            MouseArea{
                id: compactObjectButton
                anchors.fill: parent
                onClicked: {
                    if ( objectContainer.compact )
                        expand()
                    else
                        collapse()
                }
            }
        }

        Text{
            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            text: objectContainer.title
            clip: true
            color: '#82909b'
        }

        Item{
            id: eraseButton
            anchors.right: parent.right
            anchors.rightMargin: 80 - (closeObjectItem.visible ? 0 : 15)
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 80
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-erase-object.png"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    editor.documentHandler.codeHandler.deleteObject(editingFragment)
                }
            }
        }


        Item{
            id: connectionsButton
            anchors.right: parent.right
            anchors.rightMargin: 60 - (closeObjectItem.visible ? 0 : 15)
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-connections.png"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                }
            }
        }


        Item{
            id: paletteAddButton
            anchors.right: parent.right
            anchors.rightMargin: 40 - (closeObjectItem.visible ? 0 : 15)
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
                    var editingFragment = objectContainer.editingFragment
                    if ( !editingFragment )
                        return

                    var palettes = editor.documentHandler.codeHandler.findPalettes(editingFragment.position(), true)
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

                            var newPaletteBox = objectContainer.paletteContainerFactory.createObject(paletteGroup)

                            palette.item.x = 5
                            palette.item.y = 7

                            newPaletteBox.child = palette.item
                            newPaletteBox.palette = palette

                            newPaletteBox.name = palette.name
                            newPaletteBox.type = palette.type
                            newPaletteBox.moveEnabledSet = false
                            newPaletteBox.documentHandler = editor.documentHandler
                            newPaletteBox.cursorRectangle = paletteGroup.cursorRectangle
                            newPaletteBox.editorPosition = paletteGroup.editorPosition
                            newPaletteBox.paletteContainerFactory = function(arg){
                                return objectContainer.paletteContainerFactory.createObject(arg)
                            }
                        }
                    }
                }
            }
        }

        Item{
            id: composeButton
            anchors.right: parent.right
            anchors.rightMargin: 20  - (closeObjectItem.visible ? 0 : 15)
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
                    var codeHandler = objectContainer.editor.documentHandler.codeHandler

                    var position =
                        objectContainer.editingFragment.valuePosition() +
                        objectContainer.editingFragment.valueLength() - 1

                    var addContainer = codeHandler.getAddOptions(position)
                    if ( !addContainer )
                        return

                    var addBoxItem = objectContainer.addBoxFactory.createObject()
                    addBoxItem.addContainer = addContainer
                    addBoxItem.codeQmlHandler = codeHandler

                    var oct = objectContainer.parent

                    // capture y
                    var octit = objectContainer
                    var octY = 0
                    while ( octit && octit.objectName !== 'editorBox' ){
                        octY += octit.y
                        octit = octit.parent
                    }
                    if ( octit.objectName === 'editorBox' ){
                        octY += octit.y
                    }

                    var rect = Qt.rect(oct.x + 150, octY, oct.width, 25)
                    var cursorCoords = objectContainer.editor.cursorWindowCoords()

                    var addBox = lk.layers.editor.environment.createEditorBox(
                        addBoxItem, rect, cursorCoords, lk.layers.editor.environment.placement.bottom
                    )
                    addBox.color = 'transparent'
                    addBoxItem.cancel = function(){
                        addBox.destroy()
                    }
                    addBoxItem.accept = function(type, data){
                        if ( addBoxItem.activeIndex === 0 ){
                            var ppos = codeHandler.addProperty(
                                addContainer.propertyModel.addPosition, addContainer.objectType, type, data, true
                            )

                            var ef = codeHandler.openNestedConnection(
                                objectContainer.editingFragment, ppos, project.appRoot()
                            )
                            if ( ef ){

                                var propertyContainer = propertyContainerFactory.createObject(container)

                                propertyContainer.title = data
                                propertyContainer.documentHandler = objectContainer.editor.documentHandler
                                propertyContainer.propertyContainerFactory = propertyContainerFactory

                                propertyContainer.editor = objectContainer.editor
                                propertyContainer.editingFragment = ef


                                if ( codeHandler.isForAnObject(ef) ){

                                    var childObjectContainer = objectContainerFactory.createObject(container)

                                    childObjectContainer.editor = objectContainer.editor
                                    childObjectContainer.editingFragment = ef
                                    childObjectContainer.title = type

                                    var paletteBoxGroup = objectContainer.paletteGroupFactory.createObject(childObjectContainer.groupsContainer)
                                    paletteBoxGroup.editingFragment = ef
                                    paletteBoxGroup.codeHandler = codeHandler
                                    ef.visualParent = paletteBoxGroup

                                    childObjectContainer.paletteGroup = paletteBoxGroup
                                    paletteBoxGroup.x = 5

                                    propertyContainer.valueContainer = childObjectContainer
                                    propertyContainer.paletteAddButtonVisible = false

                                } else {

                                    propertyContainer.valueContainer = objectContainer.paletteGroupFactory.createObject()
                                    propertyContainer.valueContainer.editingFragment = objectContainer.editingFragment
                                    propertyContainer.valueContainer.codeHandler = objectContainer.editor.documentHandler.codeHandler
                                }
                            }
                            else {
                                lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
                                console.error("Error: Can't create a palette in a non-compiled program")
                                return
                            }
                        } else if ( addBoxItem.activeIndex === 1 ){
                            var opos = codeHandler.addItem(addContainer.itemModel.addPosition, addContainer.objectType, data)
                            codeHandler.addItemToRuntime(objectContainer.editingFragment, data, project.appRoot())

                            var ef = codeHandler.openNestedConnection(
                                objectContainer.editingFragment, opos, project.appRoot()
                            )

                            if ( ef ){
                                var childObjectContainer = objectContainerFactory.createObject(container)

                                childObjectContainer.editor = objectContainer.editor
                                childObjectContainer.editingFragment = ef
                                childObjectContainer.title = data
                                childObjectContainer.x = 50

                                var paletteBoxGroup = objectContainer.paletteGroupFactory.createObject(childObjectContainer.groupsContainer)
                                paletteBoxGroup.editingFragment = ef
                                paletteBoxGroup.codeHandler = codeHandler
                                ef.visualParent = paletteBoxGroup

                                childObjectContainer.paletteGroup = paletteBoxGroup
                                paletteBoxGroup.x = 5
                            }
                        } else if ( addBoxItem.activeIndex === 2 ){
                            var ppos = codeHandler.addEvent(
                                addContainer.propertyModel.addPosition, addContainer.objectType, type, data
                            )

                            // TODO: Add event palette too
                        }
                        addBox.destroy()
                    }

                    addBoxItem.assignFocus()
                    lk.layers.workspace.panes.setActiveItem(addBox, objectContainer.editor)
                }
            }
        }

        Item{
            id: closeObjectItem
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            visible: !(objectContainer.editingFragment && objectContainer.editingFragment.parentFragment())
            Text{
                anchors.verticalCenter: parent.verticalCenter
                text: 'x'
                color: '#ffffff'
            }
            MouseArea{
                id: paletteCloseArea
                anchors.fill: parent
                onClicked: {
                    editor.documentHandler.codeHandler.removeConnection(objectContainer.editingFragment)
                }
            }
        }

    }

    PaletteListView{
        id: paletteHeaderList
        visible: model ? true : false
        anchors.top: parent.top
        anchors.topMargin: titleHeight
        width: 250
        color: "#0a141c"
        selectionColor: "#0d2639"
        fontSize: 10
        fontFamily: "Open Sans, sans-serif"
        onFocusChanged : if ( !focus ) model = null
        z: 2000

        property var selectedHandler : function(){}
        property var cancelledHandler : function(index){}

        onPaletteSelected: selectedHandler(index)
        onCancelled : cancelledHandler()
    }

    Column{
        id: container

        anchors.top: parent.top
        anchors.topMargin: titleHeight
        visible: !compact
        spacing: 20
        width: {
            var maxWidth = 0;
            if ( children.length > 0 ){
                for ( var i = 0; i < children.length; ++i ){
                    if ( children[i].width > maxWidth )
                        maxWidth = children[i].width
                }
            }
            return maxWidth
        }
        height: {
            if (compact) return 0
            var totalHeight = 0;
            if ( children.length > 0 ){
                for ( var i = 0; i < children.length; ++i ){
                    totalHeight += children[i].height
                }
            }
            if ( children.length > 1 )
                return totalHeight + (children.length - 1) * spacing
            else
                return totalHeight
        }
    }


}

