import QtQuick 2.0
import editqml 1.0 as EditQml
import editor 1.0
import workspace.nodeeditor 1.0

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

        function createAddQmlBox(parent){
            return __factories.addQmlBox.createObject(parent)
        }

        function createPaletteGroup(parent, ef){
            var pg = __factories.paletteGroup.createObject(parent)
            pg.__initialize(ef)
            return pg
        }

        function createObjectContainer(editor, ef, parent){
            var oc = __factories.objectContainer.createObject(parent)
            oc.__initialize(editor, ef)
            return oc
        }

        function createPaletteContainer(parent){
            return __factories.paletteContainer.createObject(parent)
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

        //TOMOVE: into factories
        function wrapPaletteInContainer(palette, paletteBoxParent, options){
            if ( !palette.item )
                return null

            var newPaletteBox = __factories.createPaletteContainer(paletteBoxParent)
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

            var rootPos = languageHandler.findRootPosition()
            if (ef.position() === rootPos){
                objectContainer.contentWidth = Qt.binding(function(){
                    return objectContainer.containerContentWidth > objectContainer.editorContentWidth ? objectContainer.containerContentWidth : objectContainer.editorContentWidth
                })
                languageHandler.rootFragment = ef
            }
            return objectContainer
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

        function openPaletteListForNode(container, paletteGroup, parent){
            var palettes = container.editFragment.language.findPalettesForFragment(container.editFragment)
            palettes.data = filterOutPalettes(palettes.data, paletteGroup.palettesOpened, true)
            if (!palettes.data || palettes.data.length === 0) return null

            var paletteList = openPaletteList(theme.selectableListView, palettes.data, parent,
            {
                onCancelled: function(){
                    container.objectGraph.paletteListOpened = false
                    container.objectGraph.activateFocus()
                },
                onSelected: function(index){
                    var palette = container.editor.code.language.expand(container.editFragment, {
                        "palettes" : [palettes.data[index].name]
                    })

                    var objectRoot = container.objectName === "objectNode" ? container : null
                    var paletteBox = __private.wrapPaletteInContainer(palette, paletteGroup)
                    if ( objectRoot ){
                        if ( palette.item ){ // send expand signal
                            if ( objectRoot.compact )
                                objectRoot.expand()
                        } else {
                            expandObjectContainerLayout(objectRoot, palette, {expandChildren: false})
                        }
                    }

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
            var palettes = container.editFragment.language.findPalettesForFragment(container.editFragment)
            palettes.data = filterOutPalettes(
                palettes.data,
                paletteGroup.palettesOpened,
                mode === PaletteFunctions.PaletteListMode.ObjectContainer
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
                var palette = container.editor.code.language.expand(container.editFragment, {
                    "palettes" : [palettes.data[index].name]
                })

                var objectRoot = mode === PaletteFunctions.PaletteListMode.ObjectContainer
                               ? container.parent
                               : (container.objectName === "objectNode" ? container : null)
                var paletteBox = __private.wrapPaletteInContainer(palette, paletteGroup)
                if ( container ){
                    if ( palette.item ){ // send expand signal
                        if ( container.compact )
                            container.expand()
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

    // Public

    function userOpenPaletteAtPosition(editor, userPosition, callback){
        var languageHandler = editor.code.language
        var palettes = languageHandler.findPalettes(userPosition)
        if ( !palettes )
            return

        var position = palettes.declaration.position
        palettes.data = filterOutPalettes(palettes.data)
        if ( !palettes.data || palettes.data.length === 0){
            return
        }

        if ( palettes.data.length === 1 ){
            openPaletteAtPosition(editor, palettes.data[0].name, position)
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
                        if ( callback )
                            callback(null)
                    },
                    onSelected: function(index, paletteListBox){
                        editor.editor.forceFocus()
                        paletteListBox.destroy()
                        var newPalette = openPaletteAtPosition(editor, palettes.data[index].name, position, callback)
                    }
                }
            )
            lk.layers.workspace.panes.setActiveItem(paletteList, editor)
        }
    }

    function openPaletteAtPosition(editor, paletteName, position, callback){
        var languageHandler = editor.code.language
        var ef = languageHandler.openConnection(position)
        if (!ef){
            lk.layers.workspace.messages.pushError("Error: Can't create a palette in a non-compiled program", 1)
            return
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

        var paletteBox = __private.wrapPaletteInContainer(palette, paletteBoxGroup)

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        // Update new placement so as to animate
        editorBox.updatePlacement(rect, cursorCoords, lk.layers.editor.environment.placement.top)

        if ( callback )
            callback(ef, palette)
    }

    function openEditPaletteAtPosition(editor, position, callback){
        var languageHandler = editor.code.language

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        var ef = languageHandler.openConnection(editor.textEdit.cursorPosition)
        var palette = languageHandler.edit(ef)

        var editorBox = lk.layers.editor.environment.createEmptyEditorBox()
        var paletteGroup = __factories.createPaletteGroup(lk.layers.editor.environment.content, ef)
        editorBox.setChild(paletteGroup, rect, cursorCoords, lk.layers.editor.environment.placement.top)
        paletteGroup.x = 2
        editorBox.color = "black"
        editorBox.border.width = 1
        editorBox.border.color = "#141c25"

        var paletteBox = __private.wrapPaletteInContainer(palette, paletteGroup)
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
    }

    function userShapePaletteAtPosition(editor, userPosition, callback){
        var languageHandler = editor.code.language
        var palettes = languageHandler.findPalettes(userPosition)
        if ( !palettes )
            return

        var position = palettes.declaration.position

        if ( !palettes.data || palettes.data.length === 0 ){
            shapePaletteAtPosition(editor, "", position, function(ef, palette){
                if ( ef.visualParent.expand )
                    ef.visualParent.expand()
                if ( callback )
                    callback(ef, palette)
            })
        } else if ( palettes.length === 1 ){
            shapePaletteAtPosition(editor, palettes.data[0].name, position, function(ef, palette){
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
                        shapePaletteAtPosition(editor, palettes.data[index].name, position, function(ef, palette){
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
        var languageHandler = editor.code.language
        var ef = languageHandler.openConnection(position)

        if (!ef){
            lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't shape palette"
            throw linkError(new Error('Failed to find editing fragment for palette at: ' + position, 300))
        }

        var forAnObject = ef.location === EditQml.QmlEditFragment.Object
        var editorBox = ef.visualParent ? ef.visualParent.parent : null

        var objectContainer = null
        var paletteBoxGroup = editorBox ? editorBox.child : null
        if ( paletteBoxGroup === null ){
            if (forAnObject){
                objectContainer = __private.createEditorObjectContainerBoxForFragment(ef, editor.textEdit)
                if (objectContainer.editFragment.position() === languageHandler.findRootPosition()){
                    objectContainer.contentWidth = Qt.binding(function(){
                        return objectContainer.containerContentWidth > objectContainer.editorContentWidth
                                ? objectContainer.containerContentWidth
                                : objectContainer.editorContentWidth
                    })
                }

                //TOREMOVE
                objectContainer.title = ef.typeName() + (ef.objectId() ? ("#" + ef.objectId()) : "")

                paletteBoxGroup = objectContainer.paletteGroup
                editorBox = objectContainer.parent

            } else {
                editorBox = __private.createEditorPaletteBoxForFragment(ef, editor.textEdit)
                paletteBoxGroup = ef.visualParent
            }
        } else {
            var p = paletteBoxGroup
            while (p && p.objectName !== "objectContainer") p = p.parent
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
            var paletteBox = __private.wrapPaletteInContainer(palette, paletteBoxGroup, {moveEnabled: false})
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
    }

    function shapeImports(editorPane, callback){
        var languageHandler = editorPane.code.language
        var importsPosition = languageHandler.findImportsPosition()
        var paletteImports = languageHandler.findPalettes(importsPosition)
        if (paletteImports) {
            var position = paletteImports.declaration.position
            paletteImports.data = filterOutPalettes(paletteImports.data)

            shapePaletteAtPosition(editorPane, paletteImports.data[0].name, position, function(ef, palette){
                palette.item.width = Qt.binding(function(){
                    if (!palette.item.parent || !palette.item.parent.parent)
                        return
                    var editorSize = editorPane.width - editorPane.editor.lineSurfaceWidth - 30 - palette.item.parent.parent.headerWidth
                    return editorSize > 280 ? editorSize : 280
                })

                if ( callback ){
                    callback(ef, palette)
                }
            })
        }
    }

    function shapeRoot(editor, callback){
        var languageHandler = editor.code.language

        editor.startLoadingMode()
        languageHandler.removeSyncImportsListeners()
        languageHandler.onImportsScanned(function(){
            editor.stopLoadingMode()

            var rootPosition = languageHandler.findRootPosition()
            var result = shapePaletteAtPosition(editor, "", rootPosition, function(ef, palette){
                var oc = ef.visualParent

                oc.contentWidth = Qt.binding(function(){
                    return oc.containerContentWidth > oc.editorContentWidth ? oc.containerContentWidth : oc.editorContentWidth
                })
                languageHandler.rootFragment = oc.editFragment
                if ( callback ){
                    callback(oc.editFragment)
                }
            })

        })
    }

    function userBind(editor, userPosition, callback){
        var languageHandler = editor.code.language

        var palettes = languageHandler.findPalettes(userPosition)
        if ( !palettes )
            return

        var position = palettes.declaration.position
        palettes.data = filterOutPalettes(palettes.data)

        if ( palettes.data.length === 1 ){
            var ef = languageHandler.openConnection(position)
            ef.incrementRefCount()
            languageHandler.openBinding(ef, palettes.data[0].name)
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
                        var ef = languageHandler.openConnection(position)
                        ef.incrementRefCount()
                        languageHandler.openBinding(ef, palettes.data[index].name)
                        if ( callback )
                            callback(ef)
                    }
                }
            )
            lk.layers.workspace.panes.setActiveItem(paletteList, editor)
        }
    }

    function openPaletteInPropertyContainer(propertyContainer, paletteName){
        var paletteItem = propertyContainer.paletteByName(paletteName)
        if ( paletteItem )
            return paletteItem

        var ef = propertyContainer.editFragment
        if ( !ef )
            return
        var editor = ef.language.code.textEdit().getEditor()

        if ( paletteName === root.defaultPalette ){
            paletteName = ef.language.defaultPalette(ef)
        }
        if ( !paletteName.length )
            return

        var palette = editor.code.language.expand(ef, {
            "palettes" : [paletteName]
        })
        var paletteBox = __private.wrapPaletteInContainer(palette, propertyContainer.paletteGroup(), { moveEnabled: false })
        return paletteBox
    }

    function openPaletteInObjectContainer(objectContainer, paletteName){
        var ef = objectContainer.editFragment
        var paletteBoxParent = objectContainer.paletteListContainer.children[0]
        var editor = ef.language.code.textEdit().getEditor()
        var ch = ef.language
        if (!ef)
            return

        if ( paletteName === root.defaultPalette ){
            paletteName = ch.defaultPalette(ef)
        }
        if ( !paletteName.length )
            return

        var palette = ch.expand(ef, { "palettes" : [paletteName] })
        var paletteBox = __private.wrapPaletteInContainer(palette, paletteBoxParent, { moveEnabled: false })

        if ( palette.item ){ // send expand signal
            if ( objectContainer.compact )
                objectContainer.expand()
        } else {
            expandObjectContainerLayout(objectContainer, palette, {expandChildren: false})
        }

        return paletteBox
    }

    function addPropertyToObjectContainer(objectContainer, name, readOnly, position){
        //name = selection.name
        var languageHandler = objectContainer.editFragment.language

        var propContainer = objectContainer.propertyByName(name)
        if ( propContainer ){
            objectContainer.expand()
            return propContainer
        }

        objectContainer.expand()

        var metaTypeInfo = languageHandler.typeInfo(objectContainer.editFragment.type())
        var propertyInfo = metaTypeInfo.propertyInfo(name)

        var ef = null
        var isWritable = readOnly ? false : propertyInfo.isWritable
        if (isWritable){
            var defaultValue = EditQml.MetaInfo.defaultTypeValue(propertyInfo.type)
            var groupParentFragment = (objectContainer.editFragment.fragmentType() & EditQml.QmlEditFragment.Group) ? objectContainer.editFragment : null
            var ppos = languageHandler.addPropertyToCode( position, name, defaultValue, groupParentFragment)
            ef = languageHandler.openNestedConnection(objectContainer.editFragment, ppos)
        } else {
            ef = languageHandler.openReadOnlyPropertyConnection(objectContainer.editFragment, name)
        }

        if (!ef) {
            lk.layers.workspace.messages.pushError("Error: Can't create a palette in a non-compiled program", 1)
            return null
        }

        objectContainer.editFragment.signalChildAdded(ef)

        var paletteList = ef.paletteList()
        for ( var i = 0; i < paletteList.length; ++i ){
            if ( paletteList[i].writer ){
                paletteList[i].writer()
                break
            }
        }

        return objectContainer.propertyByName(name)
    }

    function addEventToObjectContainer(objectContainer, name, position){
        // check if event is opened already
        var propContainer = objectContainer.propertyByName(name)
        if ( propContainer ){
            objectContainer.expand()
            return propContainer
        }

        var languageHandler = objectContainer.editingFragment.language
        var ppos = languageHandler.addEventToCode(position, name)
        var ef = languageHandler.openNestedConnection(objectContainer.editFragment, ppos)
        if (ef){
            objectContainer.editFragment.signalChildAdded(ef)
        }

        return objectContainer.propertyByName(name)
    }

    //TODO: Add return type (last child)
    //TODO: Configure `type`: can be either object, or string
    function addObjectToObjectContainer(container, type, extraProperties, position){
        var languageHandler = container.editFragment.language

        var opos = languageHandler.addObjectToCode(position, type, extraProperties)
        languageHandler.addItemToRuntime(container.editFragment, type, extraProperties)
        var ef = languageHandler.openNestedConnection(container.editFragment, opos)
        if (!ef)
            return null

        container.editFragment.signalChildAdded(ef)
        container.sortChildren()
    }

    function userAddToObjectContainer(container, handlers){
        var languageHandler = container.editor.code.language

        var isForNode = container.objectName === "objectNode"
        var isGroup = container.editFragment.fragmentType() & EditQml.QmlEditFragment.Group

        var position = container.editFragment.position()
        if (isGroup)
            position += 1

        var addContainer = languageHandler.getAddOptions({ editFragment: container.editFragment, isReadOnly: isGroup })
        if ( !addContainer )
            return

        var pane = container.getPane()
        var coords = container.mapToItem(pane, 0, 0)

        if ( container.parent.objectName !== 'objectContainer' )
            coords.y = coords.y - 30 // if this container is in the title of a pane

        var paneCoords = pane.mapGlobalPosition()

        var categories = ['properties', 'events']
        if (!isGroup && addContainer.model.supportsObjectNesting()){
            categories.push('objects')
        }
        if (container.supportsFunctions){
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
                                    addObjectToObjectContainer(container, selection.name, propertiesToAdd, selection.position )
                                },
                                onCancelled: function(){}
                            })
                        } else {
                            //TODO: use `id` instead of `selection.name` which is `Type#id`
                            addObjectToObjectContainer(container, selection.name, null, selection.position)
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
        var rootDeleted = (objectContainer.editFragment.position() === objectContainer.editor.code.language.findRootPosition())
        objectContainer.editor.code.language.eraseObject(objectContainer.editFragment, !objectContainer.isForProperty)

        if (rootDeleted) {
            objectContainer.editFragment.language.rootFragment = null
            objectContainer.editor.addRootButton.visible = true
        }

        if ( objectContainer.isForProperty ) {
            objectContainer.collapse()
        }
    }

    function closeObjectContainer(objectContainer){
        if ( objectContainer.pane )
            objectContainer.closeAsPane()
        var languageHandler = objectContainer.editor.code.language
        objectContainer.collapse()
        var rootPos = languageHandler.findRootPosition()
        if (rootPos === objectContainer.editFragment.position())
            languageHandler.rootFragment = null

        languageHandler.removeConnection(objectContainer.editFragment)

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

    function objectContainerToPane(objectContainer){
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
        var editor = objectContainer.editor

        if ( layout['type'] !== objectContainer.editFragment.type() )
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
                var position = objectContainer.editFragment.valuePosition() +
                               objectContainer.editFragment.valueLength() - 1
                var readOnly = prop.hasOwnProperty('readOnly') ? prop.readOnly : false

                var propertyContainer = addPropertyToObjectContainer(objectContainer, prop.name, readOnly, position)

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
            var childFragments = objectContainer.editFragment.language.openNestedFragments(objectContainer.editFragment, ['objects'])
            if (children.length === childFragments.length){
                for (var i = 0; i < childFragments.length; ++i){
                    var childObjectContainer = null
                    if ( childFragments[i].visualParent ){
                        childObjectContainer = childFragments[i].visualParent
                    } else {
                        childObjectContainer = objectContainer.addChildObject(childFragments[i])
                    }

                    expandObjectContainerLayout(childObjectContainer, children[i])
                }
            }
        }
    }

    function expandLayout(editorPane, layout, callback){
        var language = editorPane.editor.code.language

        if ( layout.hasOwnProperty('shapeImports' ) ){
            shapeImports(editorPane)
        }

        shapeRoot(editorPane, function(ef){
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
            result['container'] = editFragment.visualParent.getLayoutState()
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

    function createSuggestionBox(parent, font){
        var sb = factories.suggestionBox.createObject(parent)
        sb.__initialize(font)
        return sb
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

    // TOMOVE
    // -----------------------------------------------

    function addPropertyByName(container, name){
        if (container.propertiesOpened){
            for (var i = 0; i < container.propertiesOpened.length; ++i){
                if (container.propertiesOpened[i] === name){
                    return
                }
            }
        }

        var languageHandler = container.editor.code.language
        return addPropertyByFragment(container.editFragment, name)
    }

    function addPropertyByFragment(ef, name){

        var languageHandler = ef.language
        var position = ef.valuePosition() +
                       ef.valueLength() - 1

        var addContainer = languageHandler.getAddOptions(position)
        if ( !addContainer )
            return

        addContainer.model.setCategoryFilter(1)
        addContainer.model.setFilter(name)

        if (addContainer.model.rowCount() === 0) return

        var type = addContainer.model.data(addContainer.model.index(0, 0), 256 + 3/*QmlSuggestionModel.Type*/)

        var ppos = languageHandler.addProperty(
            addContainer.model.addPosition, addContainer.objectType, type, name, true
        )

        var propEf = languageHandler.openNestedConnection(ef, ppos)

        if (propEf) {
            ef.signalChildAdded(propEf, false)
        }

        return propEf
    }
}
