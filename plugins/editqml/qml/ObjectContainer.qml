import QtQuick 2.0
import live 1.0
import editor 1.0
import editor.private 1.0

Item{
    id: root
    objectName: "objectContainer"

    property alias paletteGroup: objectContainer.paletteGroup
    property alias groupsContainer: container

    function createNewContainer(parent){
        return objectContainerFactory.createObject(parent)
    }

    property alias editingFragment : objectContainer.editingFragment
    property alias editor : objectContainer.editor
    property alias title : objectContainer.title
    property alias titleHeight : objectContainer.titleHeight
    property alias compact: objectContainer.compact
    property alias topSpacing: objectContainer.topSpacing

    width: objectContainer.width
    height: objectContainer.pane ? 30 : objectContainer.height

    property Rectangle placeHolder : Rectangle{
        height: 30
        width: root.width
        color: "#222"
        radius: 3
        Text{
            anchors.left: parent.left
            anchors.leftMargin: 10
            width: parent.width - 20
            anchors.verticalCenter: parent.verticalCenter
            text: objectContainer.title
            clip: true
            elide: Text.ElideRight
            color: '#82909b'
        }
    }

    function expand(){
        objectContainer.expand()
    }

    function collapse(){
        objectContainer.collapse()
    }

    Item{
        id: objectContainer

        property string title: "Object"

        property Item paletteGroup : null
        property alias groupsContainer: container
        property QtObject editingFragment : null
        property Item editor: null


        property Item pane : null
        property Item wrapper : root

        property Component addBoxFactory: Component{ AddQmlBox{} }
        property Component propertyContainerFactory: Component{ PropertyContainer{} }
        property Component paletteContainerFactory: Component{ PaletteContainer{} }
        property Component paletteGroupFactory : Component{ PaletteGroup{} }

        property int titleHeight: 30
        property bool compact: true

        property int topSpacing: editingFragment && !editingFragment.isRoot() ? 0 : 10

        width: container.width < 160 ? 200 : container.width + 40
        height: container.height < 10 || compact ? 40 : titleHeight + objectContainerTitleWrap.height + topSpacing

        function closeAsPane(){
            objectContainerTitle.parent = objectContainerTitleWrap
            objectContainer.parent = objectContainer.wrapper
            objectContainer.pane.removePane()
            objectContainer.pane = null
            root.placeHolder.parent = null
        }

        function expand(){
            var connections = editor.documentHandler.codeHandler.openNestedObjects(editingFragment)
            for ( var i = 0; i < connections.length; ++i ){
                var ef = connections[i]
                var childObjectContainer = objectContainerFactory.createObject(container)

                childObjectContainer.editor = objectContainer.editor
                childObjectContainer.editingFragment = ef
                childObjectContainer.title = ef.typeName()
                childObjectContainer.x = 20

                var paletteBoxGroup = objectContainer.paletteGroupFactory.createObject(childObjectContainer.groupsContainer)
                paletteBoxGroup.editingFragment = ef
                paletteBoxGroup.codeHandler = editor.documentHandler.codeHandler
                ef.visualParent = paletteBoxGroup

                childObjectContainer.paletteGroup = paletteBoxGroup
                paletteBoxGroup.x = 5
            }

            var properties = editor.documentHandler.codeHandler.openNestedProperties(editingFragment)
            for (var j = 0; j < properties.length; ++j){
                var efp = properties[j]
                var propertyContainer = propertyContainerFactory.createObject(container)
                propertyContainer.z = 3000 + properties.length - j
                container.sortChildren()

                propertyContainer.title = efp.identifier()
                propertyContainer.documentHandler = objectContainer.editor.documentHandler
                propertyContainer.propertyContainerFactory = propertyContainerFactory

                propertyContainer.editor = objectContainer.editor
                propertyContainer.editingFragment = efp

                propertyContainer.valueContainer = objectContainer.paletteGroupFactory.createObject()
                propertyContainer.valueContainer.editingFragment = objectContainer.editingFragment
                propertyContainer.valueContainer.codeHandler = objectContainer.editor.documentHandler.codeHandler
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
                var p = root.parent
                if ( p.objectName === 'editorBox' ){ // if this is root for the editor box
                    p.destroy()
                } else { // if this is nested
                    //TODO: Check if this is nested within a property container
                    if ( objectContainer.pane )
                        objectContainer.closeAsPane()
                    root.destroy()
                }
            }
        }

        Item{
            id: objectContainerTitleWrap
            y: topSpacing
            height: objectContainer.pane ? 0 : titleHeight
            width: parent.width + 10

            ObjectContainerTop{
                id: objectContainerTitle
                anchors.fill: parent
                compact: objectContainer.compact
                color: objectContainer.pane ? objectContainer.pane.topColor : '#062945'

                onToggleCompact: {
                    if ( objectContainer.compact )
                        expand()
                    else
                        collapse()
                }
                onErase: {
                    editor.documentHandler.codeHandler.deleteObject(editingFragment)
                }
                onOpenConnections: {

                }
                onPaletteToPane : {
                    if ( objectContainer.pane === null ){
                        var objectPane = lk.layers.workspace.panes.createPane('objectPalette', {}, [400, 400])
                        lk.layers.workspace.panes.splitPaneHorizontallyWith(
                            objectContainer.editor.parentSplitter,
                            objectContainer.editor.parentSplitterIndex(),
                            objectPane
                        )

                        objectContainerTitle.parent = objectPane.paneHeaderContent
                        objectPane.objectContainer = objectContainer
                        objectPane.title = objectContainer.title
                        objectContainer.pane = objectPane
                        root.placeHolder.parent = root
                    } else {
                        objectContainer.closeAsPane()
                    }
                }
                onClose : {
                    if ( objectContainer.pane )
                        objectContainer.closeAsPane()
                    editor.documentHandler.codeHandler.removeConnection(objectContainer.editingFragment)
                }
                onAddPalette: {
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
                onCompose : {
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
                                container.sortChildren()

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
                                if (compact) expand()
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
                                container.sortChildren()
                                childObjectContainer.editor = objectContainer.editor
                                childObjectContainer.editingFragment = ef
                                childObjectContainer.title = data
                                childObjectContainer.x = 40

                                var paletteBoxGroup = objectContainer.paletteGroupFactory.createObject(childObjectContainer.groupsContainer)
                                paletteBoxGroup.editingFragment = ef
                                paletteBoxGroup.codeHandler = codeHandler
                                ef.visualParent = paletteBoxGroup

                                childObjectContainer.paletteGroup = paletteBoxGroup
                                paletteBoxGroup.x = 5
                                if (compact) expand()
                            }
                        } else if ( addBoxItem.activeIndex === 2 ){
                            var ppos = codeHandler.addEvent(
                                addContainer.eventModel.addPosition, addContainer.objectType, type, data
                            )

                            var ef = codeHandler.openNestedConnection(
                                objectContainer.editingFragment, ppos, project.appRoot()
                            )
                            if ( ef ){
                                var propertyContainer = propertyContainerFactory.createObject(container)
                                container.sortChildren()

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
                            if (compact) expand()
                            // TODO: Add event palette too
                        }
                        addBox.destroy()
                    }

                    addBoxItem.assignFocus()
                    lk.layers.workspace.panes.setActiveItem(addBox, objectContainer.editor)
                }
            }
        }

        PaletteListView{
            id: paletteHeaderList
            visible: model ? true : false
            anchors.top: parent.top
            anchors.topMargin: titleHeight + topSpacing
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
            anchors.topMargin: objectContainerTitleWrap.height + topSpacing
            visible: !compact
            spacing: 0
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

            function sortChildren(){

                if (!objectContainer.parent) return
                if (children.length === 0) return

                var childrenCopy = []
                childrenCopy.push(children[0])
                for (var i=1; i<children.length; ++i)
                {
                    if (!children[i]) continue
                    if (children[i].objectName === "propertyContainer")
                        childrenCopy.push(children[i])
                }

                for (var i=1; i<children.length; ++i)
                {
                    if (!children[i]) continue
                    if (children[i].objectName === "objectContainer")
                        childrenCopy.push(children[i])
                }

                children = childrenCopy
            }
        }



    }
}
