import QtQuick 2.3
import editor 1.0
import editqml 1.0

WorkspaceExtension{
    id: root

    property WorkspaceTheme currentTheme: lk.layers.workspace.themes.current

    globals : ProjectQmlExtension{
        property PaletteControls paletteControls: PaletteControls{}

        function add(activeIndex, objectsOnly, forRoot){
            root.add(activeIndex, objectsOnly, forRoot)
        }

        function shapeRootObject(editor, codeHandler, callback){
            root.shapeRootObject(editor, codeHandler, callback)
        }


        function shapeAllInEditor(editor){
            var codeHandler = editor.documentHandler.codeHandler

            if (editor.loading){
                editor.stopLoadingMode()
                rootPosition = -1
                return
            }

            shapeImports(editor, codeHandler)
            rootPosition = codeHandler.findRootPosition()

            if ( rootPosition >= 0){
                shapeRootObject(editor, codeHandler)
            } else {
                editor.editor.addRootButton.visible = true
            }
        }


        function shapeImports(editor, codeHandler){
            root.shapeImports(editor, codeHandler)
        }
        property alias rootPosition: root.rootPosition
    }
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

    property int rootPosition: -1

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
            globals.paletteControls.edit(activePane)
        }
    }

    function palette(){
        var activePane = lk.layers.workspace.panes.activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) )
        {
            globals.paletteControls.palette(activePane)
        }
    }

    function shape(){
        var activePane = lk.layers.workspace.panes.activePane

        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) )
        {
            globals.paletteControls.shape(activePane)
        }
    }

    function shapeImports(editor, codeHandler){
        var importsPosition = codeHandler.findImportsPosition()
        var paletteImports = codeHandler.findPalettes(importsPosition)
        if (paletteImports) {
            var pc = globals.paletteControls.shapePalette(editor, paletteImports, 0)
            pc.item.width = Qt.binding(function(){
                if (!pc.item.parent || !pc.item.parent.parent) return
                var editorSize = editor.width - editor.editor.lineSurfaceWidth - 50 - pc.item.parent.parent.headerWidth
                return editorSize > 280 ? editorSize : 280
            })
        }
    }

    function shapeRootObject(editor, codeHandler, callback){

        editor.startLoadingMode()

        codeHandler.removeSyncImportsListeners()
        codeHandler.onImportsScanned(function(){

            editor.stopLoadingMode()

            if (rootPosition === -1){
                return
            }

            var paletteRoot = codeHandler.findPalettes(rootPosition)
            if (paletteRoot){
                if ( paletteRoot ){
                    if (callback)
                        callback()
                    else {
                        var oc = globals.paletteControls.shapePalette(editor, paletteRoot, 0)
                        oc.contentWidth = Qt.binding(function(){
                            return oc.containerContentWidth > oc.editorContentWidth ? oc.containerContentWidth : oc.editorContentWidth
                        })

                        editor.editor.rootShaped = true
                    }
                }
            } else {
                throw linkError(new Error("Failed to shape root object."), this)
            }
        })

    }

    function shapeAll(){
        var activePane = lk.layers.workspace.panes.activePane
        if ( activePane.objectName !== 'editor' ||
             !activePane.document ||
             !canBeQml(activePane.document) ) return

        var editor = activePane
        var codeHandler = editor.documentHandler.codeHandler

        if (editor.loading){
            editor.stopLoadingMode()
            rootPosition = -1
            return
        }

        shapeImports(editor, codeHandler)
        rootPosition = codeHandler.findRootPosition()

        if ( rootPosition >= 0){
            shapeRootObject(editor, codeHandler)
        } else {
            editor.editor.addRootButton.visible = true
        }
    }

    property Component shapeAllTrigger: Component {
        Connections {
            id: shapeTrigger
            target: null
            property var editor: null
            property var callback: null
            ignoreUnknownSignals: true
            onImportsScanned: {
                if (rootPosition === -1){
                    editor.stopLoadingMode()
                    return
                }
                var codeHandler = editor.documentHandler.codeHandler
                var paletteRoot = codeHandler.findPalettes(rootPosition)
                if (paletteRoot){
                    if (!callback){
                        var oc = globals.paletteControls.shapePalette(editor, paletteRoot, 0)
                        oc.contentWidth = Qt.binding(function(){
                            return oc.containerContentWidth > oc.editorContentWidth ? oc.containerContentWidth : oc.editorContentWidth
                        })
                        editor.editor.rootShaped = true
                    } else {
                        callback()
                    }
                    rootPosition = -1

                    shapeTrigger.destroy()
                } else {
                    throw linkError(new Error("Failed to shape root object. Unknown type."), this)
                }

                editor.stopLoadingMode()
            }
        }
    }

    function addProperty(){
        add(1)
    }

    function addObject(){
        add(2)
    }

    function addEvent(){
        add(3)
    }

    function add(activeIndex, objectsOnly, forRoot){
        var activePane = lk.layers.workspace.panes.activePane
        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) )
        {
            var addContainer = activePane.documentHandler.codeHandler.getAddOptions(activePane.textEdit.cursorPosition, CodeQmlHandler.NoReadOnly)
            if ( !addContainer )
                return

            var rect = activePane.getCursorRectangle()
            var cursorCoords = activePane.cursorWindowCoords()
            var addBoxItem = globals.paletteControls.createAddQmlBox(null)
            if (!addBoxItem) return

            addBoxItem.assignFocus()
            addBoxItem.addContainer = addContainer
            addBoxItem.codeQmlHandler = activePane.documentHandler.codeHandler

            addBoxItem.activeIndex = activeIndex ? activeIndex : 0
            if (objectsOnly)
                addBoxItem.mode = AddQmlBox.DisplayMode.ObjectsOnly


            var addBox = lk.layers.editor.environment.createEditorBox(
                addBoxItem, rect, cursorCoords, lk.layers.editor.environment.placement.bottom
            )
            addBox.color = 'transparent'
            addBoxItem.cancel = function(){
                addBoxItem.destroy()
                addBox.destroy()
            }
            addBoxItem.accept = function(type, data){
                if ( addBoxItem.activeIndex === 1 ){
                    activePane.documentHandler.codeHandler.addProperty(
                        addContainer.model.addPosition, addContainer.objectType, type, data, true
                    )
                } else if ( addBoxItem.activeIndex === 2 ){
                    if (forRoot){
                        var position = activePane.documentHandler.codeHandler.insertRootItem(data)
                        if (position === -1){
                            lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create object with name " + data
                            console.error("Error: Can't create object with name " + data)
                        } else {
                            root.rootPosition = position
                            shapeRootObject(activePane, activePane.documentHandler.codeHandler)
                        }
                    }
                    else
                        activePane.documentHandler.codeHandler.addItem(
                            addContainer.model.addPosition, addContainer.objectType, data
                        )
                } else if ( addBoxItem.activeIndex === 3 ){
                    activePane.documentHandler.codeHandler.addEvent(
                        addContainer.model.addPosition, addContainer.objectType, type, data
                    )
                }
                addBoxItem.destroy()
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
            globals.paletteControls.bind(activePane)
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

    function objectContainerAdd(){
        var activePane = lk.layers.workspace.panes.activePane
        var activeItem = lk.layers.workspace.panes.activeItem
        if ( activePane.paneType === 'editor' && activeItem.objectName === 'objectContainerFrame' ){
            lk.layers.workspace.extensions.editqml.paletteControls.compose(activeItem, false)
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
        'shape_all' : [shapeAll, "Shape all the code"],
        'object_container_add' : [objectContainerAdd, "EditQml - Object Container: Open add menu."]
    }

    keyBindings : {
        "alt+a" : "editqml.palette",
        "alt+z" : "editqml.add",
        "alt+x" : "editqml.shape",
        "alt+s" : { command: "editqml.object_container_add", whenPane: "editor", whenItem: "objectContainerFrame" }
    }

    menuInterceptors : [
        {
            whenPane: 'editor',
            whenItem: 'editorType',
            intercept: function(pane, item){

                if ( item.document ){

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
    ]
}
