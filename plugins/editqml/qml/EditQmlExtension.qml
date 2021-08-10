import QtQuick 2.3
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceExtension{
    id: root

    property WorkspaceTheme currentTheme: lk.layers.workspace.themes.current

    globals : ProjectQmlExtension{
        property PaletteFunctions paletteFunctions: PaletteFunctions{}

        function shapeAll(editor, callback){
            var codeHandler = editor.code.language

            if (lk.layers.editor.environment.hasLoadingScreen(editor)){
                lk.layers.editor.environment.removeLoadingScreen(editor)
                return
            }

            lk.layers.editor.environment.addLoadingScreen(editor)
            paletteFunctions.shapeImports(editor)
            var rootPosition = codeHandler.findRootPosition()

            if ( rootPosition >= 0){
                paletteFunctions.shapeRoot(editor, function(ef, palette){
                    lk.layers.editor.environment.removeLoadingScreen(editor)
                    if ( callback )
                        callback(ef, palette)
                })
            } else {
                paletteControls.__private.createAddRootButton(editor)
                if ( callback )
                    callback(null, null)
            }
        }
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

    function getActiveQmlPane(){
        var activePane = lk.layers.workspace.panes.activePane
        if ( activePane.objectName === 'editor' &&
             activePane.document &&
             canBeQml(activePane.document) ){
            return activePane
        }
        return null
    }

    function edit(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteFunctions.openEditPaletteAtPosition(activePane.editor, userPosition)
    }

    function palette(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteFunctions.userOpenPaletteAtPosition(activePane.editor, userPosition)
    }

    function shape(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteFunctions.userShapePaletteAtPosition(activePane.editor, userPosition)
    }

    function shapeAll(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        globals.shapeAll(activePane.editor)
    }

    function bind(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteFunctions.userBind(activePane.editor, userPosition)
    }

    function unbind(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        var editor = activePane
        var codeHandler = editor.code.language

        codeHandler.closeBinding(
            editor.textEdit.selectionStart,
            editor.textEdit.selectionEnd - editor.textEdit.selectionStart
        )
    }

    function add(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteFunctions.userAddCodeToPosition(activePane.editor, userPosition)
    }

    function addProperty(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return
        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteFunctions.userAddCodeToPosition(activePane.editor, userPosition, ['properties'])
    }

    function addObject(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return
        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteFunctions.userAddCodeToPosition(activePane.editor, userPosition, ['objects'])
    }

    function addEvent(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return
        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteFunctions.userAddCodeToPosition(activePane.editor, userPosition, ['events'])
    }

    function objectContainerAdd(){
        var activePane = lk.layers.workspace.panes.activePane
        var activeItem = lk.layers.workspace.panes.activeItem
        if ( activePane.paneType === 'editor' && activeItem.objectName === 'objectContainerFrame' ){
            lk.layers.workspace.extensions.editqml.paletteFunctions.userAddToObjectContainer(activeItem)
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

                        var codeHandler = item.code.language
                        var declarationInfo = codeHandler.declarationInfo(
                            item.textEdit.selectionStart, item.textEdit.selectionEnd - item.textEdit.selectionStart
                        );

                        return [
                            {
                                name : "Edit",
                                action : root.commands['edit'][0],
                                enabled : declarationInfo ? true : false
                            }, {
                                name : "Palette",
                                action : root.commands['palette'][0],
                                enabled : declarationInfo ? true : false
                            }, {
                                name : "Shape",
                                action : root.commands['shape'][0],
                                enabled : declarationInfo ? true : false
                            }, {
                                name : "Bind",
                                action : root.commands['bind'][0],
                                enabled : declarationInfo ? true : false
                            }, {
                                name : "Unbind",
                                action : root.commands['unbind'][0],
                                enabled : declarationInfo ? true : false
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
