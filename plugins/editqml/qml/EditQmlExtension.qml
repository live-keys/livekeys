import QtQuick 2.3
import editor 1.0
import editqml 1.0

LiveExtension{
    id: root

    globals : ProjectQmlExtension{}
    interceptLanguage : function(document, handler, ext){
        var extLower = ext.toLowerCase()
        if ( extLower === 'js' || extLower === 'qml' || document.file.path === '' ){
            return globals.createHandler(document, handler)
        }
        return null;
    }

    objectName : "editqml"

    property Component addBoxFactory: Component{ AddQmlBox{} }

    function endsWith(str, suffix){
        return str.indexOf(suffix, str.length - suffix.length) !== -1;
    }

    function edit(){
        var activePane = livecv.windowControls().activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             ( endsWith(activePane.document.file.path, 'qml') || activePane.document.file.path === '' ) )
        {
            var editor = activePane
            var codeHandler = editor.documentHandler.codeHandler
            var windowControls = livecv.windowControls()

            var palette = codeHandler.edit(editor.textEdit.cursorPosition, editor.windowControls.runSpace.item)
            var rect = editor.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()

            var editorBox = windowControls.editSpace.createEmptyEditorBox()
            codeHandler.addPaletteBox(editorBox)
            var paletteBoxContainer = windowControls.editSpace.createPaletteBoxContainer()
            editorBox.setChild(paletteBoxContainer, rect, cursorCoords, windowControls.editSpace.placement.top)
            paletteBoxContainer.x = 5
            editorBox.color = "#02070b"
            editorBox.radius = 5
            editorBox.border.width = 1
            editorBox.border.color = "#061b24"

            var paletteBox = windowControls.editSpace.createPaletteBox(paletteBoxContainer)

            palette.item.x = 5
            palette.item.y = 7

            paletteBox.child = palette.item
            paletteBox.title = 'Edit'
            paletteBox.titleLeftMargin = 10
            paletteBox.paletteSwapVisible = false
            paletteBox.paletteAddVisible = false
            paletteBox.documentHandler = editor.documentHandler
            paletteBox.cursorRectangle = rect
            paletteBox.editorPosition = cursorCoords

            editorBox.updatePlacement(rect, cursorCoords, windowControls.editSpace.placement.top)
        }
    }

    function palette(){
        var activePane = livecv.windowControls().activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             ( endsWith(activePane.document.file.path, 'qml') || activePane.document.file.path === '' ) )
        {
            var editor = activePane
            var codeHandler = editor.documentHandler.codeHandler
            var windowControls = livecv.windowControls()

            var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition, true)
            var rect = editor.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()
            if ( palettes.size() === 1 ){
                var editorBox = codeHandler.paletteBoxAtPosition(palettes.position())
                var paletteBoxContainer = editorBox ? editorBox.child : null

                var palette = codeHandler.openPalette(palettes, 0, editor.windowControls.runSpace.item)

                if ( paletteBoxContainer === null ){
                    editorBox = windowControls.editSpace.createEmptyEditorBox()
                    codeHandler.addPaletteBox(editorBox)
                    paletteBoxContainer = windowControls.editSpace.createPaletteBoxContainer()
                    editorBox.setChild(paletteBoxContainer, rect, cursorCoords, windowControls.editSpace.placement.top)
                    paletteBoxContainer.x = 5
                    editorBox.color = "#02070b"
                    editorBox.radius = 5
                    editorBox.border.width = 1
                    editorBox.border.color = "#061b24"
                }

                var paletteBox = windowControls.editSpace.createPaletteBox(paletteBoxContainer)

                palette.item.x = 5
                palette.item.y = 7

                paletteBox.child = palette.item
                paletteBox.name = palette.name
                paletteBox.type = palette.type
                paletteBox.documentHandler = editor.documentHandler
                paletteBox.cursorRectangle = rect
                paletteBox.editorPosition = cursorCoords

                editorBox.updatePlacement(rect, cursorCoords, windowControls.editSpace.placement.top)

            } else {
                //Palette list box
                var palList      = windowControls.editSpace.createPaletteList()
                var palListBox   = windowControls.editSpace.createEditorBox(palList, rect, cursorCoords, windowControls.editSpace.placement.bottom)
                palListBox.color = 'transparent'
                palList.model    = palettes
                editor.internalFocus = false
                palList.forceActiveFocus()
                windowControls.setActiveItem(palList, editor)

                palList.cancelled.connect(function(){
                    palList.focus = false
                    editor.forceFocus()
                    palListBox.destroy()
                })
                palList.paletteSelected.connect(function(index){
                    palList.focus = false
                    editor.forceFocus()
                    palListBox.destroy()

                    var editorBox = codeHandler.paletteBoxAtPosition(palettes.position())
                    var paletteBoxContainer = editorBox ? editorBox.child : null

                    var palette = codeHandler.openPalette(palettes, index, editor.windowControls.runSpace.item)

                    if ( paletteBoxContainer === null ){
                        editorBox = windowControls.editSpace.createEmptyEditorBox()
                        codeHandler.addPaletteBox(editorBox)
                        paletteBoxContainer = windowControls.editSpace.createPaletteBoxContainer()
                        editorBox.setChild(paletteBoxContainer, rect, cursorCoords, windowControls.editSpace.placement.top)
                        paletteBoxContainer.x = 5
                        editorBox.color = "#02070b"
                        editorBox.radius = 5
                        editorBox.border.width = 1
                        editorBox.border.color = "#061b24"
                    }

                    var paletteBox = windowControls.editSpace.createPaletteBox(paletteBoxContainer)

                    palette.item.x = 5
                    palette.item.y = 7

                    paletteBox.child = palette.item

                    paletteBox.name = palette.name
                    paletteBox.type = palette.type
                    paletteBox.documentHandler = editor.documentHandler
                    paletteBox.cursorRectangle = rect
                    paletteBox.editorPosition = cursorCoords

                    editorBox.updatePlacement(rect, cursorCoords, windowControls.editSpace.placement.top)
                })
            }
        }
    }

    function shape(){
        var activePane = livecv.windowControls().activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             ( endsWith(activePane.document.file.path, 'qml') || activePane.document.file.path === '' ) )
        {
            var editor = activePane
            var codeHandler = editor.documentHandler.codeHandler
            var windowControls = livecv.windowControls()

            var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition, true)
            var rect = editor.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()
            if ( palettes.size() === 1 ){
                var editorBox = codeHandler.paletteBoxAtPosition(palettes.position())
                var paletteBoxContainer = editorBox ? editorBox.child : null

                var palette = codeHandler.openPalette(palettes, 0, editor.windowControls.runSpace.item)

                if ( paletteBoxContainer === null ){
                    editorBox = windowControls.editSpace.createEmptyEditorBox()
                    codeHandler.addPaletteBox(editorBox)
                    paletteBoxContainer = windowControls.editSpace.createPaletteBoxContainer()
                    editorBox.setChild(paletteBoxContainer, rect, cursorCoords, windowControls.editSpace.placement.top)
                    paletteBoxContainer.x = 5
                    editorBox.color = "#02070b"
                    editorBox.radius = 5
                    editorBox.border.width = 1
                    editorBox.border.color = "#061b24"
                }

                var paletteBox = windowControls.editSpace.createPaletteBox(paletteBoxContainer)

                palette.item.x = 5
                palette.item.y = 7

                paletteBox.child = palette.item
                paletteBox.name = palette.name
                paletteBox.type = palette.type
                paletteBox.documentHandler = editor.documentHandler
                paletteBox.cursorRectangle = rect
                paletteBox.editorPosition = cursorCoords

                editorBox.updatePlacement(rect, cursorCoords, windowControls.editSpace.placement.top)

            } else {
                //Palette list box
                var palList      = windowControls.editSpace.createPaletteList()
                var palListBox   = windowControls.editSpace.createEditorBox(palList, rect, cursorCoords, windowControls.editSpace.placement.bottom)
                palListBox.color = 'transparent'
                palList.model    = palettes
                editor.internalFocus = false
                palList.forceActiveFocus()
                windowControls.setActiveItem(palList, editor)

                palList.cancelled.connect(function(){
                    palList.focus = false
                    editor.forceFocus()
                    palListBox.destroy()
                })
                palList.paletteSelected.connect(function(index){
                    palList.focus = false
                    editor.forceFocus()
                    palListBox.destroy()

                    var editorBox = codeHandler.paletteBoxAtPosition(palettes.position())
                    var paletteBoxContainer = editorBox ? editorBox.child : null

                    var palette = codeHandler.openPalette(palettes, index, editor.windowControls.runSpace.item)

                    if ( paletteBoxContainer === null ){
                        editorBox = windowControls.editSpace.createEmptyEditorBox()
                        codeHandler.addPaletteBox(editorBox)
                        paletteBoxContainer = windowControls.editSpace.createPaletteBoxContainer()
                        editorBox.setChild(paletteBoxContainer, rect, cursorCoords, windowControls.editSpace.placement.top)
                        paletteBoxContainer.x = 5
                        editorBox.color = "#02070b"
                        editorBox.radius = 5
                        editorBox.border.width = 1
                        editorBox.border.color = "#061b24"
                    }

                    var paletteBox = windowControls.editSpace.createPaletteBox(paletteBoxContainer)

                    palette.item.x = 5
                    palette.item.y = 7

                    paletteBox.child = palette.item

                    paletteBox.name = palette.name
                    paletteBox.type = palette.type
                    paletteBox.documentHandler = editor.documentHandler
                    paletteBox.cursorRectangle = rect
                    paletteBox.editorPosition = cursorCoords

                    codeHandler.framePalette(editorBox, palette)

//                    editorBox.updatePlacement(rect, cursorCoords, windowControls.editSpace.placement.top)
                })
            }
        }
    }

    function add(){
        var activePane = livecv.windowControls().activePane
        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             ( activePane.document.file.path.endsWith('.qml') || activePane.document.file.path === '' ) )
        {
            var addContainer = activePane.documentHandler.codeHandler.getAddOptions(activePane.textEdit.cursorPosition)
            if ( !addContainer )
                return

            var rect = activePane.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()
            var addBoxItem = addBoxFactory.createObject()
            addBoxItem.addContainer = addContainer

            var addBox = livecv.windowControls().editSpace.createEditorBox(
                addBoxItem, rect, cursorCoords, livecv.windowControls().editSpace.placement.bottom
            )
            addBox.color = 'transparent'
            addBoxItem.cancel = function(){
                addBox.destroy()
            }
            addBoxItem.accept = function(data){
                if ( addBoxItem.activeIndex === 0 ){
                    activePane.documentHandler.codeHandler.addProperty(addContainer.propertyModel.addPosition, data)
                } else {
                    activePane.documentHandler.codeHandler.addItem(addContainer.itemModel.addPosition, data)
                }
                addBox.destroy()
            }

            addBoxItem.assignFocus()
            livecv.windowControls().setActiveItem(addBox, activePane)
        }
    }

    function bind(){
        var activePane = livecv.windowControls().activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             ( endsWith(activePane.document.file.path, 'qml') || activePane.document.file.path === '' ) )
        {
            var editor = activePane
            var codeHandler = editor.documentHandler.codeHandler
            var windowControls = livecv.windowControls()

            var palettes = codeHandler.findPalettes(editor.textEdit.cursorPosition, false)
            var rect = editor.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()
            if ( palettes.size() === 1 ){
                codeHandler.openBinding(palettes, 0, editor.windowControls.runSpace.item)
            } else {
                var palList      = windowControls.editSpace.createPaletteList()
                var palListBox   = windowControls.editSpace.createEditorBox(palList, rect, cursorCoords, windowControls.editSpace.placement.bottom)
                palListBox.color = 'transparent'
                palList.model    = palettes
                editor.internalFocus = false
                palList.forceActiveFocus()
                windowControls.setActiveItem(palList, editor)

                palList.cancelled.connect(function(){
                    palList.focus = false
                    editor.forceFocus()
                    palListBox.destroy()
                })
                palList.paletteSelected.connect(function(index){
                    palList.focus = false
                    editor.forceFocus()
                    palListBox.destroy()

                    codeHandler.openBinding(palettes, index, editor.windowControls.runSpace.item)
                })
            }
        }
    }

    function unbind(){
        var activePane = livecv.windowControls().activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             ( endsWith(activePane.document.file.path, 'qml') || activePane.document.file.path === '' ) )
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
        "edit" : edit,
        "palette" : palette,
        "add" : add,
        'bind' : bind,
        'unbind' : unbind,
        'shape' : shape
    }

    keyBindings : {
        "alt+a" : "editqml.palette",
        "alt+z" : "editqml.add"
    }

    interceptMenu : function(item){
        if ( item.objectName === 'editor' && item.document ){
            if ( item.document.file.path === '' || endsWith(item.document.file.path, '.qml') ){

                var codeHandler = item.documentHandler.codeHandler
                var cursorInfo = codeHandler.cursorInfo(
                    item.textEdit.selectionStart, item.textEdit.selectionEnd - item.textEdit.selectionStart
                );

                return [
                    {
                        name : "Edit",
                        action : function(){ root.commands['edit']()},
                        enabled : cursorInfo.canEdit
                    }, {
                        name : "Palette",
                        action : function(){ root.commands['palette']()},
                        enabled : cursorInfo.canAdjust
                    }, {
                        name : "Shape",
                        action : function(){ root.commands['shape']()},
                        enabled : cursorInfo.canAdjust
                    }, {
                        name : "Bind",
                        action : function(){ root.commands['bind']() },
                        enabled : cursorInfo.canBind
                    }, {
                        name : "Unbind",
                        action : function(){ root.commands['unbind']() },
                        enabled : cursorInfo.canUnbind
                    }, {
                        name : "Add",
                        action : function(){
                            root.commands['add']()
                        },
                        enabled : true
                    }
                ]
            }
        }
        return null
    }

}
