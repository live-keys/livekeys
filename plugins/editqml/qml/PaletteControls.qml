import QtQuick 2.0
import editqml 1.0
import editor 1.0

QtObject{

    property QtObject factories : QtObject{

        property Component addQmlBox: Component{ AddQmlBox{} }
        property Component paletteGroup: Component{ PaletteGroup{} }
        property Component objectContainer: Component{ ObjectContainer{} }
        property Component paletteContainer: Component{ PaletteContainer{} }
        property Component paletteListView: Component{
            PaletteListView{
                visible: model ? true : false
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

    function createPaletteListView(parent){
        return factories.paletteListView.createObject(parent)
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
            newPaletteBox.paletteContainerFactory = function(arg){
                return createPaletteContainer(arg)
            }

            if (objectRoot && objectRoot.compact) objectRoot.expand()

            return newPaletteBox
        }

        if (objectRoot) objectRoot.expandOptions(palette)
    }

    function addChildObjectContainer(parentObjectContainer, ef, propertyContainer, propPalette){
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
        } else {
            expandDefaultPalette(ef,
                                 childObjectContainer.editor,
                                 childObjectContainer.paletteGroup,
                                 childObjectContainer)
        }

        if (propertyContainer)
            propertyContainer.paletteAddButtonVisible = false

        return childObjectContainer
    }
}
