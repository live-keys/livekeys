import QtQuick 2.0
import editqml 1.0
import editor 1.0
import workspace.nodeeditor 1.0

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
        property Component addExtraPropertiesBox: Component{ AddExtraPropertiesBox{} }
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

                onPaletteSelected : function(index){
                    focus = false
                    model = null
                    destroy()
                    selectedHandler(index)
                }
                onCancelled : function(){
                    focus = false
                    model = null
                    destroy()
                    cancelledHandler()
                }
                onParentChanged: {
                    if (parent) anchors.top = parent.top
                }
            }
        }
        property Component suggestionBox: Component {
            SuggestionBox {
                Behavior on opacity {
                    NumberAnimation { duration: 150 }
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

    function createPaletteGroup(parent, ef){
        var pg = factories.paletteGroup.createObject(parent)
        pg.__initialize(ef)
        return pg
    }

    function createObjectContainer(parent, editor, ef){
        var oc = factories.objectContainer.createObject(parent)
        oc.__initialize(editor, ef)
        return oc
    }

    function createPaletteContainer(parent){
        return factories.paletteContainer.createObject(parent)
    }

    function createPaletteListView(parent, style){
        var plv = factories.paletteListView.createObject(parent)
        if (style) plv.style = style
        return plv
    }

    function createPropertyContainer(parent, editor, ef){
        var pc = factories.propertyContainer.createObject(parent)
        pc.__initialize(editor, ef)
        return pc
    }

    function createSuggestionBox(parent, font){
        var sb = factories.suggestionBox.createObject(parent)
        sb.__initialize(font)
        return sb
    }

    //////////////////////////////////////////////

    property QtObject views: QtObject {

        function openPaletteList(style, model, parent, handlers){
            var paletteList = createPaletteListView(parent, style)
            paletteList.model = model

            if (handlers){
                paletteList.cancelledHandler = handlers.onCancelled
                paletteList.selectedHandler = handlers.onSelected
            }

            return paletteList
        }

        function openPaletteListBox(style, model, editorBoxParams, handlers){
            var paletteList = openPaletteList(style, model, null)

            var paletteListBox = lk.layers.editor.environment.createEditorBox(
                paletteList,
                editorBoxParams.aroundRect,
                editorBoxParams.panePosition,
                editorBoxParams.relativePlacement
            )

            paletteList.forceActiveFocus()
            paletteListBox.color = 'transparent'

            if (handlers){
                paletteList.cancelledHandler = function(){
                    handlers.onCancelled(paletteListBox)
                }
                paletteList.selectedHandler = function(index){
                    handlers.onSelected(index, paletteListBox)
                }
            }
            return paletteList
        }

        function openPaletteListForNode(container, paletteGroup, parent){

            var palettes = findPalettesFromFragment(container.editingFragment, paletteGroup.palettesOpened, true)

            if (!palettes.data || palettes.data.length === 0) return null

            var paletteList = openPaletteList(theme.selectableListView, palettes.data, parent,
            {
                onCancelled: function(){
                    container.objectGraph.paletteListOpened = false
                    container.objectGraph.activateFocus()
                },
                onSelected: function(index){
                    var palette = container.editor.documentHandler.codeHandler.expand(container.editingFragment, {
                        "palettes" : [palettes.data[index].name]
                    })

                    var objectRoot = container.objectName === "objectNode" ? container : null
                    var paletteBox = openPalette(palette,
                                                 paletteGroup,
                                                 objectRoot)

                    if (paletteBox){
                        paletteBox.moveEnabledSet = false
                    }

                    container.objectGraph.paletteListOpened = false
                    container.objectGraph.activateFocus()
                }
            })

            container.objectGraph.paletteListOpened = true

            return paletteList
        }

        function openPaletetListBoxForContainer(container, paletteGroup, aroundBox, mode, swap){

            var palettes = findPalettesFromFragment(
                container.editingFragment,
                paletteGroup.palettesOpened,
                mode === PaletteControls.PaletteListMode.ObjectContainer
            )

            if (!palettes.data || palettes.data.length === 0) return null

            var pane = container.parent
            if ( container.pane )
                pane = container.pane
            while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
                pane = pane.parent
                if ( pane && pane.pane && pane.pane.objectName === 'objectPalette' ){
                    pane = pane.pane
                }
            }
            var paneCoords = pane.mapGlobalPosition()

            var editorBoxParams = {
                aroundRect: aroundBox,
                panePosition: Qt.point(paneCoords.x, paneCoords.y - 35),
                relativePlacement: lk.layers.editor.environment.placement.bottom
            }

            var cancelledHandler = function(paletteListBox){
                paletteListBox.destroy()
            }

            var selectedHandler = function(index, paletteListBox){
                var palette = container.editor.documentHandler.codeHandler.expand(container.editingFragment, {
                    "palettes" : [palettes.data[index].name]
                })

                var objectRoot = mode === PaletteControls.PaletteListMode.ObjectContainer
                               ? container.parent
                               : (container.objectName === "objectNode" ? container : null)
                var paletteBox = openPalette(palette,
                                             paletteGroup,
                                             objectRoot)

                if (paletteBox){
                    if (mode === PaletteControls.PaletteListMode.ObjectContainer){
                        paletteBox.moveEnabledSet = false
                    } else if (mode === PaletteControls.PaletteListMode.PaletteContainer){
                        paletteBox.moveEnabledSet = container.moveEnabledGet
                    }
                }

                if (swap === PaletteControls.PaletteListSwap.Swap){

                    var p = container.parent
                    while (p && p.objectName !== "paletteGroup"){
                        p = p.parent
                    }
                    p.palettesOpened = p.palettesOpened.filter(function(name){ return name !== container.palette.name })
                    container.parent = null
                    container.documentHandler.codeHandler.removePalette(container.palette)
                    container.destroy()
                }

                paletteListBox.destroy()
            }

            var handlers = {
                onCancelled: cancelledHandler,
                onSelected: selectedHandler
            }

            var paletteList = openPaletteListBox(
                theme.selectableListView,
                palettes.data,
                {
                    aroundRect: aroundBox,
                    panePosition: Qt.point(paneCoords.x, paneCoords.y - 35),
                    relativePlacement: lk.layers.editor.environment.placement.bottom
                },
                handlers
            )

            return paletteList
        }

        function openAddOptionsBox(addContainer, codeHandler, editorBoxParams, handlers){
            var addBoxItem = createAddQmlBox(null)
            if (!addBoxItem) return null

            addBoxItem.addContainer = addContainer
            addBoxItem.codeQmlHandler = codeHandler

            addBoxItem.categories = handlers.categories

            var addEditorBox = lk.layers.editor.environment.createEditorBox(
                addBoxItem,
                editorBoxParams.aroundRect,
                editorBoxParams.panePosition,
                editorBoxParams.relativePlacement
            )
            
            addEditorBox.color = 'transparent'

            if (handlers){
                addBoxItem.accept = function(selection){
                    handlers.onAccepted(addEditorBox, selection)
                }
                addBoxItem.cancel = function(){
                    handlers.onCancelled(addEditorBox)
                }
                addBoxItem.finalize = function(){
                    handlers.onFinalized(addEditorBox)
                }
            }

            addBoxItem.assignFocus()

            return addEditorBox
        }

        function openAddExtraPropertiesBox(type, handlers){
            var boxItem = factories.addExtraPropertiesBox.createObject(null)
            boxItem.type = type
            boxItem.ready.connect(function(data){
                if ( handlers && handlers.onAccepted ){
                    handlers.onAccepted(data)
                }
                boxItem.destroy()
            })
            boxItem.cancel.connect(function(){
                boxItem.destroy()
            })

            lk.layers.window.dialogs.overlayBox(boxItem)

        }
    
    }

    function findPalettesFromFragment(editingFragment, palettesOpened, includeLayouts){
        if (!editingFragment)
            return null

        var palettes = editingFragment.codeHandler.findPalettesFromFragment(editingFragment)

        palettes.data = filterOutPalettes(
            palettes.data,
            palettesOpened,
            includeLayouts
        )

        return palettes
    }

    function openDefaultPalette(editingFragment, paletteBoxParent, objectRoot){
        if (!editingFragment) return
        var editor = editingFragment.codeHandler.documentHandler.textEdit().getEditor()
        var defaultPaletteName = editor.documentHandler.codeHandler.defaultPalette(editingFragment)
        if ( defaultPaletteName.length ){
            openPaletteByName(defaultPaletteName,
                              editingFragment,
                              paletteBoxParent,
                              objectRoot)
        }
    }

    function openPaletteByName(paletteName, editingFragment, paletteBoxParent, objectRoot){

        if ( !editingFragment ) return
        var editor = editingFragment.codeHandler.documentHandler.textEdit().getEditor()
        var palette = editor.documentHandler.codeHandler.expand(editingFragment, {
            "palettes" : [paletteName]
        })

        var paletteBox = openPalette(palette,
                                     paletteBoxParent,
                                     objectRoot)
        if (paletteBox) paletteBox.moveEnabledSet = false

        return paletteBox
    }

    function openPalette(palette, paletteBoxParent, objectRoot){

        if (!palette) return

        if ( palette.item ){
            var newPaletteBox = createPaletteContainer(paletteBoxParent)
            palette.item.x = 2
            palette.item.y = 2

            newPaletteBox.palette = palette

            if ( paletteBoxParent.palettesOpened )
                paletteBoxParent.palettesOpened.push(palette.name)

            if (objectRoot && objectRoot.compact) objectRoot.expand()

            return newPaletteBox
        }

        if (objectRoot) objectRoot.expandOptions(palette)
    }

    function addChildObjectContainer(parentObjectContainer, ef, expandDefault){
        var childObjectContainer = createObjectContainer(parentObjectContainer.groupsContainer, parentObjectContainer.editor, ef)

        childObjectContainer.parentObjectContainer = parentObjectContainer

        if (expandDefault) {
            openDefaultPalette(ef,
                               childObjectContainer.paletteGroup,
                               childObjectContainer)
        }


        return childObjectContainer
    }


    function addItemToRuntime(ef, insPosition, parentType, type){
        var codeHandler = ef.codeHandler
        var opos = codeHandler.addItem(
                    insPosition,
                    parentType,
                    type)

        codeHandler.addItemToRuntime(ef, type)


        var res = codeHandler.openNestedConnection(ef, opos)

        return res
    }

    function addItemToRuntimeWithNotification(container, insPosition, parentType, type, isForNode){
        var ef = addItemToRuntime(container.editingFragment, insPosition, parentType, type)
        if (!ef)
            return

        if (!isForNode && container.compact)
            container.expand()
        else
            container.editingFragment.signalObjectAdded(ef)
        if (!isForNode && container.compact)
            container.sortChildren()
    }

    function compose(container){
        var codeHandler = container.editor.documentHandler.codeHandler

        var isForNode = container.objectName === "objectNode"
        var isGroup = container.editingFragment.isGroup()

        var objectGraph = null
        if (isForNode){
            var par = container
            while (par && par.objectName !== "objectGraph")
                par = par.parent
            if (par)
                objectGraph = par
        }

        var position = 0
        if (isGroup){
            position = container.editingFragment.position()
        } else {
            position = container.editingFragment.position() + 1
        }

        var filter = 0 | (isForNode ? CodeQmlHandler.ForNode : 0) | (isGroup ? CodeQmlHandler.ReadOnly : 0)

        var addContainer = codeHandler.getAddOptions(position, filter, container.editingFragment)

        if ( !addContainer )
            return

        var oct = container.parent

        var pane = container.parent
        while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
            pane = pane.parent
        }
        var coords = container.mapToItem(pane, 0, 0)

        if ( container.parent.objectName !== 'objectContainer' )
            coords.y = coords.y - 30 // if this container is in the title of a pane

        var paneCoords = pane.mapGlobalPosition()

        var categories = ['properties', 'events']
        if (!isGroup && addContainer.model.supportsObjectNesting()){
            categories.push('objects')
        }
        if (isForNode){
            categories.push('functions')
        }
        var addEditorBox = views.openAddOptionsBox(
            addContainer,
            codeHandler,
            {
                aroundRect: Qt.rect(coords.x + container.width - 180 / 2, coords.y, 30, 30),
                panePosition: Qt.point(paneCoords.x, paneCoords.y - 35),
                relativePlacement: lk.layers.editor.environment.placement.top
            },
            {
                categories: categories,
                onCancelled: function(box){
                    box.child.finalize()
                },
                onFinalized: function(box){
                    if (isForNode) objectGraph.activateFocus()
                    box.child.destroy()
                    box.destroy()
                },
                onAccepted: function(box, selection){
                    if ( selection.category === 'property' ){ // property

                        // check if property is opened already
                        for (var i = 0; i < container.propertiesOpened.length; ++i){
                            if (container.propertiesOpened[i] === selection.name){
                                if (!isForNode && container.compact) container.expand()
                                box.child.finalize()
                                return
                            }
                        }

                        if (!isForNode && container.compact) container.expand()


                        var propsWritable = codeHandler.propertiesWritable(container.editingFragment)

                        var ef = null

                        var isWritable = propsWritable[selection.name]
                        if (selection.mode === AddQmlBox.AddPropertyMode.AddAsReadOnly){
                            isWritable = false
                        }

                        if (isWritable){
                            var ppos = codeHandler.addProperty(
                                selection.position,
                                selection.objectType,
                                selection.type,
                                selection.name,
                                true,
                                isGroup ? container.editingFragment : null
                            )

                            ef = codeHandler.openNestedConnection(
                                container.editingFragment, ppos, project.appRoot()
                            )

                            if (isGroup) ef.addFragmentType(QmlEditFragment.GroupChild)
                        } else {
                            ef = codeHandler.createReadOnlyPropertyFragment(container.editingFragment, selection.name)
                        }

                        if (ef) {
                            container.editingFragment.signalPropertyAdded(ef)
                        }

                        if (!ef) {
                            lk.layers.workspace.messages.pushError("Error: Can't create a palette in a non-compiled program", 1)
                        }

                        var paletteList = ef.paletteList()
                        for ( var i = 0; i < paletteList.length; ++i ){
                            if ( paletteList[i].writer ){
                                paletteList[i].writer()
                                break
                            }
                        }


                    } else if ( selection.category === 'object' ){ // object

                        if ( selection.extraProperties ){
                            views.openAddExtraPropertiesBox(selection.name, {
                                onAccepted: function(propertiesToAdd){
                                    var parentType = selection.objectType
                                    var type = selection.name
                                    var ef = container.editingFragment

                                    var codeHandler = ef.codeHandler

                                    var opos = codeHandler.addItem(selection.position, parentType, type, propertiesToAdd)
                                    codeHandler.addItemToRuntime(ef, type, propertiesToAdd)

                                    var newEf = codeHandler.openNestedConnection(ef, opos)
                                    if (!newEf)
                                        return

                                    if (!isForNode && container.compact)
                                        container.expand()
                                    else
                                        container.editingFragment.signalObjectAdded(newEf)
                                    if (!isForNode && container.compact)
                                        container.sortChildren()

                                },
                                onCancelled: function(){}
                            })
                        } else {
                            addItemToRuntimeWithNotification(container, selection.position, selection.objectType, selection.name, isForNode)
                        }


                    } else if ( selection.category === 'event' ){ // event

                        // check if event is opened already
                        for (var i = 0; i < container.propertiesOpened.length; ++i){
                            if (container.propertiesOpened[i] === selection.name){
                                if (!isForNode && container.compact) container.expand()
                                box.child.finalize()
                                return
                            }
                        }

                        var ppos = codeHandler.addEvent(
                            selection.position,
                            selection.objectType,
                            selection.type,
                            selection.name,
                            true
                        )

                        ef = codeHandler.openNestedConnection(
                            container.editingFragment, ppos, project.appRoot()
                        )

                        if (ef) {
                            container.editingFragment.signalPropertyAdded(ef)
                        }


                    } else if (isForNode && selection.category === 'function' ){
                        container.nodeParent.item.addSubobject(container.nodeParent, selection.name, container.nodeParent.item.id ? ObjectGraph.PortMode.InPort : ObjectGraph.PortMode.None, null, {isMethod: true})
                    }

                    box.child.finalize()
                }
            }
        )

        lk.layers.workspace.panes.setActiveItem(addEditorBox, container.editor)
    }

    function addPropertyContainer(objectContainer, ef, expandDefault){
        for (var i = 0; i < objectContainer.propertiesOpened.length; ++i)
            if (objectContainer.propertiesOpened[i] === ef.identifier()) return

        var codeHandler = objectContainer.editor.documentHandler.codeHandler
        var propertyContainer = createPropertyContainer(objectContainer.groupsContainer, objectContainer.editor, ef)

        if ( codeHandler.isForAnObject(ef)){
            var childObjectContainer = addChildObjectContainer(objectContainer, ef, !instructionsShaping, propertyContainer)
            propertyContainer.childObjectContainer = childObjectContainer
            childObjectContainer.isForProperty = true
            propertyContainer.valueContainer = childObjectContainer
            propertyContainer.paletteAddButtonVisible = false
            childObjectContainer.expand()
        } else {
            propertyContainer.valueContainer = createPaletteGroup(null, ef)

            if (expandDefault){
                openDefaultPalette(ef, propertyContainer.valueContainer)
            }
        }
        ef.visualParent.owner = propertyContainer

        objectContainer.propertiesOpened.push(ef.identifier())
        ef.incrementRefCount()
    }

    function createObjectContainerForFragment(ef){
        var editor = ef.codeHandler.documentHandler.textEdit().getEditor()

        var codeHandler = editor.documentHandler.codeHandler
        var editorBox = lk.layers.editor.environment.createEmptyEditorBox(editor.textEdit)

        var objectContainer = createObjectContainer(lk.layers.editor.environment.content, editor, ef)

        var rect = editor.getCursorRectangle()
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
            editor.rootShaped = true
        }
        return objectContainer
    }

    function createEditorBoxForFragment(ef, shape){
        var editor = ef.codeHandler.documentHandler.textEdit().getEditor()
        var codeHandler = editor.documentHandler.codeHandler
        var par = editor
        if (shape)
            par = editor.textEdit
        var editorBox = lk.layers.editor.environment.createEmptyEditorBox(par)

        var paletteBoxGroup = createPaletteGroup(lk.layers.editor.environment.content, ef)

        var rect = editor.getCursorRectangle()
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

        var objectContainer = createObjectContainerForFragment(ef)
        if (objectContainer.editingFragment.position() === codeHandler.findRootPosition())
            objectContainer.contentWidth = Qt.binding(function(){
                return objectContainer.containerContentWidth > objectContainer.editorContentWidth
                        ? objectContainer.containerContentWidth
                        : objectContainer.editorContentWidth
            })

        ef.incrementRefCount()

        editor.documentHandler.frameBox(objectContainer.parent, ef.position(), ef.length())
        shapeContainerWithInstructions(objectContainer, instructions)
        instructionsShaping = false
    }

    function shapeContainerWithInstructions(objectContainer, instructions){
        var editor = objectContainer.editor
        if (instructions['type'] !== objectContainer.editingFragment.type()) return
        var containers = openEmptyNestedObjects(objectContainer)

        var hasChildren = false

        if ('palettes' in instructions){
            var palettes = instructions['palettes']
            for (var i = 0; i < palettes.length; ++i){
                var palette = palettes[i]
                openPaletteByName(palettes[i],
                              objectContainer.editingFragment,
                              objectContainer.groupsContainer.children[0],
                              objectContainer)

                if (!hasChildren) hasChildren = true
            }
        }

        if ('children' in instructions){
            var children = instructions['children']
            if (children.length === containers.length){
                for (var i = 0; i < containers.length; ++i){
                    shapeContainerWithInstructions(containers[i], children[i])
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
            objectContainer.groupsContainer.sortChildren()
        }
    }

    function openPropertyInContainer(objectContainer, prop){

        var ef = addPropertyByName(objectContainer, prop['name'])

        if (!ef) return

        var propertyContainer = null
        if (ef.visualParent && ef.visualParent.owner)
            propertyContainer = ef.visualParent.owner
        else {
            var codeHandler = objectContainer.editor.documentHandler.codeHandler
            propertyContainer = createPropertyContainer(objectContainer.groupsContainer, objectContainer.editor, ef)
        }

        var isAnObject = ('isAnObject' in prop) && prop['isAnObject']

        if (isAnObject){

            var childObjectContainer = ef.visualParent.parent.parent.parent
            if ('instructions' in prop)
                shapeContainerWithInstructions(childObjectContainer, prop['instructions'])
        }

        if ('palettes' in prop){
            var palettes = prop['palettes']

            for (var i = 0; i < palettes.length; ++i)
            {
                var palette = palettes[i]
                openPaletteByName(palette,
                              ef,
                              ef.visualParent)

            }
        }


        objectContainer.propertiesOpened.push(ef.identifier())
        ef.incrementRefCount()
    }

    function addPropertyByName(container, name){
        if (container.propertiesOpened){
            for (var i = 0; i < container.propertiesOpened.length; ++i){
                if (container.propertiesOpened[i] === name){
                    return
                }
            }
        }

        var codeHandler = container.editor.documentHandler.codeHandler
        return addPropertyByFragment(container.editingFragment, name)
    }

    function addPropertyByFragment(ef, name){

        var codeHandler = ef.codeHandler
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
            addPropertyContainer(objectContainer, properties[i], expandDefault)
        }
    }

    function openDefaults(objectContainer){
        openDefaultPalette(objectContainer.editingFragment,
                           objectContainer.paletteGroup,
                           objectContainer)

        openNestedProperties(objectContainer, true)
    }

    function filterOutPalettes(palettes, names, includeLayouts){
        if ((!names || names.length === 0) && includeLayouts)
            return palettes

        var result = []
        for (var i = 0; i < palettes.length; ++i){
            if (names && names.includes(palettes[i].name)) continue
            if (!includeLayouts && palettes[i].configuresLayout) continue

            result.push(palettes[i])
        }

        return result
    }

    function eraseObject(objectContainer){
        var rootDeleted = (objectContainer.editingFragment.position() === objectContainer.editor.documentHandler.codeHandler.findRootPosition())
        objectContainer.editor.documentHandler.codeHandler.eraseObject(objectContainer.editingFragment, !objectContainer.isForProperty)

        if (rootDeleted) {
            objectContainer.editor.rootShaped = false
            objectContainer.editor.addRootButton.visible = true
        }

        if ( objectContainer.isForProperty ) {
            objectContainer.collapse()
        }
    }

    function paletteToPane(objectContainer){
        if ( objectContainer.pane ){
            objectContainer.closeAsPane()
            return
        }

        objectContainer.expand()

        var objectPane = lk.layers.workspace.panes.createPane('objectPalette', {}, [400, 400])
        lk.layers.workspace.panes.splitPaneHorizontallyWith(
            objectContainer.editor.parentSplitView,
            objectContainer.editor.parentSplitViewIndex(),
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
            objectContainer.editor.rootShaped = false

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
        var paletteGroup = createPaletteGroup(lk.layers.editor.environment.content, ef)
        editorBox.setChild(paletteGroup, rect, cursorCoords, lk.layers.editor.environment.placement.top)
        paletteGroup.x = 2
        editorBox.color = "black"
        editorBox.border.width = 1
        editorBox.border.color = "#141c25"

        var paletteBox = openPalette(palette, paletteGroup)
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

        var position = palettes.declaration.position
        palettes.data = filterOutPalettes(palettes.data)

        if ( !palettes.data || palettes.data.length === 0){
            return
        }

        if ( palettes.data.length === 1 ){
            loadPalette(editor, palettes.data, 0, position)
        } else {
            //Palette list box
            editor.internalFocus = false

            var paletteList = views.openPaletteListBox(
                theme.selectableListView,
                palettes.data,
                {
                    aroundRect: editor.getCursorRectangle(),
                    panePosition: editor.mapGlobalPosition(),
                    relativePlacement: lk.layers.editor.environment.placement.bottom
                },
                {
                    onCancelled: function(paletteListBox){
                        editor.editor.forceFocus()
                        paletteListBox.destroy()
                    },
                    onSelected: function(index, paletteListBox){
                        editor.editor.forceFocus()
                        paletteListBox.destroy()
                        loadPalette(editor, palettes.data, index, position)
                    }
                }
            )
            lk.layers.workspace.panes.setActiveItem(paletteList, editor)
        }
    }

    function shape(editor){
        var codeHandler = editor.documentHandler.codeHandler
        var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition)

        var position = palettes.declaration.position

        if ( !palettes.data || palettes.data.length === 0 ){
            shapePalette(editor, "", position)
        } else if ( palettes.length === 1 ){
            shapePalette(editor, palettes.data[0].name, position)
        } else {
            //Palette list box
            editor.internalFocus = false

            var paletteList = views.openPaletteListBox(
                theme.selectableListView,
                palettes.data,
                {
                    aroundRect: editor.getCursorRectangle(),
                    panePosition: editor.mapGlobalPosition(),
                    relativePlacement: lk.layers.editor.environment.placement.bottom
                },
                {
                    onCancelled: function(paletteListBox){
                        editor.editor.forceFocus()
                        paletteListBox.destroy()
                    },
                    onSelected: function(index, paletteListBox){
                        editor.editor.forceFocus()
                        paletteListBox.destroy()
                        shapePalette(editor, palettes.data[index].name, position)
                    }
                }
            )
            lk.layers.workspace.panes.setActiveItem(paletteList, editor)
        }
    }

    function bind(editor){
        var codeHandler = editor.documentHandler.codeHandler

        var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition)
        var position = palettes.declaration.position
        palettes = filterOutPalettes(palettes)

        if ( palettes.data.length === 1 ){
            var ef = codeHandler.openConnection(position)
            ef.incrementRefCount()
            codeHandler.openBinding(ef, palettes.data[0].name)
        } else {
            editor.internalFocus = false

            var paletteList = views.openPaletteListBox(
                theme.selectableListView,
                palettes.data,
                {
                    aroundRect: editor.getCursorRectangle(),
                    panePosition: editor.mapGlobalPosition(),
                    relativePlacement: lk.layers.editor.environment.placement.bottom
                },
                {
                    onCancelled: function(paletteListBox){
                        editor.editor.forceFocus()
                        paletteListBox.destroy()
                    },
                    onSelected: function(index, paletteListBox){
                        editor.editor.forceFocus()
                        paletteListBox.destroy()
                        var ef = codeHandler.openConnection(position)
                        ef.incrementRefCount()
                        codeHandler.openBinding(ef, palettes.data[index].name)
                    }
                }
            )
            lk.layers.workspace.panes.setActiveItem(paletteList, editor)
        }
    }

    function shapePalette(editor, paletteName, position){
        var codeHandler = editor.documentHandler.codeHandler
        var ef = codeHandler.openConnection(position)

        if (!ef){
            lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't shape palette"
            throw linkError(new Error('Failed to find editing fragment for palette at: ' + position, 300))
        }

        var forAnObject = ef.location === QmlEditFragment.Object
        var editorBox = ef.visualParent ? ef.visualParent.parent : null

        var objectContainer = null
        var paletteBoxGroup = editorBox ? editorBox.child : null
        if ( paletteBoxGroup === null ){
            if (forAnObject){
                objectContainer = createObjectContainerForFragment(ef)
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
                editorBox = createEditorBoxForFragment(ef, true)
                paletteBoxGroup = ef.visualParent
            }
        } else {
            var p = paletteBoxGroup
            while (p && p.objectName !== "objectContainer") p = p.parent
            objectContainer = p
        }


        var palette = paletteName !== ""
                ? editor.documentHandler.codeHandler.expand(ef, {
                    "palettes" : [paletteName]
                })
                : null
        var forImports = ef.location === QmlEditFragment.Imports

        if (palette){
            if (forImports){
                palette.item.model = codeHandler.importsModel()
                palette.item.editor = editor.editor
            }
            if (forAnObject && !palette.item ){
                objectContainer.expandOptions(palette)
            }

            var paletteBox = openPalette(palette, paletteBoxGroup)
            if (paletteBox) paletteBox.moveEnabledSet = false
        }

        var frameBoxPosition = ef.position()
        if (forImports){
            frameBoxPosition = codeHandler.checkPragma(ef.position())
        }
        editor.documentHandler.frameBox(editorBox, frameBoxPosition, ef.length() + ef.position() - frameBoxPosition)

        if (forImports) editor.editor.importsShaped = true
        ef.incrementRefCount()

        return objectContainer ? objectContainer : palette
    }

    function loadPalette(editor, palettes, index, position){
        var codeHandler = editor.documentHandler.codeHandler

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        var ef = codeHandler.openConnection(position)

        if (!ef)
        {
            lk.layers.workspace.messages.pushError("Error: Can't create a palette in a non-compiled program", 1)
            return
        }
        ef.incrementRefCount()

        var palette = editor.documentHandler.codeHandler.expand(ef, {
            "palettes" : [palettes[index].name]
        })

        var editorBox = ef.visualParent ? ef.visualParent.parent : null
        var paletteBoxGroup = editorBox ? editorBox.child : null

        if ( paletteBoxGroup === null ){
            editorBox = createEditorBoxForFragment(ef, false)
            editorBox.color = "black"
            paletteBoxGroup = ef.visualParent
        }

        var paletteBox = openPalette(palette, paletteBoxGroup)

        editorBox.updatePlacement(rect, cursorCoords, lk.layers.editor.environment.placement.top)
    }

    function convertEditorStateIntoInstructions(editor){
        var codeHandler = editor.documentHandler.codeHandler

        var result = null
        var editingFragments = codeHandler.editingFragments()
        for ( var i = 0; i < editingFragments.length; ++i ){
            if (editingFragments[i].location === QmlEditFragment.Object){
                result = convertObjectIntoInstructions(editingFragments[i])
            }
        }

        if (editor.importsShaped)
            result['shapeImports'] = true

        return result
    }

    function convertStateIntoInstructions(){
        var editorPane = lk.layers.workspace.panes.focusPane('editor')
        if ( !editorPane )
            return

        var editor = editorPane.editor
        return convertEditorStateIntoInstructions(editor)
    }

    function convertObjectIntoInstructions(object, container){
        var result = ({})
        result['type'] = object.type()
        var paletteList = object.paletteList()
        var nameList = []
        for (var idx = 0; idx < paletteList.length; ++idx)
            nameList.push(paletteList[idx].name)
        if (nameList.length > 0)
            result['palettes'] = nameList

        var objectContainer = container
        if (!container){
            var p = object.visualParent
            while (p && p.objectName !== "objectContainer")
                p = p.parent
            if (p) objectContainer = p
        }
        if (objectContainer && objectContainer.compact) return result
        var childFragments = object.getChildFragments()
        var objects = []
        var properties = []
        for (var i = 0; i < childFragments.length; ++i){
            var frag = childFragments[i]
            if (frag.location === QmlEditFragment.Object)
                objects.push(convertObjectIntoInstructions(frag))
            if (frag.location === QmlEditFragment.Property)
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
