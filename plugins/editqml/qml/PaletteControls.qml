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
            var codeHandler = ef.codeHandler
            var editor = codeHandler.documentHandler.textEdit().getEditor()
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
            var editor = ef.codeHandler.documentHandler.textEdit().getEditor()
            var codeHandler = editor.documentHandler.codeHandler

            var editorBox = lk.layers.editor.environment.createEmptyEditorBox(parent)
            var objectContainer = __factories.createObjectContainer(editor, ef, lk.layers.editor.environment.content)

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
                codeHandler.rootShaped = true
            }
            return objectContainer
        }
    }

    property bool instructionsShaping: false

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
            var palettes = container.editFragment.codeHandler.findPalettesFromFragment(editFragment)
            palettes.data = filterOutPalettes(palettes.data, paletteGroup.palettesOpened, true)
            if (!palettes.data || palettes.data.length === 0) return null

            var paletteList = openPaletteList(theme.selectableListView, palettes.data, parent,
            {
                onCancelled: function(){
                    container.objectGraph.paletteListOpened = false
                    container.objectGraph.activateFocus()
                },
                onSelected: function(index){
                    var palette = container.editor.documentHandler.codeHandler.expand(container.editFragment, {
                        "palettes" : [palettes.data[index].name]
                    })

                    var objectRoot = container.objectName === "objectNode" ? container : null
                    var paletteBox = __private.wrapPaletteInContainer(palette, paletteGroup)
                    if ( objectRoot ){
                        if ( palette.item ){ // send expand signal
                            if ( objectRoot.compact )
                                objectRoot.expand()
                        } else {
                            objectRoot.expandOptions(palette) // expand json palette
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
            var palettes = container.editFragment.codeHandler.findPalettesFromFragment(container.editFragment)
            palettes.data = filterOutPalettes(
                palettes.data,
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
                var palette = container.editor.documentHandler.codeHandler.expand(container.editFragment, {
                    "palettes" : [palettes.data[index].name]
                })

                var objectRoot = mode === PaletteControls.PaletteListMode.ObjectContainer
                               ? container.parent
                               : (container.objectName === "objectNode" ? container : null)
                var paletteBox = __private.wrapPaletteInContainer(palette, paletteGroup)
                if ( objectRoot ){
                    if ( palette.item ){ // send expand signal
                        if ( objectRoot.compact )
                            objectRoot.expand()
                    } else {
                        objectRoot.expandOptions(palette) // expand json palette
                    }
                }

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
            var addBoxItem = __factories.createAddQmlBox(null)
            addBoxItem.addContainer = addContainer
            addBoxItem.languageQmlHandler = codeHandler

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


    function userOpenPaletteAtPosition(editor, userPosition, callback){
        var codeHandler = editor.documentHandler.codeHandler
        var palettes = codeHandler.findPalettes(userPosition)
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
                        var newPalette = openPaletteAtPosition(editor, palettes.data[index].name, position)
                        if ( callback )
                            callback(newPalette)
                    }
                }
            )
            lk.layers.workspace.panes.setActiveItem(paletteList, editor)
        }
    }

    function openPaletteAtPosition(editor, paletteName, position){
        var codeHandler = editor.documentHandler.codeHandler
        var ef = codeHandler.openConnection(position)
        if (!ef){
            lk.layers.workspace.messages.pushError("Error: Can't create a palette in a non-compiled program", 1)
            return
        }
        ef.incrementRefCount()
        var palette = editor.documentHandler.codeHandler.expand(ef, {
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

        return palette
    }

    function openEditPaletteAtPosition(editor, position){
        var codeHandler = editor.documentHandler.codeHandler

        var rect = editor.editor.getCursorRectangle()
        var cursorCoords = editor.cursorWindowCoords()

        var ef = codeHandler.openConnection(editor.textEdit.cursorPosition)
        var palette = codeHandler.edit(ef)

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
    }

    function userShapePaletteAtPosition(editor, userPosition){
        var codeHandler = editor.documentHandler.codeHandler
        var palettes = codeHandler.findPalettes(userPosition)
        if ( !palettes )
            return

        var position = palettes.declaration.position

        if ( !palettes.data || palettes.data.length === 0 ){
            shapePaletteAtPosition(editor, "", position)
        } else if ( palettes.length === 1 ){
            shapePaletteAtPosition(editor, palettes.data[0].name, position)
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
                        shapePaletteAtPosition(editor, palettes.data[index].name, position)
                    }
                }
            )
            lk.layers.workspace.panes.setActiveItem(paletteList, editor)
        }
    }

    function shapePaletteAtPosition(editor, paletteName, position){
        var codeHandler = editor.documentHandler.codeHandler
        var ef = codeHandler.openConnection(position)

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
                if (objectContainer.editFragment.position() === codeHandler.findRootPosition()){
                    objectContainer.contentWidth = Qt.binding(function(){
                        return objectContainer.containerContentWidth > objectContainer.editorContentWidth
                                ? objectContainer.containerContentWidth
                                : objectContainer.editorContentWidth
                    })
                }
                objectContainer.expand()

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
            palette = codeHandler.expand(ef, {
                "palettes" : [paletteName]
            })
        }

        var forImports = ef.location === EditQml.QmlEditFragment.Imports

        if (palette){
            if (forAnObject && !palette.item ){
                objectContainer.expandOptions(palette)
            }
            var paletteBox = __private.wrapPaletteInContainer(palette, paletteBoxGroup, {moveEnabled: false})
        }

        var frameBoxPosition = ef.position()
        if (forImports){
            frameBoxPosition = codeHandler.checkPragma(ef.position())
        }
        editor.documentHandler.frameBox(editorBox, frameBoxPosition, ef.length() + ef.position() - frameBoxPosition)

        if (forImports)
            editor.documentHandler.codeHandler.importsShaped = true //TODO: Move to LanguageQmlHandler

        ef.incrementRefCount()

        return { objectContainer: objectContainer, palette: palette }
    }

    function shapeImports(editor){
        var codeHandler = editor.documentHandler.codeHandler
        var importsPosition = codeHandler.findImportsPosition()
        var paletteImports = codeHandler.findPalettes(importsPosition)
        if (paletteImports) {
            var position = paletteImports.declaration.position
            paletteImports.data = filterOutPalettes(paletteImports.data)

            var result = shapePaletteAtPosition(editor, paletteImports.data[0].name, position)
            var palette = result.palette
            palette.item.width = Qt.binding(function(){
                if (!palette.item.parent || !palette.item.parent.parent)
                    return
                var editorSize = editor.width - editor.editor.lineSurfaceWidth - 30 - palette.item.parent.parent.headerWidth
                return editorSize > 280 ? editorSize : 280
            })
        }
        return paletteImports
    }

    function shapeRoot(editor, callback){
        var codeHandler = editor.documentHandler.codeHandler

        editor.startLoadingMode()
        codeHandler.removeSyncImportsListeners()
        codeHandler.onImportsScanned(function(){
            editor.stopLoadingMode()

            var rootPosition = codeHandler.findRootPosition()
            var result = shapePaletteAtPosition(editor, "", rootPosition)
            var oc = result.objectContainer

            oc.contentWidth = Qt.binding(function(){
                return oc.containerContentWidth > oc.editorContentWidth ? oc.containerContentWidth : oc.editorContentWidth
            })
            codeHandler.rootShaped = true
            if ( callback ){
                callback(oc)
            }
        })
    }

    function userBind(editor, userPosition){
        var codeHandler = editor.documentHandler.codeHandler

        var palettes = codeHandler.findPalettes(userPosition)
        if ( !palettes )
            return

        var position = palettes.declaration.position
        palettes.data = filterOutPalettes(palettes.data)

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

    function openPaletteInPropertyContainer(propertyContainer, paletteName){
        var paletteItem = propertyContainer.paletteByName(paletteName)
        if ( paletteItem )
            return paletteItem

        var ef = propertyContainer.editFragment
        if ( !ef )
            return
        var editor = ef.codeHandler.documentHandler.textEdit().getEditor()

        if ( paletteName === root.defaultPalette ){
            paletteName = ef.codeHandler.defaultPalette(ef)
        }
        if ( !paletteName.length )
            return

        var palette = editor.documentHandler.codeHandler.expand(ef, {
            "palettes" : [paletteName]
        })
        var paletteBox = __private.wrapPaletteInContainer(palette, propertyContainer.paletteGroup(), { moveEnabled: false })
        return paletteBox
    }

    function openPaletteInObjectContainer(objectContainer, paletteName){
        var ef = objectContainer.editFragment
        var paletteBoxParent = objectContainer.paletteListContainer.children[0]
        var editor = ef.codeHandler.documentHandler.textEdit().getEditor()
        var ch = ef.codeHandler
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
            objectContainer.expandOptions(palette) // expand json palette
        }

        return paletteBox
    }

    function addPropertyToObjectContainer(objectContainer, position, name, type, readOnly){
        //name = selection.name
        var codeHandler = objectContainer.editFragment.codeHandler

        // check if property is opened already
        for (var i = 0; i < objectContainer.propertiesOpened.length; ++i){
            if (objectContainer.propertiesOpened[i] === name){
                objectContainer.expand()
                return
            }
        }
        objectContainer.expand()

        var propsWritable = codeHandler.propertiesWritable(objectContainer.editFragment)

        var ef = null

        var isWritable = propsWritable[name]
        if (readOnly){
            isWritable = false
        }

        if (isWritable){
            var defaultValue = EditQml.MetaInfo.defaultTypeValue(type)
            var groupParentFragment = objectContainer.editFragment.isGroup() ? objectContainer.editFragment : null
            var ppos = codeHandler.addPropertyToCode( position, name, defaultValue, groupParentFragment)
            ef = codeHandler.openNestedConnection(objectContainer.editFragment, ppos)

            if (objectContainer.editFragment.isGroup())
                ef.addFragmentType(EditQml.QmlEditFragment.GroupChild)
        } else {
            ef = codeHandler.createReadOnlyPropertyFragment(objectContainer.editFragment, name)
        }

        if (!ef) {
            lk.layers.workspace.messages.pushError("Error: Can't create a palette in a non-compiled program", 1)
            return
        }

        objectContainer.editFragment.signalPropertyAdded(ef)

        var paletteList = ef.paletteList()
        for ( var i = 0; i < paletteList.length; ++i ){
            if ( paletteList[i].writer ){
                paletteList[i].writer()
                break
            }
        }
    }

    function addEventToObjectContainer(container, position, name){
        // check if event is opened already
        for (var i = 0; i < container.propertiesOpened.length; ++i){
            if (container.propertiesOpened[i] === name){
                container.expand()
                return
            }
        }

        var ppos = codeHandler.addEventToCode(position, name)
        var ef = codeHandler.openNestedConnection(container.editFragment, ppos)
        if (ef){
            container.editFragment.signalPropertyAdded(ef)
        }
        return ef
    }

    function addObjectToObjectContainer(container, position, type, extraProperties){
        var codeHandler = container.editFragment.codeHandler

        var opos = codeHandler.addObjectToCode(position, type, extraProperties)
        codeHandler.addItemToRuntime(container.editFragment, type, extraProperties)
        var ef = codeHandler.openNestedConnection(container.editFragment, opos)
        if (!ef)
            return

        container.editFragment.signalObjectAdded(ef)
        container.sortChildren()
    }

    function userAddToObjectContainer(container, handlers){
        var codeHandler = container.editor.documentHandler.codeHandler

        var isForNode = container.objectName === "objectNode"
        var isGroup = container.editFragment.isGroup()

        var position = container.editFragment.position()
        if (isGroup)
            position += 1

        // This will have to be transfered to addContainer
        //TODO: will need to set this as a parameter
//        var filter = 0 | (isForNode ? LanguageQmlHandler.ForNode : 0) | (isGroup ? LanguageQmlHandler.ReadOnly : 0)

        var addContainer = codeHandler.getAddOptions({ editFragment: container.editFragment, isReadOnly: isGroup })
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
                    if ( handlers && handlers.onCancelled )
                        handlers.onCancelled()
                },
                onFinalized: function(box){
                    box.child.destroy()
                    box.destroy()
                },
                onAccepted: function(box, selection){
                    if ( selection.category === 'property' ){ // property
                        addPropertyToObjectContainer(container, selection.position, selection.name, selection.type, selection.mode === AddQmlBox.AddPropertyMode.AddAsReadOnly)
                    } else if ( selection.category === 'object' ){ // object

                        if ( selection.extraProperties ){
                            views.openAddExtraPropertiesBox(selection.name, {
                                onAccepted: function(propertiesToAdd){
                                    addObjectToObjectContainer(container, selection.position, selection.name, propertiesToAdd )
                                },
                                onCancelled: function(){}
                            })
                        } else {
                            //TODO: use `id` instead of `selection.name` which is `Type#id`
                            addObjectToObjectContainer(container, selection.position, selection.name )
                        }

                    } else if ( selection.category === 'event' ){ // event
                        addEventToObjectContainer(container, selection.position, selection.name)

                    } else if (isForNode && selection.category === 'function' ){
                        var node = container.nodeParent.item
                        //TODO: remove isForNode in favor of addFunctionProperty
                        //HERE
//                        container.nodeParent.item.addSubobject(container.nodeParent, selection.name, container.nodeParent.item.id ? ObjectGraph.PortMode.InPort : ObjectGraph.PortMode.None, null, {isMethod: true})
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
        var rootDeleted = (objectContainer.editFragment.position() === objectContainer.editor.documentHandler.codeHandler.findRootPosition())
        objectContainer.editor.documentHandler.codeHandler.eraseObject(objectContainer.editFragment, !objectContainer.isForProperty)

        if (rootDeleted) {
            objectContainer.editFragment.codeHandler.rootShaped = false
            objectContainer.editor.addRootButton.visible = true
        }

        if ( objectContainer.isForProperty ) {
            objectContainer.collapse()
        }
    }

    // populateObjectInfo()
    // instructions

    function closeObjectContainer(objectContainer){
        if ( objectContainer.pane )
            objectContainer.closeAsPane()
        var codeHandler = objectContainer.editor.documentHandler.codeHandler
        objectContainer.collapse()
        var rootPos = codeHandler.findRootPosition()
        if (rootPos === objectContainer.editFragment.position())
            codeHandler.rootShaped = false

        codeHandler.removeConnection(objectContainer.editFragment)

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
    }

    // Instructions
    // -----------------------------------------------

    function shapeAtPositionWithInstructions(editor, position, instructions){

        //TODO: shapePaletteAtPosition
        //TODO: shapeImports
        //TODO: When ready: continue with instructions

        instructionsShaping = true
        var codeHandler = editor.documentHandler.codeHandler

        if ("shapeImports" in instructions){
            lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)
        }

        var ef = codeHandler.openConnection(position)

        if (!ef)
            return

        var objectContainer = createObjectContainerForFragment(ef)
        if (objectContainer.editFragment.position() === codeHandler.findRootPosition())
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
        if (instructions['type'] !== objectContainer.editFragment.type()) return

        var objects = objectContainer.editor.documentHandler.codeHandler.openNestedFragments(objectContainer.editFragment, ['objects'])

        var containers = []
        for (var i=0; i < objects.length; ++i){
            var childObjectContainer = objectContainer.addChildObject(objects[i])
            containers.push(childObjectContainer)
        }

        var hasChildren = false

        if ('palettes' in instructions){
            var palettes = instructions['palettes']
            for (var i = 0; i < palettes.length; ++i){
                var palette = palettes[i]
                openPaletteInObjectContainer(objectContainer, palettes[i])

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
            objectContainer.paletteListContainer.sortChildren()
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
            propertyContainer = __factories.createPropertyContainer(objectContainer.paletteListContainer, objectContainer.editor, ef)
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
                openPaletteInPropertyContainer(propertyContainer, palette)
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
        return addPropertyByFragment(container.editFragment, name)
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

        var propEf = codeHandler.openNestedConnection(ef, ppos)

        if (propEf) {
            ef.signalPropertyAdded(propEf, false)
        }

        return propEf
    }

    function convertEditorStateIntoInstructions(editor){
        var codeHandler = editor.documentHandler.codeHandler

        var result = null
        var editFragments = codeHandler.editFragments()
        for ( var i = 0; i < editFragments.length; ++i ){
            if (editFragments[i].location === EditQml.QmlEditFragment.Object){
                result = convertObjectIntoInstructions(editFragments[i])
            }
        }

        if (codeHandler.importsShaped)
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
            if (frag.location === EditQml.QmlEditFragment.Object)
                objects.push(convertObjectIntoInstructions(frag))
            if (frag.location === EditQml.QmlEditFragment.Property)
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
        if (!p)
            return result

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
