import QtQuick 2.3
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceExtension{
    id: root

    property WorkspaceTheme currentTheme: lk.layers.workspace.themes.current

    globals : ProjectQmlExtension{
        property PaletteControls paletteControls: PaletteControls{}

        function add(activeIndex, objectsOnly, forRoot){
            root.add(activeIndex, objectsOnly, forRoot)
        }

        function shapeLayout(editorPane, layout){
            var codeHandler = editorPane.editor.code.language
            var rootPosition = codeHandler.findRootPosition()
            root.rootPosition = rootPosition
            shapeRootObject(editorPane, codeHandler, function(){
                lk.layers.workspace.extensions.editqml.paletteControls.shapeAtPositionWithInstructions(
                    editorPane,
                    rootPosition,
                    layout
                )
            })
        }

        function shapeAll(editor, callback){
            var codeHandler = editor.code.language

            if (editor.loading){
                editor.stopLoadingMode()
                return
            }

            paletteControls.shapeImports(editor)
            var rootPosition = codeHandler.findRootPosition()

            if ( rootPosition >= 0){
                paletteControls.shapeRoot(editor, callback)
            } else {
                editor.editor.addRootButton.visible = true
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
        globals.paletteControls.openEditPaletteAtPosition(activePane, userPosition)
    }

    function palette(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteControls.userOpenPaletteAtPosition(activePane, userPosition)
    }

    function shape(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteControls.userShapePaletteAtPosition(activePane, userPosition)
    }

    function shapeAll(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        globals.shapeAll(activePane)
    }

    function bind(){
        var activePane = getActiveQmlPane()
        if ( !activePane )
            return

        var userPosition = activePane.textEdit.cursorPosition
        globals.paletteControls.userBind(activePane, userPosition)
    }

    function unbind(){
        var activePane = lk.layers.workspace.panes.activePane
        if ( !activePane )
            return

        var editor = activePane
        var codeHandler = editor.code.language

        codeHandler.closeBinding(
            editor.textEdit.selectionStart,
            editor.textEdit.selectionEnd - editor.textEdit.selectionStart
        )
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

        if (activePane.objectName !== 'editor' ||
            !activePane.document ||
            !canBeQml(activePane.document) )
            return

        var addContainer = activePane.code.language.getAddOptions(activePane.textEdit.cursorPosition, LanguageQmlHandler.NoReadOnly)
        if ( !addContainer )
            return

        var rect = activePane.getCursorRectangle()
        var paneCoords = activePane.mapGlobalPosition()

        var categories = objectsOnly ? ['objects'] : ['objects', 'properties', 'events']

        var addEditorBox = globals.paletteControls.views.openAddOptionsBox(
            addContainer,
            activePane.code.language,
            {
                aroundRect: rect,
                panePosition: Qt.point(paneCoords.x, paneCoords.y),
                relativePlacement: lk.layers.editor.environment.placement.bottom
            },
            {
                categories: categories,
                onCancelled: function(box){
                    box.child.finalize()
                },
                onAccepted: function(box, selection){
                    if ( selection.category === 'property' ){
                        activePane.code.language.addProperty(
                            selection.position, selection.objectType, selection.type, selection.name, true
                        )
                    } else if ( selection.category === 'object' ){
                        if (forRoot){
                            var position = activePane.code.language.addRootObjectToCode(selection.name)
                            if (position === -1){
                                lk.layers.workspace.messages.pushError("Error: Can't create object with name " + selection.name, 1)
                            } else {
                                root.rootPosition = position
                                shapeRootObject(activePane, activePane.code.language)
                            }
                        }
                        else
                            activePane.code.language.addItem(
                                selection.position, selection.objectType, selection.name
                            )
                    } else if ( selection.category === 'event' ){
                        activePane.code.language.addEvent(
                            selection.position, selection.objectType, selection.type, selection.name
                        )
                    }
                    box.child.finalize()
                },
                onFinalized: function(box){
                    box.child.destroy()
                    box.destroy()
                }
            }
        )

        addEditorBox.child.activeIndex = activeIndex ? activeIndex : 0
        lk.layers.workspace.panes.setActiveItem(addEditorBox, activePane)

    }

    function objectContainerAdd(){
        var activePane = lk.layers.workspace.panes.activePane
        var activeItem = lk.layers.workspace.panes.activeItem
        if ( activePane.paneType === 'editor' && activeItem.objectName === 'objectContainerFrame' ){
            lk.layers.workspace.extensions.editqml.paletteControls.userAddToObjectContainer(activeItem)
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
