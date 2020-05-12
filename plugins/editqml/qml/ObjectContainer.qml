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

    property alias paletteGroupFactory: objectContainer.paletteGroupFactory

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

    function expandOptions(options){
        objectContainer.expandOptions(options)
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

        property var propertiesOpened: []

        width: container.width < 260 ? 300 : container.width + 40
        height: container.height < 10 || compact ? 40 : objectContainerTitleWrap.height + topSpacing + /*(paletteGroup ? paletteGroup.height : 0) +*/ container.height

        function closeAsPane(){
            objectContainerTitle.parent = objectContainerTitleWrap
            objectContainer.parent = objectContainer.wrapper
            objectContainer.pane.removePane()
            objectContainer.pane = null
            root.placeHolder.parent = null
        }

        function addObjectFragmentToContainer(ef){
            if (!ef) return
            var childObjectContainer = objectContainerFactory.createObject(container)
            childObjectContainer.editor = objectContainer.editor
            childObjectContainer.editingFragment = ef
            childObjectContainer.title = ef.typeName()
            childObjectContainer.x = 20
            childObjectContainer.y = 10

            var paletteBoxGroup = objectContainer.paletteGroupFactory.createObject(childObjectContainer.groupsContainer)
            paletteBoxGroup.editingFragment = ef
            paletteBoxGroup.codeHandler = editor.documentHandler.codeHandler
            ef.visualParent = paletteBoxGroup

            childObjectContainer.paletteGroup = paletteBoxGroup
            paletteBoxGroup.x = 5
            paletteBoxGroup.y = 10

            ef.incrementRefCount()
        }

        function addPropertyFragmentToContainer(ef){
            if (!ef) return
            var codeHandler = objectContainer.editor.documentHandler.codeHandler

            var propertyContainer = objectContainer.propertyContainerFactory.createObject(container)

            propertyContainer.title = ef.identifier()
            propertyContainer.documentHandler = objectContainer.editor.documentHandler
            propertyContainer.propertyContainerFactory = objectContainer.propertyContainerFactory

            propertyContainer.editor = objectContainer.editor
            propertyContainer.editingFragment = ef


            if ( codeHandler.isForAnObject(ef) ){
                var childObjectContainer = objectContainerFactory.createObject(container)

                childObjectContainer.editor = objectContainer.editor
                childObjectContainer.editingFragment = ef
                childObjectContainer.title = ef.typeName()

                var paletteBoxGroup = objectContainer.paletteGroupFactory.createObject(childObjectContainer.groupsContainer)
                paletteBoxGroup.editingFragment = ef
                paletteBoxGroup.codeHandler = codeHandler
                ef.visualParent = paletteBoxGroup

                childObjectContainer.paletteGroup = paletteBoxGroup
                paletteBoxGroup.x = 5

                propertyContainer.valueContainer = childObjectContainer
                propertyContainer.expandDefaultPalette()

                propertyContainer.paletteAddButtonVisible = false

            } else {

                propertyContainer.valueContainer = objectContainer.paletteGroupFactory.createObject()
                propertyContainer.valueContainer.editingFragment = ef
                propertyContainer.valueContainer.codeHandler = objectContainer.editor.documentHandler.codeHandler
                propertyContainer.expandDefaultPalette()

            }

            objectContainer.propertiesOpened.push(ef.identifier())
            ef.incrementRefCount()
        }
        function expandOptions(options){
            var codeHandler = objectContainer.editor.documentHandler.codeHandler

            if ( 'properties' in options){
                var newProps = options['properties']
                for ( var i = 0; i < newProps.length; ++i ){

                    var propName = newProps[i][0]
                    var propType = codeHandler.propertyType(objectContainer.editingFragment, propName)

                    var propPalette = newProps[i].length > 1 ? newProps[i][1] : ''

                    if ( propType !== '' ){
                        var ppos = codeHandler.addProperty(
                            objectContainer.editingFragment.valuePosition() + objectContainer.editingFragment.valueLength() - 1,
                            objectContainer.editingFragment.typeName(),
                            propType,
                            propName,
                            true
                        )

                        var ef = codeHandler.openNestedConnection(
                            objectContainer.editingFragment, ppos
                        )

                        if ( ef ){
                            ef.incrementRefCount()
                            var propertyContainer = objectContainer.propertyContainerFactory.createObject(container)
                            container.sortChildren()

                            propertyContainer.title = propName
                            propertyContainer.documentHandler = objectContainer.editor.documentHandler
                            propertyContainer.propertyContainerFactory = objectContainer.propertyContainerFactory

                            propertyContainer.editor = objectContainer.editor
                            propertyContainer.editingFragment = ef


                            if ( codeHandler.isForAnObject(ef) ){

                                var childObjectContainer = objectContainerFactory.createObject(container)

                                childObjectContainer.editor = objectContainer.editor
                                childObjectContainer.editingFragment = ef
                                childObjectContainer.title = propType

                                var paletteBoxGroup = objectContainer.paletteGroupFactory.createObject(childObjectContainer.groupsContainer)
                                paletteBoxGroup.editingFragment = ef
                                paletteBoxGroup.codeHandler = codeHandler
                                ef.visualParent = paletteBoxGroup

                                childObjectContainer.paletteGroup = paletteBoxGroup
                                paletteBoxGroup.x = 5

                                propertyContainer.valueContainer = childObjectContainer
                                propertyContainer.expandDefaultPalette()

                                propertyContainer.paletteAddButtonVisible = false

                            } else {
                                propertyContainer.valueContainer = objectContainer.paletteGroupFactory.createObject()
                                propertyContainer.valueContainer.editingFragment = objectContainer.editingFragment
                                propertyContainer.valueContainer.codeHandler = objectContainer.editor.documentHandler.codeHandler
                                propertyContainer.expandDefaultPalette()


                                /*if ( propPalette.length ){
                                    propertyContainer.expandPalette(propPalette)
                                }*/
                            }
                            if (compact)
                                expand()
                        }
                        else {
                            lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
                            console.error("Error: Can't create a palette in a non-compiled program")
                            return
                        }
                    }
                }
            }
        }

        function expand(){
            var objects = editor.documentHandler.codeHandler.openNestedObjects(editingFragment)
            for (var i=0; i < objects.length; ++i){
                addObjectFragmentToContainer(objects[i])
            }

            var properties = editor.documentHandler.codeHandler.openNestedProperties(editingFragment)

            for (var i=0; i < properties.length; ++i){
                addPropertyFragmentToContainer(properties[i])
            }

            container.sortChildren()
            compact = false
        }

        function collapse(){
            for ( var i = 1; i < container.children.length; ++i ){
                var edit = container.children[i].editingFragment
                edit.decrementRefCount()
                if (edit.refCount === 0)
                    editor.documentHandler.codeHandler.removeConnection(edit)
            }

            for (var i=1; i < container.children.length; ++i)
                container.children[i].destroy()

            compact = true
        }

        property Connections editingFragmentRemovals: Connections{
            target: editingFragment
            onConnectionChanged : {
                objectContainerTitle.isBuilder = root.editingFragment.isBuilder()
            }
            onAboutToBeRemoved : {
                var p = root.parent
                if (!p) return
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

        property Connections addFragmentToContainerConn: Connections{
            target: editingFragment
            ignoreUnknownSignals: true
            onObjectAdded: {
                if (compact) expand()
                else objectContainer.addObjectFragmentToContainer(obj)
                container.sortChildren()
            }
            onPropertyAdded: {
                for (var i = 0; i < objectContainer.propertiesOpened.length; ++i){
                    if (objectContainer.propertiesOpened[i] === ef.identifier()){
                        if (compact) expand()
                        addBox.destroy()
                        return
                    }
                }

                if (compact) expand()
                else objectContainer.addPropertyFragmentToContainer(ef)
                container.sortChildren()

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
                isBuilder: root.editingFragment ? root.editingFragment.isBuilder() : false

                onToggleCompact: {
                    if ( objectContainer.compact )
                        expand()
                    else
                        collapse()
                }
                onErase: {
                    editor.documentHandler.codeHandler.deleteObject(editingFragment)
                }
                onToggleConnections: {
                    if ( paletteConnection.model ){
                        paletteConnection.model = null
                    } else {
                        paletteConnection.forceActiveFocus()
                        paletteConnection.model = editingFragment.bindingModel(editor.documentHandler.codeHandler)
                    }
                }
                onRebuild : {
                    editingFragment.rebuild()
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

                    collapse()

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
                onAddPalette: {
                    var editingFragment = objectContainer.editingFragment
                    if ( !editingFragment )
                        return

                    var palettes = editor.documentHandler.codeHandler.findPalettes(editingFragment.position(), true, true)
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

                            if ( palette.item ){
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
                            } else {
                                objectContainer.expandOptions(palette)
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
                            // ADDING PROPERTIES

                            // check if property is opened already
                            for (var i = 0; i < objectContainer.propertiesOpened.length; ++i){
                                if (objectContainer.propertiesOpened[i] === data){
                                    if (compact) expand()
                                    addBox.destroy()
                                    return
                                }
                            }

                            if (compact) expand()

                            var ppos = codeHandler.addProperty(
                                objectContainer.editingFragment.valuePosition() + objectContainer.editingFragment.valueLength() - 1,
                                objectContainer.editingFragment.typeName(),
                                type,
                                data,
                                true
                            )

                            var ef = codeHandler.openNestedConnection(
                                objectContainer.editingFragment, ppos
                            )

                            if (ef) {
                                objectContainer.editingFragment.signalPropertyAdded(ef)
                                container.sortChildren()

                            }

                            if (!ef) {
                                lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
                                console.error("Error: Can't create a palette in a non-compiled program")
                                return
                            }
                        } else if ( addBoxItem.activeIndex === 1 ){

                            // ADDING OBJECT
                            var opos = codeHandler.addItem(addContainer.itemModel.addPosition, addContainer.objectType, data)
                            codeHandler.addItemToRuntime(objectContainer.editingFragment, data, project.appRoot())

                            if (compact) expand()

                            var ef = codeHandler.openNestedConnection(
                                objectContainer.editingFragment, opos
                            )

                            if (ef) {
                                objectContainer.editingFragment.signalObjectAdded(ef)
                                container.sortChildren()
                            }

                        } else if ( addBoxItem.activeIndex === 2 ){

                            // check if property is opened already
                            for (var i = 0; i < objectContainer.propertiesOpened.length; ++i){
                                if (objectContainer.propertiesOpened[i] === data){
                                    if (compact) expand()
                                    addBox.destroy()
                                    return
                                }
                            }

                            var ppos = codeHandler.addEvent(
                                addContainer.eventModel.addPosition, addContainer.objectType, type, data
                            )

                            var ef = codeHandler.openNestedConnection(
                                objectContainer.editingFragment, ppos
                            )

                            if (ef) {
                                objectContainer.editingFragment.signalPropertyAdded(ef)
                                container.sortChildren()

                            }
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


        PaletteConnection{
            id: paletteConnection
            visible: model ? true:false
            anchors.top: parent.top
            anchors.topMargin: 24
            width: parent.width
            color: "#0a141c"
            selectionColor: "#0d2639"
            fontSize: 10
            fontFamily: "Open Sans, sans-serif"
            onFocusChanged : if ( !focus ) model = null

            property var selectedHandler : function(){}
            property var cancelledHandler : function(index){}
        }


        Column{
            id: container

            anchors.top: parent.top
            anchors.topMargin: objectContainerTitleWrap.height + topSpacing
            visible: !compact
            spacing: 10
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
                children[0].z = children.length

                for (var i=1; i<children.length; ++i)
                {
                    if (!children[i]) continue
                    if (children[i].objectName === "propertyContainer")
                    {
                        childrenCopy.push(children[i])
                        children[i].z = children.length - childrenCopy.length
                    }
                }

                for (var i=1; i<children.length; ++i)
                {
                    if (!children[i]) continue
                    if (children[i].objectName === "objectContainer"){
                        children[i].z = children.length - childrenCopy.length
                        childrenCopy.push(children[i])
                    }
                }

                children = childrenCopy
            }
        }



    }
}