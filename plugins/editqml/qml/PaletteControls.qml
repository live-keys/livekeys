import QtQuick 2.0
import editqml 1.0
import editor 1.0

QtObject{

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

    function expandDefaultPalette(editingFragment, editor, paletteBoxParent, objectRoot){
        var defaultPaletteName = editor.documentHandler.codeHandler.defaultPalette(editingFragment)
        if ( defaultPaletteName.length ){
            var paletteControls = lk.layers.workspace.extensions.editqml.paletteControls
            paletteControls.expandPalette(defaultPaletteName,
                                          editingFragment,
                                          editor,
                                          paletteBoxParent,
                                          objectRoot)
        }
    }

    function expandPalette(paletteName, editingFragment, editor, paletteBoxParent, objectRoot){

        if ( !editingFragment ) return
        var palette = editor.documentHandler.codeHandler.expand(editingFragment, {
            "palettes" : [paletteName]
        })

        var paletteBox = addPalette(palette,
                                    editingFragment,
                                    editor,
                                    paletteBoxParent,
                                    objectRoot)
        if (paletteBox) paletteBox.moveEnabledSet = false

    }

    function addPalette(palette, editingFragment, editor, paletteBoxParent, objectRoot){
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

            newPaletteBox.documentHandler = editor.documentHandler
            newPaletteBox.cursorRectangle = editor.getCursorRectangle()
            newPaletteBox.editorPosition = editor.cursorWindowCoords()

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
        ef.visualParent = paletteBoxGroup
        childObjectContainer.paletteGroup = paletteBoxGroup

        paletteBoxGroup.x = 5
        paletteBoxGroup.y = 10

        if (propertyContainer)
            propertyContainer.valueContainer = childObjectContainer
        if ( propPalette ){
            expandPalette(propPalette,
                          ef,
                          childObjectContainer.editor,
                          paletteBoxGroup,
                          childObjectContainer)
        } else if (expandDefault) {
            expandDefaultPalette(ef,
                                 childObjectContainer.editor,
                                 childObjectContainer.paletteGroup,
                                 childObjectContainer)
        }

        if (propertyContainer)
            propertyContainer.paletteAddButtonVisible = false

        return childObjectContainer
    }

    function compose(container, isForNode){
        var codeHandler = container.editor.documentHandler.codeHandler

        var position =
            container.editingFragment.valuePosition() +
            container.editingFragment.valueLength() - 1

        var addContainer = codeHandler.getAddOptions(position)
        if ( !addContainer )
            return

        var addBoxItem = createAddQmlBox()
        addBoxItem.addContainer = addContainer
        addBoxItem.codeQmlHandler = codeHandler
        addBoxItem.isForNode = isForNode

        addBoxItem.assignFocus()

        var oct = container.parent

        // capture y
        var octit = container
        var octY = 0
        while ( octit && octit.objectName !== 'editorBox' ){
            octY += octit.y
            octit = octit.parent
        }
        if ( octit.objectName === 'editorBox' ){
            octY += octit.y
        }

        var rect = Qt.rect(oct.x + 150, octY, oct.width, 25)
        var cursorCoords = container.editor.cursorWindowCoords()

        var addBox = lk.layers.editor.environment.createEditorBox(
            addBoxItem, rect, cursorCoords, lk.layers.editor.environment.placement.bottom
        )

        addBox.color = 'transparent'
        addBoxItem.cancel = function(){
            addBoxItem.destroy()
            addBox.destroy()
        }

        addBoxItem.accept = function(type, data){
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

                var ppos = codeHandler.addProperty(
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
                }

                if (!ef) {
                    lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
                    console.error("Error: Can't create a palette in a non-compiled program")
                }


            } else if ( addBoxItem.activeIndex === 2 ){ // object

                var opos = codeHandler.addItem(
                            addContainer.model.addPosition,
                            addContainer.objectType,
                            data)

                codeHandler.addItemToRuntime(container.editingFragment, data, project.appRoot())

                if (!isForNode && container.compact) container.expand()

                var ef = codeHandler.openNestedConnection(
                    container.editingFragment, opos, project.appRoot()
                )

                if (ef){
                    container.editingFragment.signalObjectAdded(ef)
                    if (!isForNode && container.compact) container.sortChildren()
                }


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

        if ( codeHandler.isForAnObject(ef) && !expandDefault){
            addChildObjectContainer(objectContainer, ef, true, propertyContainer, propPalette)
        } else {
            propertyContainer.valueContainer = createPaletteGroup()
            ef.visualParent = propertyContainer.valueContainer

            propertyContainer.valueContainer.editingFragment = ef
            propertyContainer.valueContainer.codeHandler = objectContainer.editor.documentHandler.codeHandler

            if ( propPalette ){
                expandPalette(propPalette,
                              ef,
                              objectContainer.editor,
                              propertyContainer.valueContainer)

            } else if (expandDefault){
                expandDefaultPalette(ef,
                                     objectContainer.editor,
                                     propertyContainer.valueContainer)
            }
        }
        ef.visualParent.owner = propertyContainer

        objectContainer.propertiesOpened.push(ef.identifier())
        ef.incrementRefCount()
    }

    function expandAtPosition(editor, position, instructions){
        var codeHandler = editor.documentHandler.codeHandler

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()
        var ef = codeHandler.openConnection(position)

        if (!ef) return

        ef.incrementRefCount()
        var editorBox = lk.layers.editor.environment.createEmptyEditorBox(editor.textEdit)

        // create blank object container for editing fragment
        var objectContainer = createObjectContainer(lk.layers.editor.environment.content)
        objectContainer.editor = editor
        objectContainer.editingFragment = ef
        objectContainer.title = ef.typeName()

        var paletteBoxGroup = createPaletteGroup(objectContainer.groupsContainer)
        paletteBoxGroup.editingFragment = ef
        ef.visualParent = paletteBoxGroup

        paletteBoxGroup.codeHandler = codeHandler

        objectContainer.paletteGroup = paletteBoxGroup;

        editorBox.setChild(objectContainer, rect, cursorCoords, lk.layers.editor.environment.placement.top)

        editorBox.color = globals.paletteStyle.backgroundColor
        editorBox.border.width = 1
        editorBox.border.color = "#141c25"

        codeHandler.frameEdit(editorBox, ef)
        // finished with

        var rootPos = codeHandler.findRootPosition()
        if (ef.position() === rootPos)
            editor.editor.rootShaped = true


        expand(objectContainer, editor, instructions)
    }

    function expand(objectContainer, editor, instructions){

        if (instructions['type'] !== objectContainer.editingFragment.typeName()) return
        objectContainer.compact = false

        var containers = openBlankChildContainers(objectContainer, editor)

        if ('palettes' in instructions){
            var palettes = instructions['palettes']
            for (var i = 0; i < palettes.length; ++i){
                var palette = palettes[i]
                expandPalette(palettes[i],
                              objectContainer.editingFragment,
                              editor,
                              objectContainer.groupsContainer.children[0],
                              objectContainer)
            }
        }

        if ('children' in instructions){
            var children = instructions['children']
            if (children.length === containers.length){
                for (var i = 0; i < containers.length; ++i)
                    expand(containers[i], editor, children[i])
            }
        }

        if ('properties' in instructions){
            var properties = instructions['properties']
            for (var i = 0; i < properties.length; ++i){
                var property = properties[i]
                openProperty(objectContainer, property)
            }
        }

    }

    function openProperty(objectContainer, property){

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
                expand(childObjectContainer, childObjectContainer.editor, property['instructions'])
        }

        if ('palettes' in property){
            var palettes = property['palettes']

            for (var i = 0; i < palettes.length; ++i)
            {
                var palette = palettes[i]
                expandPalette(palette,
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
        var position = objectContainer.editingFragment.valuePosition() +
                       objectContainer.editingFragment.valueLength() - 1


        var addContainer = codeHandler.getAddOptions(position)
        if ( !addContainer )
            return

        addContainer.model.setCategoryFilter(1)
        addContainer.model.setFilter(name)

        if (addContainer.model.rowCount() !== 1) return

        var type = addContainer.model.data(addContainer.model.index(0, 0), 256 + 3/*QmlSuggestionModel.Type*/)

        var ppos = codeHandler.addProperty(
            addContainer.model.addPosition, addContainer.objectType, type, name, true
        )

        var ef = codeHandler.openNestedConnection(
            objectContainer.editingFragment, ppos, project.appRoot()
        )

        if (ef) {
            objectContainer.editingFragment.signalPropertyAdded(ef, false)
        }

        return ef
    }

    function openBlankChildContainers(objectContainer){
        var objects = objectContainer.editor.documentHandler.codeHandler.openNestedObjects(objectContainer.editingFragment)

        var containers = []
        for (var i=0; i < objects.length; ++i){

            var childObjectContainer =
                addChildObjectContainer(objectContainer, objects[i], false)

            childObjectContainer.x = 20
            childObjectContainer.y = 10

            containers.push(childObjectContainer)
            objects[i].incrementRefCount()
        }

        return containers
    }

    function openDefaults(objectContainer){
        expandDefaultPalette(objectContainer.editingFragment,
                             objectContainer.editor,
                             objectContainer.paletteGroup,
                             objectContainer)

        var properties = editor.documentHandler.codeHandler.openNestedProperties(editingFragment)

        for (var i=0; i < properties.length; ++i){
            objectContainer.addPropertyFragmentToContainer(properties[i])
        }
    }
}
