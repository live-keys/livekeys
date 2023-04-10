import QtQuick 2.0
import editor 1.0
import editor.private 1.0
import editqml 1.0 as EditQml
//import workspace.builder 1.0 as Builder

Item{
    id: root
    objectName: "objectContainer"
    property ObjectContainerControl control: ObjectContainerControl{}
    property var sizeInfo: ({minWidth: -1, minHeight: -1, maxWidth: -1, maxHeight: -1})
    property bool fillEditorWidth: false
    width: 0
    height: 0

    property Connections editorConnections: Connections{
        target: null
        function onWidthChanged(){ root.measureSizeInfo() }
        function onLineSurfaceWidthChanged(){ root.measureSizeInfo() }
    }

    function __initialize(editor, ef){
        root.control.__initialize(editor, ef, root, container, objectContainerFrame)
    }

    function clean(){
        root.control.clean()
        return root
    }

    function destroyObjectContainer(oc){
        oc.clean()
        oc.destroy()
    }

    function adjustSizeToEditor(){
        fillEditorWidth = true
        editorConnections.target = root.control.editor
        root.measureSizeInfo()
    }

    function adjustSize(){
        if ( !root.control.editor ){
            return 0
        }
        if ( root.control.pane ){
            var paneWidth = root.control.pane.width
            var width = paneWidth > root.sizeInfo.minWidth ? paneWidth : root.sizeInfo.minWidth
            root.width = width
            root.sizeInfo.contentWidth = width
            root.sizeInfo.objectContentWidth = width - 20
        } else if ( fillEditorWidth ){
            var editorWidth = root.control.editor.width - root.control.editor.lineSurfaceWidth - 20
            var width = editorWidth > root.sizeInfo.minWidth ? editorWidth : root.sizeInfo.minWidth
            root.width = width
            root.sizeInfo.contentWidth = width
            root.sizeInfo.objectContentWidth = width - 20
        } else {
            var w = 300
            if ( parent.sizeInfo.objectContentWidth ){
                w = parent.sizeInfo.objectContentWidth
            } else if ( parent.sizeInfo.contentWidth ){
                w = parent.sizeInfo.objectContentWidth
            }
            root.width = w
            root.sizeInfo.contentWidth = w
            root.sizeInfo.objectContentWidth = w - 20
        }

        for ( var i = 0; i < container.children.length; ++i ){
            var c = container.children[i]
            if ( c.adjustSize )
                c.adjustSize()
        }

        var height = 30
        if ( !root.control.compact ){
            if ( children.length > 0 ){
                for ( var i = 0; i < container.children.length; ++i ){
                    height += container.children[i].height
                }
                if ( container.children.length > 1 )
                    height += (container.children.length - 1) * container.spacing
            }
        }

        root.height = height
        container.height = root.height - 30
        container.width = root.width
    }

    function measureSizeInfo(){
        var size = { minWidth: -1, minHeight: -1, maxWidth: -1, maxHeight: -1, contentWidth: -1 }

        for ( var i = 0; i < container.children.length; ++i ){
            var si = container.children[i].sizeInfo

            if ( container.children[i].objectName === 'objectContainer' ){
                var minWidth = si.minWidth > 0 ? si.minWidth + 20 : 320
                if ( minWidth > size.minWidth )
                    size.minWidth = minWidth
            } else {
                if ( si.minWidth > 0 ){
                    if ( size.minWidth < 0 )
                        size.minWidth = si.minWidth
                    else if ( size.minWidth < si.minWidth ){
                        size.minWidth = si.minWidth
                    }
                }
            }

            if ( si.maxWidth > 0 ){
                if ( size.maxWidth < 0 )
                    size.maxWidth = si.maxWidth
                else if ( size.maxWidth < si.maxWidth ){
                    size.maxWidth = si.maxWidth
                }
            }
            if ( si.minHeight > 0 ){
                size.minHeight = size.minHeight < 0 ? si.minHeight : size.minHeight + si.minHeight
            }
            if ( si.maxHeight > 0 ){
                size.maxHeight = size.maxHeight < 0 ? si.maxHeight : size.maxHeight + si.maxHeight
            }
        }
        if ( size.minWidth < 300 ){
            size.minWidth = 300
        }
        if ( size.minHeight < 30 ){
            size.minHeight = 30
        }

        root.sizeInfo = size
        if ( root.parent && root.parent.measureSizeInfo ){
            root.parent.measureSizeInfo()
        } else {
            root.adjustSize()
        }
    }

    property Rectangle placeHolder : Rectangle{
        height: 30
        width: root.width
        color: "#222"
        radius: 3
        Text{
            anchors.left: parent.left
            anchors.leftMargin: 10
            width: parent.width - 20
            anchors.verticalCenter: parent.verticalCenter
            text: objectContainerFrame.control.title
            clip: true
            elide: Text.ElideRight
            color: '#82909b'
        }
    }

    Item{
        id: objectContainerFrame
        objectName: "objectContainerFrame"

        property alias groupsContainer: container
        property alias objectContainerTitle: objectContainerTitle
        property Item wrapper : root
        property ObjectContainerControl control: root.control

        Keys.onPressed: {
            var command = lk.layers.workspace.keymap.locateCommand(event.key, event.modifiers)
            if ( command ){
                lk.layers.workspace.commands.execute(command)
            }
            event.accepted = true
        }

        width: parent.width
        height: control.compact ? 30 : objectContainerTitleWrap.height + control.topSpacing + container.height + 3

        function closeAsPane(){
            objectContainerTitle.parent = objectContainerTitleWrap
            objectContainerFrame.parent = objectContainerFrame.wrapper
            objectContainerFrame.control.pane.removePane()
            objectContainerFrame.control.pane = null
            root.placeHolder.parent = null
        }

        property Connections editFragmentRemovals: Connections{
            target: control.editFragment
            function onConnectionChanged(){
                root.control.__whenEditFragmentConnectionChanged()
            }
            function onAboutToBeRemoved(){
                if (objectContainerFrame.control.isForProperty)
                    return
                var p = root.parent

                //TODO: Move to LanguageQmlHandler
                var language = root.control.editFragment.language
                var rootDeclaration = language.rootDeclaration()

                if (root.control.editFragment.position() === rootDeclaration.position())
                    root.control.editFragment.language.rootFragment = null

                if (!p)
                    return

                if ( p.objectName === 'editorBox' ){ // if this is root for the editor box
                    destroyObjectContainer(root)
                    p.destroy()
                } else { // if this is nested
                    //TODO: Check if this is nested within a property container
                    if ( objectContainerFrame.pane )
                        objectContainerFrame.closeAsPane()
                    if ( root.parent && root.parent.measureSizeInfo ){
                        var p = root.parent
                        root.parent = null
                        p.measureSizeInfo()
                    }
                    destroyObjectContainer(root)
                }
            }
        }

        property Connections addFragmentToContainerConn: Connections{
            target: control.editFragment
            ignoreUnknownSignals: true
            function onChildAdded(ef, context){
                var expandPalette = context && context.location === 'PaletteFunctions.ExpandLayout' ? false : true

                if ( ef.location === EditQml.QmlEditFragment.Object ){
                    if (control.compact)
                        root.control.expand({expandDefaultPalette: expandPalette})
                    else
                        root.control.addChildObject(ef)

                    root.control.__sortContainerChildren()
                } else {
                    var pc = root.control.addProperty(ef)
                    var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
                    if ( expandPalette )
                        paletteFunctions.openPaletteInPropertyContainer(pc, paletteFunctions.defaultPalette)
                }
            }
        }

//        property Component builderArea: null//Builder.BuilderArea{} //TODO
//        function initializeBuilderArea(target, items){
//            var result = builderArea.createObject(target)
//            result.anchors.fill = target
//            result.items = items
//            return result
//        }


        Item{
            id: objectContainerTitleWrap
            y: root.control.topSpacing
            height: objectContainerFrame.control.pane ? 0 : objectContainerFrame.control.titleHeight
            width: parent.width

            ObjectContainerTop{
                id: objectContainerTitle
                anchors.fill: parent
                compact: control.compact
                y: objectContainerFrame.control.topSpacing
                objectContainer: root
                color: {
                    return objectContainerFrame.control.pane ? objectContainerFrame.control.pane.topColor
                                                : objectContainerFrame.activeFocus
                                                  ? objectContainerFrame.control.theme.colorScheme.middlegroundOverlayDominant
                                                  : objectContainerFrame.control.theme.colorScheme.middlegroundOverlay
                }
                isBuilder: objectContainerFrame.control.editFragment ? objectContainerFrame.control.editFragment.isBuilder() : false

                onToggleCompact: {
                    if ( objectContainerFrame.control.compact ){
                        root.control.expand()
                        root.adjustSize()
                    } else {
                        root.control.collapse()
                        root.adjustSize()
                    }
                }
                onErase: {
                    try{
                        objectContainerFrame.control.paletteFunctions.eraseObject(root)
                    } catch ( e ){
                        var eunwrap = engine.unwrapError(e)
                        lk.layers.workspace.messages.pushError(eunwrap.message, eunwrap.code)
                    }
                }
                onToggleConnections: {
                    if ( paletteConnection.model ){
                        paletteConnection.model = null
                        paletteConnection.editFragment = null
                    } else {
                        paletteConnection.forceActiveFocus()
                        paletteConnection.model = objectContainerFrame.control.editor.code.language.bindingChannels
                        paletteConnection.editFragment = objectContainerFrame.control.editFragment
                    }
                }
                onAssignFocus: {
                    lk.layers.workspace.panes.activateItem(objectContainerFrame, objectContainerFrame.control.editor)
                }

                onRebuild : {
                    control.editFragment.rebuild()
                }
                onPaletteToPane : {
                    control.paletteFunctions.objectContainerToPane(root)
                }
                onClose : {
                    control.paletteFunctions.closeObjectContainer(root)
                }
                onAddPalette: {
                    var pane = container.parent
                    if ( container.pane )
                        pane = container.pane
                    while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
                        pane = pane.parent
                    }
                    var coords = container.mapToItem(pane, 0, 0)
                    coords.y -= 30
                    if ( container.pane )
                        coords.y -= 30 // if this container is in the title of a pane

                    var paletteList = root.control.paletteFunctions.views.openPaletetListBoxForContainer(
                        root,
                        root.control.paletteGroup,
                        Qt.rect(coords.x + objectContainerTitle.width - (180 / 2), coords.y, 30, 30),
                        PaletteFunctions.PaletteListMode.ObjectContainer
                    )
                    if (paletteList){
                        paletteList.width = 250
                        paletteList.x -= 70
                    }
                }
//                onToggleBuilder: {

//                    var ef = objectContainer.editFragment
//                    var items = []
//                    var ch = ef.getChildFragments()
//                    for ( var i = 0; i < ch.length; ++i ){
//                        var chef = ch[i]
//                        if ( chef.location === EditQml.QmlEditFragment.Object ){
//                            var ob = chef.readObject()
//                            if ( ob ){
//                                items.push({item: ob, data: chef})
//                            }
//                        }
//                    }

//                    vlog.i(items)
//                    var thisRoot = ef.readObject()

//                    var rs = ef.language.bindingChannels.selectedChannelRunnable().runSpace()
//                    var builderArea = objectContainerFrame.initializeBuilderArea(rs, items)
//                    builderArea.onItemsLayoutChanged.connect(function(changes){
////                        vlog.i(changes.items)

////                        for ( var j = 0; j < changes.items.length; ++j ){
////                            var changedItem = changes.items[j]
////                            // This will write to the editing fragment that has changed
////                            changedItem.data.writeProperties({
////                                'x' : 200
////                            })
////                        }
//                    })
//                }

                onCompose : {
                    objectContainerFrame.control.paletteFunctions.userAddToObjectContainer(root)
                }

                onCreateObject: {
                    var language = objectContainerFrame.control.editor.code.language
                    var editFragment = objectContainerFrame.control.editFragment
                    language.createObjectForProperty(editFragment)
                    language.createObjectInRuntime(editFragment)
                }
            }
        }

        PaletteConnection{
            id: paletteConnection
            visible: model ? true : false
            anchors.top: parent.top
            anchors.topMargin: 30
            width: parent.width
            onFocusChanged : if ( !focus ) model = null

            property var selectedHandler : function(){}
            property var cancelledHandler : function(index){}
        }


        Rectangle{
            width: container.width
            height: container.height + 5
            color: control.theme.colorScheme.middleground

            anchors.top: parent.top
            anchors.topMargin: objectContainerTitleWrap.height + control.topSpacing
            visible: !control.compact

            Column{
                id: container
                spacing: 5

                property alias sizeInfo: root.sizeInfo
                function measureSizeInfo(){ root.measureSizeInfo() }
            }
        }
    }
}
