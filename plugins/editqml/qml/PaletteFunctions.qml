import QtQuick 2.0
import editqml 1.0 as EditQml
import editor 1.0
import workspace.nodeeditor 1.0
import visual.input 1.0 as Input

QtObject{
    id: root

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

    property QtObject theme: lk.layers.workspace.themes.current
    property string defaultPalette: '-'

    property QtObject __factories : QtObject{

        property Component addQmlBox: Component{ AddQmlBox{} }
        property Component addExtraPropertiesBox: Component{ AddExtraPropertiesBox{} }
        property Component paletteGroup: Component{ PaletteGroup{} }
        property Component objectContainer: Component{ ObjectContainer{} }
        property Component paletteContainer: Component{ PaletteContainer{} }
        property Component propertyContainer: Component{ PropertyContainer{} }
        property Component paletteListView: Component{
            PaletteListView{
                visible: model ? true : false
                onFocusChanged : if ( !focus ){
                    cancelled()
                }

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

        property Component addRootButton: Component{
            Input.TextButton{
                text: "Add Root"
                objectName: "addRootButton"
                width: 120
                height: 30
                property var whenClicked: null
                onClicked: {
                    if ( whenClicked )
                        whenClicked()
                }
            }
        }

        function createAddQmlBox(parent){
            return __factories.addQmlBox.createObject(parent)
        }

        function createPaletteGroup(parent, ef){
            var pg = __factories.paletteGroup.createObject(parent)
            pg.__initialize(ef)
            return pg
        }

        function createPaletteContainer(palette, paletteBoxParent, options){
            if ( !palette.item )
                return null

            var newPaletteBox = __factories.paletteContainer.createObject(paletteBoxParent)
            palette.item.x = 2
            palette.item.y = 2

            newPaletteBox.palette = palette

            if ( paletteBoxParent.palettesOpened )
                paletteBoxParent.palettesOpened.push(palette.name)

            if ( options && options.hasOwnProperty('moveEnabled') ){
                newPaletteBox.moveEnabledSet = options.moveEnabled
            }

            return newPaletteBox
        }

        function createObjectContainer(editor, ef, parent){
            var oc = __factories.objectContainer.createObject(parent)
            oc.__initialize(editor, ef)
            return oc
        }

        function createPaletteListView(parent, style){
            var plv = __factories.paletteListView.createObject(parent)
            if (style) plv.style = style
            return plv
        }

        function createPropertyContainer(editor, ef, parent){
            var pc = __factories.propertyContainer.createObject(parent)
            pc.__initialize(editor, ef)
            return pc
        }
    }

    property QtObject __private: QtObject{

        function createEditorPaletteBoxForFragment(ef, parent){
            var languageHandler = ef.language
            var editor = languageHandler.code.textEdit().getEditor()
            var editorBox = lk.layers.editor.environment.createEmptyEditorBox(parent)
            var paletteBoxGroup = __factories.createPaletteGroup(lk.layers.editor.environment.content, ef)

            var rect = editor.getCursorRectangle()
            var cursorCoords = editor.cursorWindowCoords()

            editorBox.setChild(paletteBoxGroup, rect, cursorCoords, lk.layers.editor.environment.placement.top)
            editorBox.color = theme.colorScheme.middleground
            editorBox.border.width = 1
            editorBox.border.color = "#141c25"

            return editorBox
        }

        function createEditorObjectContainerBoxForFragment(ef, parent){
            var editor = ef.language.code.textEdit().getEditor()
            var languageHandler = editor.code.language

            var editorBox = lk.layers.editor.environment.createEmptyEditorBox(parent)
            var objectContainer = __factories.createObjectContainer(editor, ef, lk.layers.editor.environment.content)

            var rect = editor.getCursorRectangle()
            var cursorCoords = editor.cursorWindowCoords()

            editorBox.setChild(objectContainer, rect, cursorCoords, lk.layers.editor.environment.placement.top)

            editorBox.color = theme.colorScheme.middleground
            editorBox.border.width = 1
            editorBox.border.color = "#141c25"

            var rootDeclaration = languageHandler.rootDeclaration()
            if (ef.position() === rootDeclaration.position()){
                languageHandler.rootFragment = ef
            }
            return objectContainer
        }

        function createAddRootButton(editor){
            for ( var i = 0; i < editor.children.length; ++i ){
                if ( editor.children[i].objectName === 'addRootButton' )
                    return
            }
            var addRoot =  __factories.addRootButton.createObject(editor)
            addRoot.x = editor.lineSurfaceWidth + 20
            addRoot.y = editor.textEdit.totalHeight - 10
            addRoot.style = root.theme.formButtonStyle
            addRoot.whenClicked = function(){
                root.userAddCodeToPosition(
                    editor,
                    editor.textEdit.text.length,
                    ['objects'], {
                        onAccepted: function(selection, position){
                            editor.code.language.createRootObjectInRuntime({ type: selection.name, id: selection.id })
                            root.shapeRoot(editor)
                            addRoot.destroy()
                        }
                    }
                )
            }

            return addRoot
        }

        function removeAddRootButton(editor){
            for ( var i = 0; i < editor.children.length; ++i ){
                if ( editor.children[i].objectName === 'addRootButton' )
                    editor.children[i].destroy()
            }
        }
    }

    property QtObject views: QtObject{

        function openPaletteList(style, model, parent, handlers){
            var paletteList = __factories.createPaletteListView(parent, style)
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

        function openPaletetListBoxForContainer(container, paletteGroup, aroundBox, mode, swap){
            var palettes = container.control.editFragment.language.findPalettes(container.control.editFragment)
            palettes = filterOutPalettes(
                palettes,
                paletteGroup.palettesOpened,
                mode === PaletteFunctions.PaletteListMode.ObjectContainer
            )

            if (!palettes || palettes.length === 0)
                return null

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
                var palette = container.control.editor.code.language.expand(container.control.editFragment, {
                    "palettes" : [palettes[index].name]
                })

                var objectRoot = mode === PaletteFunctions.PaletteListMode.ObjectContainer
                               ? container
                               : (container.objectName === "objectNode" ? container : null)
                var paletteBox = __factories.createPaletteContainer(palette, paletteGroup)
                if ( container ){
                    if ( palette.item ){ // send expand signal
                        if ( container.control.compact )
                            container.control.expand()
                    } else {
                        expandObjectContainerLayout(container, palette, {expandChildren: false})
                    }
                }

                if (paletteBox){
                    if (mode === PaletteFunctions.PaletteListMode.ObjectContainer){
                        paletteBox.moveEnabledSet = false
                    } else if (mode === PaletteFunctions.PaletteListMode.PaletteContainer){
                        paletteBox.moveEnabledSet = container.moveEnabledGet
                    }
                }

                if (swap === PaletteFunctions.PaletteListSwap.Swap){

                    var p = container.parent
                    while (p && p.objectName !== "paletteGroup"){
                        p = p.parent
                    }
                    p.palettesOpened = p.palettesOpened.filter(function(name){ return name !== container.palette.name })
                    container.parent = null
                    container.code.language.removePalette(container.palette)
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
                palettes,
                {
                    aroundRect: aroundBox,
                    panePosition: Qt.point(paneCoords.x, paneCoords.y - 35),
                    relativePlacement: lk.layers.editor.environment.placement.bottom
                },
                handlers
            )

            return paletteList
        }

        function openAddOptionsBox(addContainer, languageHandler, editorBoxParams, handlers){
            var addBoxItem = __factories.createAddQmlBox(null)
            addBoxItem.addContainer = addContainer
            addBoxItem.languageQmlHandler = languageHandler

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
            var boxItem = __factories.addExtraPropertiesBox.createObject(null)
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

    // Public

    function userOpenPaletteAtPosition(editor, userPosition, callback){
        try{
            var languageHandler = editor.code.language
            var declaration = languageHandler.findDeclaration(userPosition)
            var palettes = languageHandler.findPalettes(declaration)
            if ( !palettes )
                return

            var position = declaration.position()
            palettes = filterOutPalettes(palettes)
            if ( !palettes || palettes.length === 0){
                return
            }

            if ( palettes.length === 1 ){
                openPaletteAtPosition(editor, palettes[0].name, position)
            } else {
                //Palette list box
                editor.internalFocus = false

                var paletteList = views.openPaletteListBox(
                    theme.selectableListView,
                    palettes,
                    {
                        aroundRect: editor.getCursorRectangle(),
                        panePosition: editor.getGlobalPosition(),
                        relativePlacement: lk.layers.editor.environment.placement.bottom
                    },
                    {
                        onCancelled: function(paletteListBox){
                            editor.forceFocus()
                            paletteListBox.destroy()
                            if ( callback )
                                callback(null)
                        },
                        onSelected: function(index, paletteListBox){
                            editor.forceFocus()
                            paletteListBox.destroy()
                            var newPalette = openPaletteAtPosition(editor, palettes[index].name, position, callback)
                        }
                    }
                )
                lk.layers.workspace.panes.setActiveItem(paletteList, editor)
            }
        } catch (e){
            lk.layers.workspace.messages.pushErrorObject(e)
        }
    }

    function openPaletteAtPosition(editor, paletteName, position, callback){
        try{
            var languageHandler = editor.code.language
            var declaration = languageHandler.findDeclaration(position)
            if ( !declaration ){
                throw new Error("QmlLanguage: Failed to find declaration at: " + position)
            }

            var ef = languageHandler.openConnection(declaration)
            if (!ef){
                throw new Error("QmLLanguage: Failed to create connection at: " + position)
            }
            ef.incrementRefCount()
            var palette = editor.code.language.expand(ef, {
                "palettes" : [paletteName]
            })

            var editorBox = ef.visualParent ? ef.visualParent.parent : null
            var paletteBoxGroup = editorBox ? editorBox.child : null

            if ( paletteBoxGroup === null ){
                editorBox = __private.createEditorPaletteBoxForFragment(ef, false)
                editorBox.color = "black"
                paletteBoxGroup = ef.visualParent
            }

            var paletteBox = __factories.createPaletteContainer(palette, paletteBoxGroup)

            var rect = editor.getCursorRectangle()
            var cursorCoords = editor.cursorWindowCoords()

            paletteBoxGroup.measureSizeInfo()

            // Update new placement so as to animate
            editorBox.updatePlacement(rect, cursorCoords, lk.layers.editor.environment.placement.top)

            if ( callback )
                callback(ef, palette)
        } catch ( e ){
            lk.layers.workspace.messages.pushErrorObject(e)
        }
    }

    function openEditPaletteAtPosition(editor, position, callback){
        try{
            var languageHandler = editor.code.language

            var rect = editor.getCursorRectangle()
            var cursorCoords = editor.cursorWindowCoords()

            var declaration = languageHandler.findDeclaration(editor.textEdit.cursorPosition)
            if ( !declaration ){
                throw new Error("QmlLanguage: Failed to find declaration at: " + editor.textEdit.cursorPosition)
            }

            var ef = languageHandler.openConnection(declaration)
            if (!ef){
                throw new Error("QmLLanguage: Failed to create connection at: " + position)
            }
            var palette = languageHandler.edit(ef)

            var editorBox = lk.layers.editor.environment.createEmptyEditorBox()
            var paletteGroup = __factories.createPaletteGroup(lk.layers.editor.environment.content, ef)
            editorBox.setChild(paletteGroup, rect, cursorCoords, lk.layers.editor.environment.placement.top)
            paletteGroup.x = 2
            editorBox.color = "black"
            editorBox.border.width = 1
            editorBox.border.color = "#141c25"

            var paletteBox = __factories.createPaletteContainer(palette, paletteGroup)
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

            if ( callback )
                callback(ef, palette)
        } catch ( e ){
            lk.layers.workspace.messages.pushErrorObject(e)
        }
    }

    function userShapePaletteAtPosition(editor, userPosition, callback){
        var languageHandler = editor.code.language
        var declaration = languageHandler.findDeclaration(userPosition)
        if ( !declaration )
            return
        var palettes = languageHandler.findPalettes(declaration)
        if ( !palettes )
            return

        var position = declaration.position()

        if ( !palettes || palettes.length === 0 ){
            shapePaletteAtPosition(editor, "", position, function(ef, palette){
                if ( ef.visualParent.expand )
                    ef.visualParent.expand()
                if ( callback )
                    callback(ef, palette)
            })
        } else if ( palettes.length === 1 ){
            shapePaletteAtPosition(editor, palettes[0].name, position, function(ef, palette){
                if ( ef.visualParent.expand )
                    ef.visualParent.expand()
                if ( callback )
                    callback(ef, palette)
            })
        } else {
            //Palette list box
            editor.internalFocus = false

            var paletteList = views.openPaletteListBox(
                theme.selectableListView,
                palettes,
                {
                    aroundRect: editor.getCursorRectangle(),
                    panePosition: editor.getGlobalPosition(),
                    relativePlacement: lk.layers.editor.environment.placement.bottom
                },
                {
                    onCancelled: function(paletteListBox){
                        editor.forceFocus()
                        paletteListBox.destroy()
                    },
                    onSelected: function(index, paletteListBox){
                        editor.forceFocus()
                        paletteListBox.destroy()
                        shapePaletteAtPosition(editor, palettes[index].name, position, function(ef, palette){
                            if ( ef.visualParent.expand )
                                ef.visualParent.expand()
                            if ( callback )
                                callback(ef, palette)
                        })
                    }
                }
            )
            lk.layers.workspace.panes.setActiveItem(paletteList, editor)
        }
    }

    function shapePaletteAtPosition(editor, paletteName, position, callback){
        try{
            var languageHandler = editor.code.language

            var declaration = languageHandler.findDeclaration(position)
            if ( !declaration ){
                throw new Error(lang("QmlLanguage: Failed to find declaration at: %0").format(position))
            }
            var ef = languageHandler.openConnection(declaration)
            if (!ef){
                throw new Error(lang("QmlLanguage: Failed to create connection at: %0").format(position))
            }

            var forAnObject = ef.location === EditQml.QmlEditFragment.Object
            var editorBox = ef.visualParent ? ef.visualParent.parent : null

            var objectContainer = null
            var paletteBoxGroup = editorBox ? editorBox.child : null

            if ( paletteBoxGroup === null ){
                if (forAnObject){
                    objectContainer = __private.createEditorObjectContainerBoxForFragment(ef, editor.textEdit)
                    objectContainer.adjustSizeToEditor()

                    var rootDeclaration = languageHandler.rootDeclaration()

                    paletteBoxGroup = objectContainer.control.paletteGroup
                    editorBox = objectContainer.parent
                } else {
                    editorBox = __private.createEditorPaletteBoxForFragment(ef, editor.textEdit)
                    paletteBoxGroup = ef.visualParent
                    paletteBoxGroup.adjustSizeToEditor()
                }
            } else {
                var p = paletteBoxGroup
                while (p && p.objectName !== "objectContainer")
                    p = p.parent
                objectContainer = p
            }

            var palette = null
            if ( paletteName !== "" ){
                palette = languageHandler.expand(ef, {
                    "palettes" : [paletteName]
                })
            }

            var forImports = ef.location === EditQml.QmlEditFragment.Imports

            if (palette){
                if (forAnObject && !palette.item ){
                    expandObjectContainerLayout(objectContainer, palette, {expandChildren: false})
                }
                var paletteBox = __factories.createPaletteContainer(palette, paletteBoxGroup, {moveEnabled: false})
            }

            var frameBoxPosition = ef.position()
            if (forImports){
                frameBoxPosition = languageHandler.checkPragma(ef.position())
            }
            editor.code.frameBox(editorBox, frameBoxPosition, ef.length() + ef.position() - frameBoxPosition)

            if (forImports)
                editor.code.language.importsFragment = ef //TODO: Move to LanguageQmlHandler

            ef.incrementRefCount()
            if ( callback )
                callback(ef, palette)
        } catch ( e ){
            lk.layers.workspace.messages.pushErrorObject(e)
        }
    }

    function shapeImports(editor, callback){
        var languageHandler = editor.code.language

        var declaration = languageHandler.findImportsDeclaration()
        var paletteImports = languageHandler.findPalettes(declaration)
        if (paletteImports) {
            var position = declaration.position()
            paletteImports = filterOutPalettes(paletteImports)

            shapePaletteAtPosition(editor, paletteImports[0].name, position, function(ef, palette){
                if ( callback ){
                    callback(ef, palette)
                }
            })
        }
    }

    function shapeRoot(editor, callback){
        var languageHandler = editor.code.language
        languageHandler.removeSyncImportsListeners()
        languageHandler.onImportsScanned(function(){
            var rootDeclaration = languageHandler.rootDeclaration()
            shapePaletteAtPosition(editor, "", rootDeclaration.position(), function(ef, palette){
                var oc = ef.visualParent
                languageHandler.rootFragment = oc.control.editFragment
                if ( callback ){
                    callback(ef, palette)
                }
            })
        })
    }

    function userBind(editor, userPosition, callback){
        try{
            var languageHandler = editor.code.language
            var declaration = languageHandler.findDeclaration(userPosition)
            if ( !declaration )
                return
            var palettes = languageHandler.findPalettes(declaration)
            if ( !palettes )
                return

            var position = declaration.position()
            palettes = filterOutPalettes(palettes)

            if ( palettes.length === 1 ){
                var ef = languageHandler.openConnection(declaration)
                if ( !ef ){
                    throw new Error(lang("QmlLanguage: Failed to create connection at: %0").format(position))
                }

                ef.incrementRefCount()
                languageHandler.openBinding(ef, palettes[0].name)
            } else {
                editor.internalFocus = false

                var paletteList = views.openPaletteListBox(
                    theme.selectableListView,
                    palettes,
                    {
                        aroundRect: editor.getCursorRectangle(),
                        panePosition: editor.getGlobalPosition(),
                        relativePlacement: lk.layers.editor.environment.placement.bottom
                    },
                    {
                        onCancelled: function(paletteListBox){
                            editor.forceFocus()
                            paletteListBox.destroy()
                        },
                        onSelected: function(index, paletteListBox){
                            editor.forceFocus()
                            paletteListBox.destroy()
                            var ef = languageHandler.openConnection(declaration)
                            if ( !ef ){
                                throw new Error(lang("QmlLanguage: Failed to create connection at: %0").format(position))
                            }

                            ef.incrementRefCount()
                            languageHandler.openBinding(ef, palettes[index].name)
                            if ( callback )
                                callback(ef)
                        }
                    }
                )
                lk.layers.workspace.panes.setActiveItem(paletteList, editor)
            }
        } catch( e ){
            lk.layers.workspace.messages.pushErrorObject(e)
        }
    }

    function openPaletteInPropertyContainer(propertyContainer, paletteName){
        var paletteItem = propertyContainer.control.paletteByName(paletteName)
        if ( paletteItem )
            return paletteItem

        var ef = propertyContainer.control.editFragment
        if ( !ef )
            return
        var editor = ef.language.code.textEdit().getEditor()

        if ( paletteName === root.defaultPalette ){
            var palettes = ef.language.findPalettes(ef)
            for ( var i = 0; i < palettes.length; ++i ){
                if ( palettes[i].isDefault ){
                    paletteName = palettes[i].name
                    break
                }
            }
        }
        if ( !paletteName.length || paletteName === '-' )
            return

        var palette = editor.code.language.expand(ef, {
            "palettes" : [paletteName]
        })
        var paletteBox = __factories.createPaletteContainer(palette, propertyContainer.control.activePaletteGroup(), { moveEnabled: false })
        return paletteBox
    }

    function openPaletteInObjectContainer(objectContainer, paletteName){
        var ef = objectContainer.control.editFragment
        var paletteBoxParent = objectContainer.control.paletteGroup
        var editor = ef.language.code.textEdit().getEditor()
        var ch = ef.language
        if (!ef)
            return

        if ( paletteName === root.defaultPalette ){
            var palettes = ef.language.findPalettes(ef)
            for ( var i = 0; i < palettes.length; ++i ){
                if ( palettes[i].isDefault ){
                    paletteName = palettes[i].name
                    break
                }
            }
        }
        if ( !paletteName.length || paletteName === '-' )
            return
        var palette = ch.expand(ef, { "palettes" : [paletteName] })
        var paletteBox = __factories.createPaletteContainer(palette, paletteBoxParent, { moveEnabled: false })
        paletteBoxParent.measureSizeInfo()

        if ( palette.item ){ // send expand signal
            if ( objectContainer.control.compact )
                objectContainer.control.expand()
        } else {
            expandObjectContainerLayout(objectContainer, palette, {expandChildren: false})
        }

        return paletteBox
    }

    function addPropertyToObjectContainer(objectContainer, name, readOnly, position, context){
        var languageHandler = objectContainer.control.editFragment.language

        var propContainer = objectContainer.control.propertyByName(name)

        if ( propContainer ){
            objectContainer.control.expand()
            return propContainer
        }

        objectContainer.control.expand()

        var metaTypeInfo = languageHandler.typeInfo(objectContainer.control.editFragment)
        var propertyInfo = metaTypeInfo.propertyInfo(name)
        if ( !propertyInfo ){
            lk.layers.workspace.messages.pushError("Error: Failed to get property info for '" + name +  "' from '" + objectContainer.editFragment.typeName() +  "'", 1)
            return null
        }

        var populateValueFromPalette = false

        var ef = null
        var isWritable = readOnly ? false : propertyInfo.isWritable
        if (isWritable){
            var defaultValue = EditQml.MetaInfo.defaultTypeValue(propertyInfo.type)
            var groupParentFragment = (objectContainer.control.editFragment.fragmentType() & EditQml.QmlEditFragment.Group) ? objectContainer.editFragment : null
            var addedProperty = languageHandler.addPropertyToCode( position, name, defaultValue, groupParentFragment)
            if ( addedProperty.totalCharsAdded > 0 )
                populateValueFromPalette = true

            var declaration = languageHandler.findDeclaration(addedProperty.position)
            var fragmentsResult = languageHandler.openChildConnections(objectContainer.control.editFragment, [declaration])
            if ( fragmentsResult.hasReport() ){
                for ( var i = 0; i < fragmentsResult.report.length; ++i ){
                    lk.layers.workspace.messages.pushErrorObject(fragmentsResult.report[i])
                }
            }

            ef = fragmentsResult.value.length ? fragmentsResult.value[0] : null
        } else {
            ef = languageHandler.openReadOnlyPropertyConnection(objectContainer.control.editFragment, name)
        }

        if (!ef) {
            lk.layers.workspace.messages.pushError("Error: Failed to create property inside object: " + name, 1)
            return null
        }

        objectContainer.control.editFragment.signalChildAdded(ef, context)

        if ( populateValueFromPalette ){
            var paletteList = ef.paletteList()
            for ( var i = 0; i < paletteList.length; ++i ){
                if ( paletteList[i].writer ){
                    paletteList[i].writer()
                    break
                }
            }
        }
        return objectContainer.control.propertyByName(name)
    }

    function addEventToObjectContainer(objectContainer, name, position, context){
        // check if event is opened already
        var propContainer = objectContainer.control.propertyByName(name)
        if ( propContainer ){
            objectContainer.control.expand()
            return propContainer
        }

        var languageHandler = objectContainer.control.editFragment.language
        var ppos = languageHandler.addEventToCode(position, name)

        var declaration = languageHandler.findDeclaration(ppos)
        var fragmentsResult = languageHandler.openChildConnections(objectContainer.control.editFragment, [declaration])
        if ( fragmentsResult.hasReport() ){
            for ( var i = 0; i < fragmentsResult.report.length; ++i ){
                lk.layers.workspace.messages.pushErrorObject(fragmentsResult.report[i])
            }
        }

        var ef = fragmentsResult.value.length ? fragmentsResult.value[0] : null

        if (ef){
            objectContainer.control.editFragment.signalChildAdded(ef, context)
        }

        return objectContainer.control.propertyByName(name)
    }

    //TODO: Add return type (last child)
    function addObjectToObjectContainer(container, typeOptions, extraProperties, position, context){
        var languageHandler = container.control.editFragment.language

        var opos = languageHandler.addObjectToCode(position, typeOptions, extraProperties)
        languageHandler.createObjectInRuntime(container.control.editFragment, typeOptions, extraProperties)

        var declaration = languageHandler.findDeclaration(opos)
        var fragmentsResult = languageHandler.openChildConnections(container.control.editFragment, [declaration])
        if ( fragmentsResult.hasReport() ){
            for ( var i = 0; i < fragmentsResult.report.length; ++i ){
                lk.layers.workspace.messages.pushErrorObject(fragmentsResult.report[i])
            }
        }

        var ef = fragmentsResult.value.length ? fragmentsResult.value[0] : null

        if (!ef){
            lk.layers.workspace.messages.pushError("Error: Failed to create object child: " + typeOptions, 1)
            return null
        }

        container.control.editFragment.signalChildAdded(ef, context)
        container.control.__sortContainerChildren()
    }

    function userAddToObjectContainer(container, handlers){
        var languageHandler = container.control.editor.code.language

        var isGroup = container.control.editFragment.fragmentType() & EditQml.QmlEditFragment.Group

        var position = container.control.editFragment.position()
        if (isGroup)
            position += 1

        var addContainer = languageHandler.getAddOptions({ editFragment: container.control.editFragment, isReadOnly: isGroup })
        if ( !addContainer )
            return

        var pane = container.control.getPane()
        var coords = container.control.mapToPane(pane)

        if ( container.control.pane )
            coords.y = coords.y - 30 // if this container is in the title of a pane

        var paneCoords = pane.mapGlobalPosition()

        var categories = ['properties', 'events']
        if (!isGroup && addContainer.model.supportsObjectNesting()){
            categories.push('objects')
        }
        if (container.control.supportsFunctions){
            categories.push('functions')
        }

        var addEditorBox = views.openAddOptionsBox(
            addContainer,
            languageHandler,
            {
                aroundRect: Qt.rect(coords.x + container.width - 180 / 2, coords.y, 30, 30),
                panePosition: Qt.point(paneCoords.x, paneCoords.y - 35),
                relativePlacement: lk.layers.editor.environment.placement.top
            },
            {
                categories: categories,
                onCancelled: function(box){
                    box.child.finalize()
                    if ( handlers && handlers.onCancelled )
                        handlers.onCancelled()
                },
                onFinalized: function(box){
                    box.child.destroy()
                    box.destroy()
                },
                onAccepted: function(box, selection){
                    if ( selection.category === 'property' ){ // property
                        addPropertyToObjectContainer(container, selection.name, selection.mode === AddQmlBox.AddPropertyMode.AddAsReadOnly, selection.position)
                    } else if ( selection.category === 'object' ){ // object

                        if ( selection.extraProperties ){
                            views.openAddExtraPropertiesBox(selection.name, {
                                onAccepted: function(propertiesToAdd){
                                    addObjectToObjectContainer(container, { type: selection.name, id: selection.id }, propertiesToAdd, selection.position )
                                },
                                onCancelled: function(){}
                            })
                        } else {
                            addObjectToObjectContainer(container, { type: selection.name, id: selection.id }, null, selection.position)
                        }

                    } else if ( selection.category === 'event' ){ // event
                        addEventToObjectContainer(container, selection.name, selection.position)

                    } else if ( selection.category === 'function' ){
                        var node = container.nodeParent.item
                        node.addFunctionProperty(selection.name)
                    }

                    box.child.finalize()

                    if ( handlers && handlers.onAccepted )
                        handlers.onAccepted()
                }
            }
        )

        lk.layers.workspace.panes.setActiveItem(addEditorBox, container.editor)
    }

    function eraseObject(objectContainer){
        var rootDeclaration = objectContainer.control.editor.code.language.rootDeclaration()
        var rootDeleted = (objectContainer.control.editFragment.position() === rootDeclaration.position())
        objectContainer.control.editor.code.language.eraseObject(objectContainer.control.editFragment, !objectContainer.control.isForProperty)

        if (rootDeleted) {
            objectContainer.control.editFragment.language.rootFragment = null
            __private.createAddRootButton(objectContainer.control.editor)
        }

        if ( objectContainer.control.isForProperty ) {
            objectContainer.control.collapse()
        }
    }

    function closeObjectContainer(objectContainer){
        if ( objectContainer.pane )
            objectContainer.closeAsPane()
        var languageHandler = objectContainer.control.editor.code.language
        objectContainer.control.collapse()

        var rootDeclaration = languageHandler.rootDeclaration()
        if (rootDeclaration.position() === objectContainer.control.editFragment.position())
            languageHandler.rootFragment = null

        languageHandler.removeConnection(objectContainer.control.editFragment)

        var p = objectContainer.parent.parent
        if ( p.objectName === 'editorBox' ){ // if this is root for the editor box
            p.destroy()
        } else { // if this is nested
            //TODO: Check if this is nested within a property container
            if ( objectContainer.control.pane )
                objectContainer.control.closeAsPane()
            objectContainer.parent.destroy()
        }
    }

    function objectContainerToPane(objectContainer){
        if ( objectContainer.control.pane ){
            objectContainer.control.closeAsPane()
            return
        }

        objectContainer.control.expand()

        var objectPane = lk.layers.workspace.panes.createPane('objectPalette', {}, [400, 400])
        lk.layers.workspace.panes.splitPaneHorizontallyWith(
            objectContainer.control.editor.parent.parentSplitView,
            objectContainer.control.editor.parent.parentSplitViewIndex(),
            objectPane
        )

        var root = objectContainer.parent

        objectContainer.control.frame.objectContainerTitle.parent = objectPane.paneHeaderContent
        objectPane.objectContainer = objectContainer.control.frame
        objectPane.title = objectContainer.control.title
        objectContainer.control.pane = objectPane

        objectContainer.placeHolder.parent = root

        return objectPane
    }

    function getEditorLayout(editor){
        var languageHandler = editor.code.language

        var result = null
        var editFragments = languageHandler.editFragments()
        for ( var i = 0; i < editFragments.length; ++i ){
            if (editFragments[i].location === EditQml.QmlEditFragment.Object){
                result = getEditFragmentLayout(editFragments[i])
            }
        }

        if (languageHandler.importsFragment)
            result['shapeImports'] = true

        return result
    }

    function expandObjectContainerLayout(objectContainer, layout, options){
        var editor = objectContainer.control.editor

        if ( layout['type'] !== objectContainer.control.editFragment.type() )
            return

        if ('palettes' in layout){
            var palettes = layout['palettes']
            for (var i = 0; i < palettes.length; ++i){
                openPaletteInObjectContainer(objectContainer, palettes[i])
            }
        }

        if ('properties' in layout){
            var properties = layout['properties']
            for (var i = 0; i < properties.length; ++i){
                var prop = properties[i]
                var position = objectContainer.control.editFragment.valuePosition() +
                               objectContainer.control.editFragment.valueLength() - 1
                var readOnly = prop.hasOwnProperty('readOnly') ? prop.readOnly : false

                var propertyContainer = addPropertyToObjectContainer(
                    objectContainer, prop.name, readOnly, position, {location: 'PaletteFunctions.ExpandLayout' }
                )

                if ( prop.isObject ){
                    expandObjectContainerLayout(propertyContainer.childObjectContainer, prop.object)
                } else {
                    if ('palettes' in prop){
                        var propPalettes = prop['palettes']
                        for (var j = 0; j < propPalettes.length; ++j){
                            openPaletteInPropertyContainer(propertyContainer, propPalettes[j])
                        }
                    }
                }
            }
        }

        var expandChildren = true
        if ( options && options.hasOwnProperty('expandChildren') )
            expandChildren = options.expandChildren

        if ('children' in layout){
            var children = layout['children']
            var ef = objectContainer.control.editFragment

            var declarationsResult = language.findDeclarations(ef.position() + 1, ef.length() - 2)
            if ( declarationsResult.hasReport() ){
                for ( var i = 0; i < declarationsResult.report.length; ++i ){
                    lk.layers.workspace.messages.pushErrorObject(declarationsResult.report[i])
                }
            }
            var fragmentsResult = language.openChildConnections(ef, declarationsResult.value)
            if ( fragmentsResult.hasReport() ){
                for ( var i = 0; i < fragmentsResult.report.length; ++i ){
                    lk.layers.workspace.messages.pushErrorObject(fragmentsResult.report[i])
                }
            }

            var childFragments = fragmentsResult.value.filter(edf => edf.location === EditQml.QmlEditFragment.Object )

            if (children.length === childFragments.length){
                for (var i = 0; i < childFragments.length; ++i){
                    var childObjectContainer = null
                    if ( childFragments[i].visualParent ){
                        childObjectContainer = childFragments[i].visualParent
                    } else {
                        childObjectContainer = objectContainer.control.addChildObject(childFragments[i])
                    }

                    expandObjectContainerLayout(childObjectContainer, children[i])
                }
            }
        }
    }

    function expandLayout(editor, layout, callback){
        var language = editor.code.language

        if ( layout.hasOwnProperty('shapeImports' ) ){
            shapeImports(editor)
        }

        shapeRoot(editor, function(ef){
            expandObjectContainerLayout(ef.visualParent, layout)

            if ( callback )
                callback(objectContainer)
        })
    }

    function getEditFragmentLayout(editFragment){
        if ( editFragment.location === EditQml.QmlEditFragment.Object )
            return getEditFragmentLayoutAsObject(editFragment)

        var result = {}

        if ( editFragment.location === EditQml.QmlEditFragment.Property ){

            result['location'] = 'property'
            result['name'] = editFragment.identifier()
            result['readOnly'] = editFragment.fragmentType & EditQml.QmlEditFragment.ReadOnly

            var paletteList = editFragment.paletteList()

            var nameList = []
            for (var idx = 0; idx < paletteList.length; ++idx)
                nameList.push(paletteList[idx].name)

            if (nameList.length > 0)
                result['palettes'] = nameList

            if ( editFragment.valueLocation === EditQml.QmlEditFragment.Object ){
                result['isObject'] = true
                result['object'] = getEditFragmentLayoutAsObject(editFragment)
            }
        }
        return result
    }

    function getEditFragmentLayoutAsObject(editFragment){
        var result = {}
        result['location'] = 'object'
        result['type'] = editFragment.type()

        var paletteList = editFragment.paletteList()

        var nameList = []
        for (var idx = 0; idx < paletteList.length; ++idx)
            nameList.push(paletteList[idx].name)

        if (nameList.length > 0)
            result['palettes'] = nameList

        if ( editFragment.visualParent ){
            result['container'] = editFragment.visualParent.control.getLayoutState()
        }

        var childFragments = editFragment.getChildFragments()

        var objects = []
        var properties = []

        for (var i = 0; i < childFragments.length; ++i){
            var frag = childFragments[i]
            if (frag.location === EditQml.QmlEditFragment.Object)
                objects.push(getEditFragmentLayoutAsObject(frag))
            if (frag.location === EditQml.QmlEditFragment.Property)
                properties.push(getEditFragmentLayout(frag))
        }

        if (objects.length > 0)
            result['children'] = objects
        if (properties.length > 0)
            result['properties'] = properties

        return result
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


    function userAddCodeToPosition(editor, position, categories, handlers){
        var addContainer = editor.code.language.getAddOptions({ position: position })
        if ( !addContainer )
            return

        var rect = editor.getCursorRectangle()
        var paneCoords = editor.getGlobalPosition()

        if ( !categories )
            categories = ['objects', 'properties', 'events']

        var addEditorBox = root.views.openAddOptionsBox(
            addContainer,
            editor.code.language,
            {
                aroundRect: rect,
                panePosition: Qt.point(paneCoords.x, paneCoords.y),
                relativePlacement: lk.layers.editor.environment.placement.bottom
            },
            {
                categories: categories,
                onCancelled: function(box){
                    box.child.finalize()
                    if ( handlers && handlers.onCancelled )
                        handlers.onCancelled()
                },
                onAccepted: function(box, selection){
                    if ( selection.category === 'property' ){
                        var defaultValue = EditQml.MetaInfo.defaultTypeValue(selection.type)
                        var addedProperty = editor.code.language.addPropertyToCode(
                            selection.position, selection.name, defaultValue
                        )
                        if ( handlers && handlers.onAccepted ){
                            handlers.onAccepted(selection, addedProperty.position)
                        }
                    } else if ( selection.category === 'object' ){
                        if ( selection.extraProperties ){
                            views.openAddExtraPropertiesBox(selection.name, {
                                onAccepted: function(propertiesToAdd){
                                    var addedPosition = editor.code.language.addObjectToCode(selection.position, { type: selection.name, id: selection.id }, propertiesToAdd)
                                    if ( handlers && handlers.onAccepted ){
                                        handlers.onAccepted(selection, addedPosition)
                                    }
                                },
                                onCancelled: function(){
                                    if ( handlers && handlers.onCancelled ){
                                        handlers.onCancelled()
                                    }
                                }
                            })
                        } else {
                            var addedPosition = editor.code.language.addObjectToCode(selection.position, { type: selection.name, id: selection.id })
                            if ( addedPosition >= 0 && handlers && handlers.onAccepted){
                                handlers.onAccepted(selection, addedPosition)
                            }
                        }
                    } else if ( selection.category === 'event' ){
                        var addedPosition = editor.code.language.addEvent(selection.position, selection.name)
                        if ( addedPosition >= 0 && handlers && handlers.onAccepted){
                            handlers.onAccepted(selection, addedPosition)
                        }
                    }
                    box.child.finalize()
                },
                onFinalized: function(box){
                    box.child.destroy()
                    box.destroy()
                }
            }
        )

        lk.layers.workspace.panes.setActiveItem(addEditorBox, editor.parent)
    }

}
