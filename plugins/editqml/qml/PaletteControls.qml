import QtQuick 2.0
import editqml 1.0
import editor 1.0

QtObject{

    enum PaletteListMode {
        ObjectContainer,
        PaletteContainer,
        PropertyContainer,
        NodeEditor
    }

    enum PaletteListSwap {
        NoSwap,
        Swap
    }

    property QtObject factories : QtObject{

        property Component addQmlBox: Component{ AddQmlBox{} }
        property Component paletteGroup: Component{ PaletteGroup{} }
        property Component objectContainer: Component{ ObjectContainer{} }
        property Component paletteContainer: Component{ PaletteContainer{} }
        property Component propertyContainer: Component{ PropertyContainer{} }
        property Component paletteListView: Component{
            PaletteListView{
                visible: model ? true : false
                onFocusChanged : if ( !focus ) model = null
                z: 2000

                property var selectedHandler : function(){}
                property var cancelledHandler : function(index){}

                onPaletteSelected: selectedHandler(index)
                onCancelled : cancelledHandler()

                onParentChanged: {
                    if (parent) anchors.top = parent.top
                }
            }
        }

    }
    property QtObject theme: lk.layers.workspace.themes.current

    property bool instructionsShaping: false
    /////////////////// FACTORY FUNCTIONS

    function createAddQmlBox(parent){
        return factories.addQmlBox.createObject(parent)
    }

    function createPaletteGroup(parent){
        return factories.paletteGroup.createObject(parent)
    }

    function createObjectContainer(parent){
        return factories.objectContainer.createObject(parent)
    }

    function createPaletteContainer(parent){
        return factories.paletteContainer.createObject(parent)
    }

    function createPaletteListView(parent, style){
        var plv = factories.paletteListView.createObject(parent)
        if (style) plv.style = style
        return plv
    }

    function createPropertyContainer(parent){
        return factories.propertyContainer.createObject(parent)
    }

    //////////////////////////////////////////////

    function openDefaultPalette(editingFragment, editor, paletteBoxParent, objectRoot){
        var defaultPaletteName = editor.documentHandler.codeHandler.defaultPalette(editingFragment)
        if ( defaultPaletteName.length ){
            openPaletteByName(defaultPaletteName,
                              editingFragment,
                              editor,
                              paletteBoxParent,
                              objectRoot)
        }
    }

    function openPaletteByName(paletteName, editingFragment, editor, paletteBoxParent, objectRoot){

        if ( !editingFragment ) return
        var palette = editor.documentHandler.codeHandler.expand(editingFragment, {
            "palettes" : [paletteName]
        })

        var paletteBox = openPalette(palette,
                                     editingFragment,
                                     editor,
                                     paletteBoxParent,
                                     objectRoot)
        if (paletteBox) paletteBox.moveEnabledSet = false

    }

    function openPalette(palette, editingFragment, editor, paletteBoxParent, objectRoot){
        if (!palette) return

        if (palette.type === "qml/Object")
        {
            palette.documentHandler = editor.documentHandler
            palette.editor = editor
            editor.documentHandler.codeHandler.populateNestedObjectsForFragment(editingFragment)
            palette.editingFragment = editingFragment
        }

        if ( palette.item ){
            var newPaletteBox = createPaletteContainer(paletteBoxParent)
            palette.item.x = 2
            palette.item.y = 2

            newPaletteBox.child = palette.item
            newPaletteBox.palette = palette

            newPaletteBox.name = palette.name
            newPaletteBox.type = palette.type

            paletteBoxParent.palettesOpened.push(palette.name)
            newPaletteBox.documentHandler = editor.documentHandler
            newPaletteBox.cursorRectangle = editor.getCursorRectangle()
            newPaletteBox.editorPosition = editor.cursorWindowCoords()
            newPaletteBox.editor = editor

            if (objectRoot && objectRoot.compact) objectRoot.expand()

            return newPaletteBox
        }

        if (objectRoot) objectRoot.expandOptions(palette)
    }

    function addChildObjectContainer(parentObjectContainer, ef, expandDefault, propertyContainer, propPalette){
        if (propertyContainer)
            propertyContainer.isAnObject = true

        var childObjectContainer = createObjectContainer(parentObjectContainer.groupsContainer)

        if (propertyContainer){
            propertyContainer.childObjectContainer = childObjectContainer
            childObjectContainer.isForProperty = true
        }
        childObjectContainer.parentObjectContainer = parentObjectContainer

        childObjectContainer.editor = parentObjectContainer.editor
        childObjectContainer.editingFragment = ef
        childObjectContainer.title = ef.typeName() + (ef.objectId() ? ("#" + ef.objectId()) : "")

        var paletteBoxGroup = createPaletteGroup(childObjectContainer.groupsContainer)
        paletteBoxGroup.editingFragment = ef
        paletteBoxGroup.codeHandler = parentObjectContainer.editor.documentHandler.codeHandler

        paletteBoxGroup.leftPadding = 7
        paletteBoxGroup.topPadding = 7
        ef.visualParent = paletteBoxGroup
        childObjectContainer.paletteGroup = paletteBoxGroup

        if (propertyContainer)
            propertyContainer.valueContainer = childObjectContainer
        if ( propPalette ){
            openPaletteByName(propPalette,
                          ef,
                          childObjectContainer.editor,
                          paletteBoxGroup,
                          childObjectContainer)
        } else if (expandDefault) {
            openDefaultPalette(ef,
                                 childObjectContainer.editor,
                                 childObjectContainer.paletteGroup,
                                 childObjectContainer)
        }

        if (propertyContainer)
            propertyContainer.paletteAddButtonVisible = false

        return childObjectContainer
    }


    function addItemToRuntime(codeHandler, ef, insPosition, parentType, type){
        var opos = codeHandler.addItem(
                    insPosition,
                    parentType,
                    type)

        codeHandler.addItemToRuntime(ef, type, project.appRoot())


        var res = codeHandler.openNestedConnection(
            ef, opos, project.appRoot()
        )

        return res
    }

    function addItem(container, insPosition, parentType, type, isForNode){
        var codeHandler = container.editor.documentHandler.codeHandler

        var ef = addItemToRuntime(codeHandler, container.editingFragment, insPosition, parentType, type)

        if (ef){
            if (!isForNode && container.compact) container.expand()
            else container.editingFragment.signalObjectAdded(ef)
            if (!isForNode && container.compact) container.sortChildren()
        }
    }

    function compose(container, isForNode, objectGraph){
        var codeHandler = container.editor.documentHandler.codeHandler

        var isGroup = container.editingFragment.isGroup()

        var position = 0
        if (isGroup){
            position = container.editingFragment.position()
        } else {
            position = container.editingFragment.valuePosition() +
                       container.editingFragment.valueLength() - 1
        }

        var isReadOnly = container.editingFragment.isOfFragmentType(QmlEditFragment.ReadOnly)

        var filter = (isReadOnly ? CodeQmlHandler.ReadOnly : 0 ) | (isForNode ? CodeQmlHandler.ForNode : 0)
        var addContainer = codeHandler.getAddOptions(position, filter, container.editingFragment)

        if ( !addContainer )
            return

        var addBoxItem = createAddQmlBox(null)

        if (!addBoxItem) return
        addBoxItem.addContainer = addContainer
        addBoxItem.codeQmlHandler = codeHandler
        if (isForNode)
            addBoxItem.mode = addBoxItem.mode | AddQmlBox.DisplayMode.WithFunctions
        if (isGroup)
            addBoxItem.mode = addBoxItem.mode | AddQmlBox.DisplayMode.NoObjects

        var oct = container.parent

        // capture y
        var octit = container
        var octY = 0
        while ( octit && (octit.objectName !== 'editorBox' && octit.objectName !== 'objectPalette')){
            octY += octit.y
            octit = octit.parent
        }
        if ( octit && (octit.objectName === 'editorBox' || octit.objectName === 'objectPalette')){
            octY += octit.y
        }

        var rect = Qt.rect(oct.x + 150, octY, oct.width, 25)
        var cursorCoords = container.editor.cursorWindowCoords()

        var addBox = lk.layers.editor.environment.createEditorBox(
            addBoxItem, rect, cursorCoords, lk.layers.editor.environment.placement.bottom
        )

        addBox.color = 'transparent'
        addBoxItem.cancel = function(){

            if (isForNode) objectGraph.activateFocus()
            addBoxItem.destroy()
            addBox.destroy()
        }

        addBoxItem.accept = function(type, data, mode){
            if ( addBoxItem.activeIndex === 1 ){ // property

                // check if property is opened already
                for (var i = 0; i < container.propertiesOpened.length; ++i){
                    if (container.propertiesOpened[i] === data){
                        if (!isForNode && container.compact) container.expand()
                        addBoxItem.destroy()
                        addBox.destroy()
                        return
                    }
                }

                if (!isForNode && container.compact) container.expand()


                var propsWritable = codeHandler.propertiesWritable(container.editingFragment)

                var ef = null

                var isWritable = propsWritable[data]
                if (mode === AddQmlBox.AddPropertyMode.AddAsReadOnly){
                    isWritable = false
                }

                if (isWritable){
                    var ppos = codeHandler.addProperty(
                        addContainer.model.addPosition,
                        addContainer.objectType,
                        type,
                        data,
                        true,
                        isGroup ? container.editingFragment : null
                    )

                    ef = codeHandler.openNestedConnection(
                        container.editingFragment, ppos, project.appRoot()
                    )

                    if (isGroup) ef.addFragmentType(QmlEditFragment.GroupChild)
                } else {
                    ef = codeHandler.createReadOnlyFragment(container.editingFragment, data)
                }

                if (ef) {
                    container.editingFragment.signalPropertyAdded(ef)
                }

                if (!ef) {
                    lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
                    console.error("Error: Can't create a palette in a non-compiled program")
                }


            } else if ( addBoxItem.activeIndex === 2 ){ // object

                addItem(container, addContainer.model.addPosition, addContainer.objectType, data, isForNode)

            } else if ( addBoxItem.activeIndex === 3 ){ // event

                // check if event is opened already
                for (var i = 0; i < container.propertiesOpened.length; ++i){
                    if (container.propertiesOpened[i] === data){
                        if (!isForNode && container.compact) container.expand()
                        addBoxItem.destroy()
                        addBox.destroy()
                        return
                    }
                }

                var ppos = codeHandler.addEvent(
                    addContainer.model.addPosition,
                    addContainer.objectType,
                    type,
                    data,
                    true
                )

                var ef = codeHandler.openNestedConnection(
                    container.editingFragment, ppos, project.appRoot()
                )

                if (ef) {
                    container.editingFragment.signalPropertyAdded(ef)
                    if (!isForNode && container.compact) container.sortChildren()
                }

                if (!ef) {
                    lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
                    console.error("Error: Can't create a palette in a non-compiled program")
                }

            } else if (isForNode && addBoxItem.activeIndex === 4 ){
                addSubobject(container.nodeParent, data, container.nodeParent.item.id ? 1 : 0, null)
            }

            if (isForNode) objectGraph.activateFocus()
            addBoxItem.destroy()
            addBox.destroy()
        }


        addBoxItem.assignFocus()
        lk.layers.workspace.panes.setActiveItem(addBox, container.editor)
    }

    function addPropertyContainer(objectContainer, ef, expandDefault, propPalette){
        for (var i = 0; i < objectContainer.propertiesOpened.length; ++i)
            if (objectContainer.propertiesOpened[i] === ef.identifier()) return

        var codeHandler = objectContainer.editor.documentHandler.codeHandler
        var propertyContainer = createPropertyContainer(objectContainer.groupsContainer)

        propertyContainer.title = ef.identifier()
        propertyContainer.documentHandler = objectContainer.editor.documentHandler

        propertyContainer.editor = objectContainer.editor
        propertyContainer.editingFragment = ef

        if ( codeHandler.isForAnObject(ef)){
            var childObjectContainer = addChildObjectContainer(objectContainer, ef, !instructionsShaping, propertyContainer, propPalette)
            childObjectContainer.expand()
        } else {
            propertyContainer.valueContainer = createPaletteGroup()
            ef.visualParent = propertyContainer.valueContainer

            propertyContainer.valueContainer.editingFragment = ef
            propertyContainer.valueContainer.codeHandler = objectContainer.editor.documentHandler.codeHandler

            if ( propPalette ){
                openPaletteByName(propPalette,
                              ef,
                              objectContainer.editor,
                              propertyContainer.valueContainer)

            } else if (expandDefault){
                openDefaultPalette(ef,
                                     objectContainer.editor,
                                     propertyContainer.valueContainer)
            }
        }
        ef.visualParent.owner = propertyContainer

        objectContainer.propertiesOpened.push(ef.identifier())
        ef.incrementRefCount()
    }

    function createObjectContainerForFragment(editor, ef){
        var codeHandler = editor.documentHandler.codeHandler
        var editorBox = lk.layers.editor.environment.createEmptyEditorBox(editor.textEdit)

        var objectContainer = createObjectContainer(lk.layers.editor.environment.content)
        objectContainer.editor = editor
        objectContainer.editingFragment = ef
        objectContainer.title = ef.typeName() + (ef.objectId() ? ("#" + ef.objectId()) : "")
        var paletteBoxGroup = createPaletteGroup(objectContainer.groupsContainer)
        paletteBoxGroup.editingFragment = ef
        ef.visualParent = paletteBoxGroup

        paletteBoxGroup.leftPadding = 7
        paletteBoxGroup.topPadding = 7

        paletteBoxGroup.codeHandler = codeHandler
        objectContainer.paletteGroup = paletteBoxGroup

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        editorBox.setChild(objectContainer, rect, cursorCoords, lk.layers.editor.environment.placement.top)

        editorBox.color = theme.colorScheme.middleground
        editorBox.border.width = 1
        editorBox.border.color = "#141c25"


        var rootPos = codeHandler.findRootPosition()
        if (ef.position() === rootPos){
            objectContainer.contentWidth = Qt.binding(function(){
                return objectContainer.containerContentWidth > objectContainer.editorContentWidth ? objectContainer.containerContentWidth : objectContainer.editorContentWidth
            })
            editor.editor.rootShaped = true
        }
        return objectContainer
    }

    function createEditorBoxForFragment(editor, ef, shape){
        var codeHandler = editor.documentHandler.codeHandler
        var editorBox = lk.layers.editor.environment.createEmptyEditorBox(shape ? editor.textEdit : null)

        var paletteBoxGroup = createPaletteGroup(lk.layers.editor.environment.content)
        paletteBoxGroup.editingFragment = ef
        ef.visualParent = paletteBoxGroup
        paletteBoxGroup.codeHandler = codeHandler

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        editorBox.setChild(paletteBoxGroup, rect, cursorCoords, lk.layers.editor.environment.placement.top)
        editorBox.color = theme.colorScheme.middleground
        editorBox.border.width = 1
        editorBox.border.color = "#141c25"

        return editorBox
    }

    function shapeAtPositionWithInstructions(editor, position, instructions){
        instructionsShaping = true
        var codeHandler = editor.documentHandler.codeHandler

        if ("shapeImports" in instructions){
            lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)
        }

        var ef = codeHandler.openConnection(position)

        if (!ef) return

        var objectContainer = createObjectContainerForFragment(editor, ef)
        if (objectContainer.editingFragment.position() === codeHandler.findRootPosition())
            objectContainer.contentWidth = Qt.binding(function(){
                return objectContainer.containerContentWidth > objectContainer.editorContentWidth
                        ? objectContainer.containerContentWidth
                        : objectContainer.editorContentWidth
            })

        ef.incrementRefCount()
        codeHandler.frameEdit(objectContainer.parent, ef)
        shapeContainerWithInstructions(objectContainer, editor, instructions)
        instructionsShaping = false
    }

    function shapeContainerWithInstructions(objectContainer, editor, instructions){

        if (instructions['type'] !== objectContainer.editingFragment.type()) return
        var containers = openEmptyNestedObjects(objectContainer)

        var hasChildren = false

        if ('palettes' in instructions){
            var palettes = instructions['palettes']
            for (var i = 0; i < palettes.length; ++i){
                var palette = palettes[i]
                openPaletteByName(palettes[i],
                              objectContainer.editingFragment,
                              editor,
                              objectContainer.groupsContainer.children[0],
                              objectContainer)

                if (!hasChildren) hasChildren = true
            }
        }

        if ('children' in instructions){
            var children = instructions['children']
            if (children.length === containers.length){
                for (var i = 0; i < containers.length; ++i){
                    shapeContainerWithInstructions(containers[i], editor, children[i])
                    if (!hasChildren) hasChildren = true
                }
            }
        }

        if ('properties' in instructions){
            var properties = instructions['properties']
            for (var i = 0; i < properties.length; ++i){
                var property = properties[i]
                openPropertyInContainer(objectContainer, property)
                if (!hasChildren) hasChildren = true
            }
        }

        if (hasChildren) {
            objectContainer.compact = false
            objectContainer.sortChildren()
        }
    }

    function openPropertyInContainer(objectContainer, property){

        var ef = addPropertyByName(objectContainer, property['name'])

        if (!ef) return

        var propertyContainer = null
        if (ef.visualParent && ef.visualParent.owner)
            propertyContainer = ef.visualParent.owner
        else {
            var codeHandler = objectContainer.editor.documentHandler.codeHandler
            propertyContainer = createPropertyContainer(objectContainer.groupsContainer)

            propertyContainer.title = ef.identifier()
            propertyContainer.documentHandler = objectContainer.editor.documentHandler

            propertyContainer.editor = objectContainer.editor
            propertyContainer.editingFragment = ef
        }

        var isAnObject = ('isAnObject' in property) && property['isAnObject']

        if (isAnObject){

            var childObjectContainer = ef.visualParent.parent.parent.parent
            if ('instructions' in property)
                shapeContainerWithInstructions(childObjectContainer, childObjectContainer.editor, property['instructions'])
        }

        if ('palettes' in property){
            var palettes = property['palettes']

            for (var i = 0; i < palettes.length; ++i)
            {
                var palette = palettes[i]
                openPaletteByName(palette,
                              ef,
                              objectContainer.editor,
                              ef.visualParent)

            }
        }


        objectContainer.propertiesOpened.push(ef.identifier())
        ef.incrementRefCount()
    }

    function addPropertyByName(objectContainer, name){
        for (var i = 0; i < objectContainer.propertiesOpened.length; ++i){
            if (objectContainer.propertiesOpened[i] === name){
                return
            }
        }

        var codeHandler = objectContainer.editor.documentHandler.codeHandler
        return addPropertyByFragment(objectContainer.editingFragment, codeHandler, name)
    }

    function addPropertyByFragment(ef, codeHandler, name){

        var position = ef.valuePosition() +
                       ef.valueLength() - 1

        var addContainer = codeHandler.getAddOptions(position)
        if ( !addContainer )
            return

        addContainer.model.setCategoryFilter(1)
        addContainer.model.setFilter(name)

        if (addContainer.model.rowCount() === 0) return

        var type = addContainer.model.data(addContainer.model.index(0, 0), 256 + 3/*QmlSuggestionModel.Type*/)

        var ppos = codeHandler.addProperty(
            addContainer.model.addPosition, addContainer.objectType, type, name, true
        )

        var propEf = codeHandler.openNestedConnection(
            ef, ppos, project.appRoot()
        )

        if (propEf) {
            ef.signalPropertyAdded(propEf, false)
        }

        return propEf
    }

    function openEmptyNestedObjects(objectContainer){
        var objects = objectContainer.editor.documentHandler.codeHandler.openNestedObjects(objectContainer.editingFragment)

        var containers = []
        for (var i=0; i < objects.length; ++i){
            var childObjectContainer = objectContainer.addObjectFragmentToContainer(objects[i])
            containers.push(childObjectContainer)
        }

        return containers
    }

    function openNestedProperties(objectContainer, expandDefault){
        var properties = objectContainer.editor.documentHandler.codeHandler.openNestedProperties(objectContainer.editingFragment)

        for (var i=0; i < properties.length; ++i){
            objectContainer.addPropertyFragmentToContainer(properties[i], expandDefault)
        }
    }

    function openDefaults(objectContainer){
        openDefaultPalette(objectContainer.editingFragment,
                             objectContainer.editor,
                             objectContainer.paletteGroup,
                             objectContainer)

        openNestedProperties(objectContainer, true)
    }

    function addPaletteList(container, paletteGroup, offset, mode, swap, listParent){
        if (!container.editingFragment) return null

        var palettes = null
        if (container.editingFragment.isOfFragmentType(QmlEditFragment.ReadOnly))
            palettes = container.editor.documentHandler.codeHandler.findPalettesFromFragment(
                container.editingFragment,
                mode === PaletteControls.PaletteListMode.ObjectContainer
            )
        else
            palettes = container.editor.documentHandler.codeHandler.findPalettes(
                container.editingFragment.position(),
                mode === PaletteControls.PaletteListMode.ObjectContainer
            )

        palettes.filterOut(paletteGroup.palettesOpened)

        if (!palettes || palettes.size() === 0) return null

        var paletteList = createPaletteListView(listParent ? listParent : null, theme.selectableListView)

        var palListBox = null
        if (mode !== PaletteControls.PaletteListMode.NodeEditor){
            var p = container.parent
            while (p && p.objectName !== "editor" && p.objectName !== "objectPalette"){
                p = p.parent
            }
            var coords = (mode === PaletteControls.PaletteListMode.ObjectContainer)
                       ? container.objectContainerTitle.mapToItem(p, 0, 0)
                       : container.mapToItem(p, 0, 0)
            palListBox   = lk.layers.editor.environment.createEditorBox(
                paletteList,
                Qt.rect(
                    coords.x + offset.x,
                    coords.y + offset.y - (mode === PaletteControls.PaletteListMode.ObjectContainer && p.objectName === "objectPalette" ? 8 : 0),
                    0, 0),
                mode === PaletteControls.PaletteListMode.PaletteContainer ? Qt.point(container.editor.x, container.editor.y) : Qt.point(p.x, p.y),
                lk.layers.editor.environment.placement.top
            )
            palListBox.color = 'transparent'

        } else {
            container.objectGraph.paletteListOpened = true
        }

        paletteList.forceActiveFocus()
        paletteList.model = palettes


        paletteList.cancelledHandler = function(){
            paletteList.focus = false
            paletteList.model = null
            paletteList.destroy()
            if (mode !== PaletteControls.PaletteListMode.NodeEditor) palListBox.destroy()
            else {
                container.objectGraph.paletteListOpened = false
                container.objectGraph.activateFocus()
            }
        }

        paletteList.selectedHandler = function(index){
            paletteList.focus = false
            paletteList.model = null

            var palette = container.editor.documentHandler.codeHandler.openPalette(container.editingFragment, palettes, index)
            var paletteBox = openPalette(palette,
                                         container.editingFragment,
                                         container.editor,
                                         paletteGroup,
                                         mode === PaletteControls.PaletteListMode.ObjectContainer ? container.parent : null)

            if (paletteBox){
                if (mode === PaletteControls.PaletteListMode.ObjectContainer || mode === PaletteControls.PaletteListMode.NodeEditor){
                    paletteBox.moveEnabledSet = false
                } else if (mode === PaletteControls.PaletteListMode.PaletteContainer){
                    paletteBox.moveEnabledSet = container.moveEnabledGet
                }
            }

            if (mode === PaletteControls.PaletteListMode.PaletteContainer && swap === PaletteControls.PaletteListSwap.Swap){
                container.parent = null
                container.documentHandler.codeHandler.removePalette(container.palette)
                container.destroy()
            }

            paletteList.destroy()
            if (mode !== PaletteControls.PaletteListMode.NodeEditor) palListBox.destroy()
            else {
                container.objectGraph.paletteListOpened = false
                container.objectGraph.activateFocus()
            }
        }

        return paletteList
    }

    function eraseObject(objectContainer){
        var rootDeleted = (objectContainer.editingFragment.position() === objectContainer.editor.documentHandler.codeHandler.findRootPosition())
        objectContainer.editor.documentHandler.codeHandler.deleteObject(objectContainer.editingFragment)

        if (rootDeleted) {
            objectContainer.editor.editor.rootShaped = false
            objectContainer.editor.editor.addRootButton.visible = true
        }
    }

    function paletteToPane(objectContainer){
        if ( objectContainer.pane ){
            objectContainer.closeAsPane()
            return
        }

        var objectPane = lk.layers.workspace.panes.createPane('objectPalette', {}, [400, 400])
        lk.layers.workspace.panes.splitPaneHorizontallyWith(
            objectContainer.editor.parentSplitter,
            objectContainer.editor.parentSplitterIndex(),
            objectPane
        )

        var root = objectContainer.parent

        objectContainer.objectContainerTitle.parent = objectPane.paneHeaderContent
        objectPane.objectContainer = objectContainer
        objectPane.title = objectContainer.title
        objectContainer.pane = objectPane

        root.placeHolder.parent = root

    }

    function closeObjectContainer(objectContainer){
        if ( objectContainer.pane )
            objectContainer.closeAsPane()
        var codeHandler = objectContainer.editor.documentHandler.codeHandler
        objectContainer.collapse()
        var rootPos = codeHandler.findRootPosition()
        if (rootPos === objectContainer.editingFragment.position())
            objectContainer.editor.editor.rootShaped = false

        codeHandler.removeConnection(objectContainer.editingFragment)

        var p = objectContainer.parent.parent
        if ( p.objectName === 'editorBox' ){ // if this is root for the editor box
            p.destroy()
        } else { // if this is nested
            //TODO: Check if this is nested within a property container
            if ( objectContainer.pane )
                objectContainer.closeAsPane()
            objectContainer.parent.destroy()
        }
    }

    function edit(editor){
        var codeHandler = editor.documentHandler.codeHandler

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        var ef = codeHandler.openConnection(editor.textEdit.cursorPosition)
        var palette = codeHandler.edit(ef)

        var editorBox = lk.layers.editor.environment.createEmptyEditorBox()
        var paletteGroup = createPaletteGroup(lk.layers.editor.environment.content)
        editorBox.setChild(paletteGroup, rect, cursorCoords, lk.layers.editor.environment.placement.top)
        paletteGroup.x = 2
        paletteGroup.editingFragment = ef
        paletteGroup.codeHandler = codeHandler
        ef.visualParent = paletteGroup
        editorBox.color = "black"
        editorBox.border.width = 1
        editorBox.border.color = "#141c25"

        var paletteBox = openPalette(palette, ef, editor, paletteGroup)
        palette.item.x = 5
        palette.item.y = 7
        if (paletteBox){
            paletteBox.title = 'Edit'
            paletteBox.titleLeftMargin = 10
            paletteBox.paletteSwapVisible = false
            paletteBox.paletteAddVisible = false
        }
        editorBox.updatePlacement(rect, cursorCoords, lk.layers.editor.environment.placement.top)
        ef.incrementRefCount()
    }

    function palette(editor){
        var codeHandler = editor.documentHandler.codeHandler

        var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition)
        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        if ( !palettes || palettes.size() === 0){
            return
        }

        if ( palettes.size() === 1 ){
            loadPalette(editor, palettes, 0)
        } else {
            //Palette list box

            var palList      = createPaletteListView(null, theme.selectableListView)
            var palListBox   = lk.layers.editor.environment.createEditorBox(
                palList, rect, cursorCoords, lk.layers.editor.environment.placement.bottom
            )
            palListBox.color = 'transparent'
            palList.model    = palettes
            editor.internalFocus = false
            palList.forceActiveFocus()
            lk.layers.workspace.panes.setActiveItem(palList, editor)

            palList.cancelled.connect(function(){
                palList.focus = false
                editor.editor.forceFocus()
                palList.destroy()
                palListBox.destroy()
            })
            palList.paletteSelected.connect(function(index){
                palList.focus = false
                editor.editor.forceFocus()
                palList.destroy()
                palListBox.destroy()
                loadPalette(editor, palettes, index)
            })
        }
    }

    function shape(editor){
        var codeHandler = editor.documentHandler.codeHandler

        var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition, true)
        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        if ( !palettes || palettes.size() === 0 ){
            shapePalette(editor, palettes, -1)
        } else if ( palettes.size() === 1 ){
            shapePalette(editor, palettes, 0)
        } else {
            //Palette list box
            var palList      = globals.paletteControls.createPaletteListView(null, currentTheme.selectableListView)
            var palListBox   = lk.layers.editor.environment.createEditorBox(palList, rect, cursorCoords, lk.layers.editor.environment.placement.bottom)
            palListBox.color = 'transparent'
            palList.model    = palettes
            editor.internalFocus = false
            palList.forceActiveFocus()
            lk.layers.workspace.panes.setActiveItem(palList, editor)

            palList.cancelled.connect(function(){
                palList.focus = false
                editor.editor.forceFocus()
                palList.destroy()
                palListBox.destroy()
            })
            palList.paletteSelected.connect(function(index){
                palList.focus = false
                editor.editor.forceFocus()
                palList.destroy()
                palListBox.destroy()
                shapePalette(editor, palettes, index)
            })
        }
    }

    function bind(editor){
        var codeHandler = editor.documentHandler.codeHandler

        var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition)
        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()
        if ( palettes.size() === 1 ){
            var ef = codeHandler.openConnection(palettes.position())
            ef.incrementRefCount()
            codeHandler.openBinding(ef, palettes, 0)
        } else {
            var palList      = createPaletteListView(null, theme.selectableListView)
            var palListBox   = lk.layers.editor.environment.createEditorBox(palList, rect, cursorCoords, lk.layers.editor.environment.placement.bottom)
            palListBox.color = 'transparent'
            palList.model = palettes
            editor.internalFocus = false
            palList.forceActiveFocus()
            lk.layers.workspace.panes.setActiveItem(palList, editor)

            palList.cancelled.connect(function(){
                palList.focus = false
                editor.editor.forceFocus()
                palList.destroy()
                palListBox.destroy()
            })
            palList.paletteSelected.connect(function(index){
                palList.focus = false
                editor.editor.forceFocus()
                palList.destroy()
                palListBox.destroy()

                var ef = codeHandler.openConnection(palettes.position())
                ef.incrementRefCount()

                codeHandler.openBinding(ef, palettes, index)
            })
        }
    }

    function shapePalette(editor, palettes, index){
        var codeHandler = editor.documentHandler.codeHandler
        var ef = codeHandler.openConnection(palettes.position())

        if (!ef){
            lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't shape palette"
            console.error("Error: Can't shape palette")
            return
        }

        var forAnObject = ef.location === QmlEditFragment.Object
        var editorBox = ef.visualParent ? ef.visualParent.parent : null

        var objectContainer = null
        var paletteBoxGroup = editorBox ? editorBox.child : null
        if ( paletteBoxGroup === null ){
            if (forAnObject){
                objectContainer = createObjectContainerForFragment(editor, ef)
                if (objectContainer.editingFragment.position() === codeHandler.findRootPosition())
                    objectContainer.contentWidth = Qt.binding(function(){
                        return objectContainer.containerContentWidth > objectContainer.editorContentWidth
                                ? objectContainer.containerContentWidth
                                : objectContainer.editorContentWidth
                    })
                objectContainer.expand()
                objectContainer.title = ef.typeName() + (ef.objectId() ? ("#" + ef.objectId()) : "")

                paletteBoxGroup = objectContainer.paletteGroup
                editorBox = objectContainer.parent
            } else {
                editorBox = createEditorBoxForFragment(editor, ef, true)
                paletteBoxGroup = ef.visualParent
            }
        } else {
            var p = paletteBoxGroup
            while (p && p.objectName !== "objectContainer") p = p.parent
            objectContainer = p
        }

        var palette = palettes.size() > 0 && !(forAnObject && palettes.size() === 1)? codeHandler.openPalette(ef, palettes, index) : null
        var forImports = ef.location === QmlEditFragment.Imports

        if (palette){
            if (forImports){
                palette.item.model = codeHandler.importsModel()
                palette.item.editor = editor.editor
            }
            if (forAnObject && !palette.item ){
                objectContainer.expandOptions(palette)
            }

            var paletteBox = openPalette(palette, ef, editor, paletteBoxGroup)
            if (paletteBox) paletteBox.moveEnabledSet = false
        }

        codeHandler.frameEdit(editorBox, ef)

        if (forImports) editor.editor.importsShaped = true
        ef.incrementRefCount()

        return objectContainer ? objectContainer : palette
    }

    function loadPalette(editor, palettes, index){
        var codeHandler = editor.documentHandler.codeHandler

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        var ef = codeHandler.openConnection(palettes.position())

        if (!ef)
        {
            lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
            console.error("Error: Can't create a palette in a non-compiled program")
            return
        }
        ef.incrementRefCount()

        var palette = codeHandler.openPalette(ef, palettes, index)

        var editorBox = ef.visualParent ? ef.visualParent.parent : null
        var paletteBoxGroup = editorBox ? editorBox.child : null

        if ( paletteBoxGroup === null ){
            editorBox = createEditorBoxForFragment(editor, ef, false)
            editorBox.color = "black"
            paletteBoxGroup = ef.visualParent
        }

        var paletteBox = openPalette(palette, ef, editor, paletteBoxGroup)

        editorBox.updatePlacement(rect, cursorCoords, lk.layers.editor.environment.placement.top)
    }

    function convertStateIntoInstructions(){
        var editorPane = lk.layers.workspace.panes.focusPane('editor')
        if ( !editorPane )
            return

        var editor = editorPane.editor
        var codeHandler = editor.documentHandler.codeHandler

        var result = null
        var editingFragments = codeHandler.editingFragments()
        for ( var i = 0; i < editingFragments.length; ++i ){
            if (editingFragments[i].location === 1){
                result = convertObjectIntoInstructions(editingFragments[i])
            }
        }

        if (editor.importsShaped)
            result['shapeImports'] = true

        return result
    }

    function convertObjectIntoInstructions(object, container){
        var result = ({})
        result['type'] = object.type()
        var paletteList = object.paletteList()
        var nameList = []
        for (var p = 0; p < paletteList.length; ++p)
            nameList.push(paletteList[p].name)
        if (nameList.length > 0)
            result['palettes'] = nameList

        var objectContainer = container ? container : object.visualParent.parent.parent.parent

        if (objectContainer.compact) return result
        var childFragments = object.getChildFragments()
        var objects = []
        var properties = []
        for (var i = 0; i < childFragments.length; ++i){
            var frag = childFragments[i]
            if (frag.location === 1)
                objects.push(convertObjectIntoInstructions(frag))
            if (frag.location === 2)
                properties.push(convertPropertyIntoInstructions(frag))
        }

        if (objects.length > 0)
            result['children'] = objects
        if (properties.length > 0)
            result['properties'] = properties

        return result
    }

    function convertPropertyIntoInstructions(prop){
        var result = ({})
        result['name'] = prop.identifier()

        var p = prop.visualParent

        while (p && p.objectName !== "propertyContainer"){
            p = p.parent
        }
        if (!p) return result

        var propertyContainer = p

        if (propertyContainer.isAnObject){
            result['isAnObject'] = true
            var inst = convertObjectIntoInstructions(prop, propertyContainer.childObjectContainer)
            result['instructions'] = inst
        } else {
            var palettes = []
            var paletteList = prop.paletteList()
            for (var k = 0; k < paletteList.length; ++k)
                palettes.push(paletteList[k].name)
            if (palettes.length > 0)
                result['palettes'] = palettes
        }

        return result

    }
}
