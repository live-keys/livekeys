import QtQuick 2.3
import editor 1.0
import editqml 1.0

LiveExtension{
    id: root

    globals : ProjectQmlExtension{}
    interceptLanguage : function(document, handler, ext){
        var extLower = ext.toLowerCase()

        if ( extLower === 'js' || extLower === 'qml'  ||
            (document.file.name.length > 2 && document.file.name.substring(0, 2) === "T:" ) )
        {
            return globals.createHandler(document, handler)
        }
        return null
    }

    objectName : "editqml"

    property Component addBoxFactory: Component{ AddQmlBox{} }
    property Component paletteGroupFactory: Component{ PaletteGroup{} }
    property Component objectContainerFactory: Component{ ObjectContainer{} }
    property Component paletteContainerFactory: Component{ PaletteContainer{} }
    property Component paletteListFactory : Component{ PaletteListView{} }

    function canBeQml(document){
        if ( endsWith(document.file.path, '.qml') ||
            (document.file.name.length > 2 && document.file.name.substring(0, 2) === "T:" ))
        {
            return true
        }
        return false
    }

    function endsWith(str, suffix){
        return str.indexOf(suffix, str.length - suffix.length) !== -1;
    }

    function edit(){
        var activePane = lk.layers.workspace.panes.activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) )
        {
            var editor = activePane
            var codeHandler = editor.documentHandler.codeHandler

            var rect = editor.editor.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()

            var ef = codeHandler.openConnection(editor.textEdit.cursorPosition, project.appRoot())
            var palette = codeHandler.edit(ef)

            var editorBox = lk.layers.editor.environment.createEmptyEditorBox()
            var paletteGroup = root.paletteGroupFactory.createObject(lk.layers.editor.environment.content)
            editorBox.setChild(paletteGroup, rect, cursorCoords, lk.layers.editor.environment.placement.top)
            paletteGroup.x = 5
            paletteGroup.editingFragment = ef
            paletteGroup.codeHandler = codeHandler
            ef.visualParent = paletteGroup
            editorBox.color = "black"
            editorBox.border.width = 1
            editorBox.border.color = "#141c25"

            var paletteBox = root.paletteContainerFactory.createObject(paletteGroup)

            palette.item.x = 5
            palette.item.y = 7

            paletteBox.child = palette.item
            paletteBox.palette = palette

            paletteBox.title = 'Edit'
            paletteBox.titleLeftMargin = 10
            paletteBox.paletteSwapVisible = false
            paletteBox.paletteAddVisible = false
            paletteBox.documentHandler = editor.documentHandler
            paletteBox.cursorRectangle = rect
            paletteBox.editorPosition = cursorCoords
            paletteBox.paletteContainerFactory = function(arg){ return root.paletteContainerFactory.createObject(arg) }

            editorBox.updatePlacement(rect, cursorCoords, lk.layers.editor.environment.placement.top)
        }
    }

    function shapePalette(editor, palettes, index){
        var codeHandler = editor.documentHandler.codeHandler

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        var ef = codeHandler.openConnection(palettes.position(), project.appRoot())
        var palette = palettes.size() > 0 ? codeHandler.openPalette(ef, palettes, index) : null
        if (codeHandler.isInImports(palettes.position()))
        {
            palette.item.model = codeHandler.importsModel()
        }

        if (!ef)
        {
            lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't shape palette of a non-running program"
            console.error("Error: Can't shape palette of a non-running program")
            return
        }

        var editorBox = ef.visualParent ? ef.visualParent.parent : null
        var paletteBoxGroup = editorBox ? editorBox.child : null

        if ( paletteBoxGroup === null ){
            editorBox = lk.layers.editor.environment.createEmptyEditorBox(editor.textEdit)
            var forAnObject = codeHandler.isForAnObject(ef)
            var objectContainer = null

            if (forAnObject)
            {
                objectContainer = root.objectContainerFactory.createObject(lk.layers.editor.environment.content)
                objectContainer.editor = editor
                objectContainer.editingFragment = ef
            }

            paletteBoxGroup = root.paletteGroupFactory.createObject(forAnObject ? objectContainer.groupsContainer : lk.layers.editor.environment.content)
            paletteBoxGroup.editingFragment = ef
            ef.visualParent = paletteBoxGroup

            paletteBoxGroup.codeHandler = codeHandler

            if (forAnObject)
                objectContainer.paletteGroup = paletteBoxGroup;

            editorBox.setChild(forAnObject ? objectContainer : paletteBoxGroup, rect, cursorCoords, lk.layers.editor.environment.placement.top)

            editorBox.color = "black"
            editorBox.border.width = 1
            editorBox.border.color = "#141c25"


            if (forAnObject)
                objectContainer.expand()
        }

        if ( palette || !codeHandler.isForAnObject(ef) ){
            var paletteBox = root.paletteContainerFactory.createObject(paletteBoxGroup)

            palette.item.x = 5
            palette.item.y = 7

            paletteBox.child = palette.item
            paletteBox.palette = palette

            paletteBox.name = palette.name
            paletteBox.type = palette.type
            paletteBox.moveEnabledSet = false
            paletteBox.documentHandler = editor.documentHandler
            paletteBox.cursorRectangle = rect
            paletteBox.editorPosition = cursorCoords
            paletteBox.paletteContainerFactory = function(arg){ return root.paletteContainerFactory.createObject(arg) }
        }

        codeHandler.frameEdit(editorBox, ef)
    }

    function loadPalette(editor, palettes, index){
        var codeHandler = editor.documentHandler.codeHandler

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        var ef = codeHandler.openConnection(palettes.position(), project.appRoot())

        if (!ef)
        {
            lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
            console.error("Error: Can't create a palette in a non-compiled program")
            return
        }

        var palette = codeHandler.openPalette(ef, palettes, index)

        var editorBox = ef.visualParent ? ef.visualParent.parent : null
        var paletteBoxGroup = editorBox ? editorBox.child : null

        if ( paletteBoxGroup === null ){
            editorBox = lk.layers.editor.environment.createEmptyEditorBox()
            paletteBoxGroup = root.paletteGroupFactory.createObject(lk.layers.editor.environment.content)
            editorBox.setChild(paletteBoxGroup, rect, cursorCoords, lk.layers.editor.environment.placement.top)
            paletteBoxGroup.editingFragment = ef
            paletteBoxGroup.codeHandler = codeHandler
            ef.visualParent = paletteBoxGroup
            editorBox.color = "black"
            editorBox.border.width = 1
            editorBox.border.color = "#141c25"
        }

        var paletteBox = root.paletteContainerFactory.createObject(paletteBoxGroup)

        palette.item.x = 5
        palette.item.y = 7

        paletteBox.child = palette.item
        paletteBox.palette = palette

        paletteBox.name = palette.name
        paletteBox.type = palette.type
        paletteBox.documentHandler = editor.documentHandler
        paletteBox.cursorRectangle = rect
        paletteBox.editorPosition = cursorCoords
        paletteBox.paletteContainerFactory = function(arg){ return root.paletteContainerFactory.createObject(arg) }

        editorBox.updatePlacement(rect, cursorCoords, lk.layers.editor.environment.placement.top)
    }

    function palette(){
        var activePane = lk.layers.workspace.panes.activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) )
        {
            var editor = activePane
            var codeHandler = editor.documentHandler.codeHandler

            var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition, true)
            var rect = editor.editor.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()
            if ( palettes.size() === 1 ){
                root.loadPalette(editor, palettes, 0)
            } else {
                //Palette list box
                var palList      = paletteListFactory.createObject()
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
                    palListBox.destroy()
                })
                palList.paletteSelected.connect(function(index){
                    palList.focus = false
                    editor.editor.forceFocus()
                    palListBox.destroy()
                    root.loadPalette(editor, palettes, index)
                })
            }
        }
    }

    function shape(){
        var activePane = lk.layers.workspace.panes.activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) )
        {
            var editor = activePane
            var codeHandler = editor.documentHandler.codeHandler

            var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition, true)
            var rect = editor.editor.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()

            if ( !palettes || palettes.size() === 0 ){
                root.shapePalette(editor, palettes, -1)

            } else if ( palettes.size() === 1 ){
                root.shapePalette(editor, palettes, 0)
            } else {
                //Palette list box
                var palList      = paletteListFactory.createObject()
                var palListBox   = lk.layers.editor.environment.createEditorBox(palList, rect, cursorCoords, lk.layers.editor.environment.placement.bottom)
                palListBox.color = 'transparent'
                palList.model    = palettes
                editor.internalFocus = false
                palList.forceActiveFocus()
                lk.layers.workspace.panes.setActiveItem(palList, editor)

                palList.cancelled.connect(function(){
                    palList.focus = false
                    editor.editor.forceFocus()
                    palListBox.destroy()
                })
                palList.paletteSelected.connect(function(index){
                    palList.focus = false
                    editor.editor.forceFocus()
                    palListBox.destroy()
                    root.shapePalette(editor, palettes, index)
                })
            }
        }
    }

    function shapeAll(){
        var activePane = lk.layers.workspace.panes.activePane
        if ( activePane.objectName !== 'editor' ||
             !activePane.document ||
             !canBeQml(activePane.document) ) return

        var editor = activePane
        var codeHandler = editor.documentHandler.codeHandler

        var imports = codeHandler.importsModel()

        var importsPosition = codeHandler.findImportsPosition(imports.firstBlock())
        var rootPosition = codeHandler.findRootPosition(imports.lastBlock())

        var paletteImports = codeHandler.findPalettes(importsPosition, true)
        if (paletteImports) root.shapePalette(editor, paletteImports, 0)

        var paletteRoot = codeHandler.findPalettes(rootPosition, true)
        if (paletteRoot) root.shapePalette(editor, paletteRoot, 0)
    }

    function addProperty(){
        add(0)
    }

    function addObject(){
        add(1)
    }

    function addEvent(){
        add(2)
    }

    function add(activeIndex){
        var activePane = lk.layers.workspace.panes.activePane
        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) )
        {
            var addContainer = activePane.documentHandler.codeHandler.getAddOptions(activePane.textEdit.cursorPosition)
            if ( !addContainer )
                return

            var rect = activePane.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()
            var addBoxItem = addBoxFactory.createObject()
            addBoxItem.addContainer = addContainer
            addBoxItem.codeQmlHandler = activePane.documentHandler.codeHandler

            addBoxItem.activeIndex = activeIndex ? activeIndex : 0

            var addBox = lk.layers.editor.environment.createEditorBox(
                addBoxItem, rect, cursorCoords, lk.layers.editor.environment.placement.bottom
            )
            addBox.color = 'transparent'
            addBoxItem.cancel = function(){
                addBox.destroy()
            }
            addBoxItem.accept = function(type, data){
                if ( addBoxItem.activeIndex === 0 ){
                    activePane.documentHandler.codeHandler.addProperty(
                        addContainer.propertyModel.addPosition, addContainer.objectType, type, data
                    )
                } else if ( addBoxItem.activeIndex === 1 ){
                    activePane.documentHandler.codeHandler.addItem(
                        addContainer.itemModel.addPosition, addContainer.objectType, data
                    )
                } else if ( addBoxItem.activeIndex === 2 ){
                    activePane.documentHandler.codeHandler.addEvent(
                        addContainer.itemModel.addPosition, addContainer.objectType, type, data
                    )
                }
                addBox.destroy()
            }

            addBoxItem.assignFocus()
            lk.layers.workspace.panes.setActiveItem(addBox, activePane)
        }
    }

    function bind(){
        var activePane = lk.layers.workspace.panes.activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) )
        {
            var editor = activePane
            var codeHandler = editor.documentHandler.codeHandler

            var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition, false)
            var rect = editor.editor.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()
            if ( palettes.size() === 1 ){
                var ef = codeHandler.openConnection(palettes.position(), project.appRoot())
                codeHandler.openBinding(ef, palettes, 0)
            } else {
                var palList      = paletteListFactory.createObject()
                var palListBox   = lk.layers.editor.environment.createEditorBox(palList, rect, cursorCoords, lk.layers.editor.environment.placement.bottom)
                palListBox.color = 'transparent'
                palList.model = palettes
                editor.internalFocus = false
                palList.forceActiveFocus()
                lk.layers.workspace.panes.setActiveItem(palList, editor)

                palList.cancelled.connect(function(){
                    palList.focus = false
                    editor.editor.forceFocus()
                    palListBox.destroy()
                })
                palList.paletteSelected.connect(function(index){
                    palList.focus = false
                    editor.editor.forceFocus()
                    palListBox.destroy()

                    var ef = codeHandler.openConnection(palettes.position(), project.appRoot())
                    codeHandler.openBinding(ef, palettes, index)
                })
            }
        }
    }

    function unbind(){
        var activePane = lk.layers.workspace.panes.activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) )
        {
            var editor = activePane
            var codeHandler = editor.documentHandler.codeHandler

            codeHandler.closeBinding(
                editor.textEdit.selectionStart,
                editor.textEdit.selectionEnd - editor.textEdit.selectionStart
            )
        }
    }

    commands : {
        "edit" :  [edit, "Edit Property Under Cursor"],
        "palette" : [palette, "Pallete for Property Under Cursor"],
        "add" : [add, "Add a Property/Object"],
        "add_property" : [addProperty, "Add a property."],
        "add_object" : [addObject, "Add an object."],
        "add_event" : [addEvent, "Add an event."],
        'bind' : [bind, "Bind to Property Under Cursor"],
        'unbind' : [unbind, "Unbind Properties Under Cursor"],
        'shape' : [shape, "Shape This Property Into a Palette"],
        'shape_all' : [shapeAll, "Shape all the code"]
    }

    keyBindings : {
        "alt+a" : "editqml.palette",
        "alt+z" : "editqml.add",
        "alt+x" : "editqml.shape"
    }

    interceptMenu : function(item){
        if ( item.objectName === 'editorType' && item.document ){

            if ( canBeQml(item.document) ){

                var codeHandler = item.documentHandler.codeHandler
                var cursorInfo = codeHandler.cursorInfo(
                    item.textEdit.selectionStart, item.textEdit.selectionEnd - item.textEdit.selectionStart
                );

                return [
                    {
                        name : "Edit",
                        action : root.commands['edit'][0],
                        enabled : cursorInfo.canEdit
                    }, {
                        name : "Palette",
                        action : root.commands['palette'][0],
                        enabled : cursorInfo.canAdjust
                    }, {
                        name : "Shape",
                        action : root.commands['shape'][0],
                        enabled : cursorInfo.canShape
                    }, {
                        name : "Bind",
                        action : root.commands['bind'][0],
                        enabled : cursorInfo.canBind
                    }, {
                        name : "Unbind",
                        action : root.commands['unbind'][0],
                        enabled : cursorInfo.canUnbind
                    }, {
                        name : "Add Property",
                        action : root.commands['add_property'][0],
                        enabled : true
                    }, {
                        name : "Add Object",
                        action : root.commands['add_object'][0],
                        enabled : true
                    }, {
                        name : "Add Event",
                        action : root.commands['add_event'][0],
                        enabled : true
                    }, {
                        name: "Shape all",
                        action: root.commands['shape_all'][0],
                        enabled: true
                    }
                ]
            }
        }
        return null
    }
}
