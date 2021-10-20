import QtQuick 2.0
import live 1.0
import editor 1.0
import editor.private 1.0
import editqml 1.0 as EditQml

Item{
    id: root
    objectName: "objectContainer"

    property alias paletteListContainer: container
    property alias pane: objectContainerFrame.pane
    property var paletteFunctions: lk.layers.workspace.extensions.editqml.paletteFunctions

    //TODO: See if this should be private

    function recalculateContentWidth(){
        var max = 0
        for (var i=0; i<paletteListContainer.children.length; ++i)
        {
            var child = paletteListContainer.children[i]
            if (child.objectName === "objectContainer"){ // objectContainer
                if (child.contentWidth + 30 > max) max = child.contentWidth + 30
            } else if (child.objectName === "propertyContainer" && child.isAnObject){ // propertyContainer
                if (child.childObjectContainer &&
                    child.childObjectContainer.contentWidth + 140 > max)
                    max = child.childObjectContainer.contentWidth + 140
            } else {
                if (child.width > max) // paletteGroup
                    max = child.width
            }
        }

        if (max < 300)
            max = 300

        if (max !== containerContentWidth){
            containerContentWidth = max
        }
    }

    // interface functions

    function __initialize(editor, ef){
        root.editor = editor
        root.editFragment = ef
        root.title = ef.typeName() + (ef.objectId() ? ("#" + ef.objectId()) : "")
        ef.visualParent = root

        var paletteBoxGroup = paletteFunctions.__factories.createPaletteGroup(root.paletteListContainer, ef)

        paletteBoxGroup.leftPadding = 7
        paletteBoxGroup.topPadding = 7
        objectContainerFrame.paletteGroup = paletteBoxGroup
    }

    function addProperty(ef){
        var pc = propertyByName(ef.identifier())
        if ( pc )
            return pc

        var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
        var propertyContainer = paletteFunctions.__factories.createPropertyContainer(root.editor, ef, root.paletteListContainer)

        ef.incrementRefCount()

        root.propertiesOpened.push(ef.identifier())
        root.sortChildren()

        return propertyContainer
    }

    function addFunctionProperty(name){
        return null
    }

    function propertyByName(name){
        for (var i = 1; i < container.children.length; ++i){
            if (!container.children[i])
                continue
            if (container.children[i].objectName === "propertyContainer"){
                if ( container.children[i].editFragment.identifier() === name )
                    return container.children[i]
            }
        }
        return null
    }

    function addChildObject(ef){
        var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions

        var childObjectContainer = paletteFunctions.__factories.createObjectContainer(root.editor, ef, root.paletteListContainer)
        childObjectContainer.parentObjectContainer = root
        childObjectContainer.x = 20
        childObjectContainer.y = 10

        ef.incrementRefCount()

        return childObjectContainer
    }

    function paletteByName(name){
        for ( var i = 0; i < objectContainerFrame.paletteGroup.children.length; ++i ){
            if ( objectContainerFrame.paletteGroup.children[i].name === name )
                return objectContainerFrame.paletteGroup.children[i]
        }
        return null
    }

    function paletteGroup(){
        return objectContainerFrame.paletteGroup
    }

    function getPane(){
        var pane = root.parent
        while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
            pane = pane.parent
        }
        return pane
    }

    function getLayoutState(){
        return { isCollapsed: root.compact }
    }

    property bool supportsFunctions: false

    onContentWidthChanged: {
        if (parentObjectContainer){
            parentObjectContainer.recalculateContentWidth()
        }
    }

    property double containerContentWidth : 0
    property double editorContentWidth: editor && !parentObjectContainer ? editor.width - editor.lineSurfaceWidth - 50 : 0

    property alias editFragment : objectContainerFrame.editFragment
    property alias editor : objectContainerFrame.editor
    property alias title : objectContainerFrame.title
    property alias titleHeight : objectContainerFrame.titleHeight
    property alias compact: objectContainerFrame.compact
    property alias topSpacing: objectContainerFrame.topSpacing
    property alias propertiesOpened: objectContainerFrame.propertiesOpened
    property var sortChildren: paletteListContainer.sortChildren

    property var parentObjectContainer: null
    property var isForProperty: false
    property QtObject theme: lk.layers.workspace.themes.current

    width: objectContainerFrame.width
    height: objectContainerFrame.pane ? 30 : objectContainerFrame.height

    property int contentWidth: containerContentWidth

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
            text: objectContainerFrame.title
            clip: true
            elide: Text.ElideRight
            color: '#82909b'
        }
    }

    function expand(options){
        objectContainerFrame.expand(options ? options : undefined)
    }

    function collapse(){
        objectContainerFrame.collapse()
    }

    function __cleanExpandedMembers(){
        editFragment.removePalettes()

        var pg = objectContainerFrame.paletteGroup
        for (var xi = 0; xi < pg.children.length; ++xi)
            if (pg.children[xi].objectName === "paletteContainer")
                pg.children[xi].destroy()
        pg.children = []
        pg.palettesOpened = []

        for (var pi = 0; pi < paletteListContainer.children.length; ++pi){
            var child = paletteListContainer.children[pi]
            var ef = null
            if (child.objectName === "propertyContainer"){
                ef = child.editFragment
                child.clean().destroy()
            } else if (child.objectName === "objectContainer"){
                ef = child.editFragment
                child.clean().destroy()
            }
            if ( ef )
                editor.code.language.removeConnection(ef)
        }
    }

    function clean(){
        __cleanExpandedMembers()
        objectContainerFrame.paletteGroup.destroy()
        return root
    }

    function destroyObjectContainer(oc){
        oc.clean()
        oc.destroy()
    }

    Item{
        id: objectContainerFrame
        objectName: "objectContainerFrame"

        property string title: "Object"

        property Item paletteGroup : null
        property alias groupsContainer: container
        property QtObject editFragment : null
        property Item editor: null

        property alias objectContainerTitle: objectContainerTitle

        property Item pane : null
        property Item wrapper : root

        property int titleHeight: 30
        property bool compact: true

        property int topSpacing: 0

        property var propertiesOpened: []


        Keys.onPressed: {
            var command = lk.layers.workspace.keymap.locateCommand(event.key, event.modifiers)
            if ( command ){
                lk.layers.workspace.commands.execute(command)
            }
            event.accepted = true
        }

        width: container.width < 260 ? 300 : container.width
        height: compact ? 30 : objectContainerTitleWrap.height + topSpacing + container.height + 3

        function closeAsPane(){
            objectContainerTitle.parent = objectContainerTitleWrap
            objectContainerFrame.parent = objectContainerFrame.wrapper
            objectContainerFrame.pane.removePane()
            objectContainerFrame.pane = null
            root.placeHolder.parent = null
        }

        function expand(options){
            if (!compact)
                return

            compact = false

            var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions

            var objects = root.editor.code.language.openNestedFragments(root.editFragment, ['objects'])

            for (var i=0; i < objects.length; ++i){
                if ( !objects[i].visualParent ){
                    root.addChildObject(objects[i])
                }
            }

            var expandDefaultPalette = options && options.expandDefaultPalette ? options.expandDefaultPalette : true
            if ( expandDefaultPalette )
                paletteFunctions.openPaletteInObjectContainer(root, paletteFunctions.defaultPalette)

            var codeHandler = root.editor.code.language

            var properties = codeHandler.openNestedFragments(root.editFragment, ['properties'])

            for (var i = 0; i < properties.length; ++i){
                if ( !properties[i].visualParent ){
                    var pc = addProperty(properties[i])

                    if ( expandDefaultPalette )
                        paletteFunctions.openPaletteInPropertyContainer(pc, paletteFunctions.defaultPalette)
                }
            }

            var id = editFragment.objectId()
            var check = (objectContainerFrame.title.indexOf('#') === -1)
            if (id && check)
                objectContainerFrame.title = objectContainerFrame.title + "#" + id

            container.sortChildren()
        }

        function collapse(){
            if (compact)
                return
            if ( root.editFragment.fragmentType() & EditQml.QmlEditFragment.Group ){
                compact = true
                return
            }
            root.__cleanExpandedMembers()

            compact = true
            propertiesOpened.length = 0
        }

        property Connections editFragmentRemovals: Connections{
            target: editFragment
            function onConnectionChanged(){
                objectContainerTitle.isBuilder = root.editFragment.isBuilder()
            }
            function onAboutToBeRemoved(){
                if (isForProperty)
                    return
                var p = root.parent

                //TODO: Move to LanguageQmlHandler
                var language = editFragment.language
                var rootPosition = language.findRootPosition()

                if (editFragment.position() === rootPosition)
                    editFragment.language.rootFragment = null

                if (!p)
                    return

                if ( p.objectName === 'editorBox' ){ // if this is root for the editor box
                    destroyObjectContainer(root)
                    p.destroy()
                } else { // if this is nested
                    //TODO: Check if this is nested within a property container
                    if ( objectContainerFrame.pane )
                        objectContainerFrame.closeAsPane()
                    destroyObjectContainer(root)
                }
            }
        }

        property Connections addFragmentToContainerConn: Connections{
            target: editFragment
            ignoreUnknownSignals: true
            function onChildAdded(ef, context){
                var expandPalette = context && context.location === 'PaletteFunctions.ExpandLayout' ? false : true

                if ( ef.location === EditQml.QmlEditFragment.Object ){
                    if (compact)
                        expand({expandDefaultPalette: expandPalette})
                    else
                        addChildObject(ef)

                    container.sortChildren()
                } else {
                    var pc = root.addProperty(ef)
                    var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
                    if ( expandPalette )
                        paletteFunctions.openPaletteInPropertyContainer(pc, paletteFunctions.defaultPalette)
                }
            }
        }

        Item{
            id: objectContainerTitleWrap
            y: topSpacing
            height: objectContainerFrame.pane ? 0 : titleHeight
            width: parent.width

            ObjectContainerTop{
                id: objectContainerTitle
                anchors.fill: parent
                compact: objectContainerFrame.compact
                objectContainer: root
                color: {
                    return objectContainerFrame.pane ? objectContainerFrame.pane.topColor
                                                : objectContainerFrame.activeFocus
                                                  ? theme.colorScheme.middlegroundOverlayDominant
                                                  : theme.colorScheme.middlegroundOverlay
                }
                isBuilder: root.editFragment ? root.editFragment.isBuilder() : false

                onToggleCompact: {
                    if ( objectContainerFrame.compact )
                        expand()
                    else
                        collapse()
                }
                onErase: {
                    try{
                        paletteFunctions.eraseObject(root)
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
                        paletteConnection.model = editor.code.language.bindingChannels
                        paletteConnection.editFragment = editFragment
                    }
                }
                onAssignFocus: {
                    lk.layers.workspace.panes.activateItem(objectContainerFrame, objectContainerFrame.editor)
                }

                onRebuild : {
                    editFragment.rebuild()
                }
                onPaletteToPane : {
                    paletteFunctions.objectContainerToPane(root)
                }
                onClose : {
                    paletteFunctions.closeObjectContainer(objectContainerFrame)
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

                    var paletteList = paletteFunctions.views.openPaletetListBoxForContainer(
                        root,
                        paletteGroup(),
                        Qt.rect(coords.x + objectContainerTitle.width - (180 / 2), coords.y, 30, 30),
                        PaletteFunctions.PaletteListMode.ObjectContainer
                    )
                    if (paletteList){
                        paletteList.width = 250
                        paletteList.x -= 70
                    }
                }
                onCompose : {
                    paletteFunctions.userAddToObjectContainer(root)
                }

                onCreateObject: {
                    var codeHandler = objectContainerFrame.editor.code.language
                    codeHandler.createObjectForProperty(editFragment)
                    codeHandler.createObjectInRuntime(editFragment)
                }
            }
        }

        PaletteConnection{
            id: paletteConnection
            visible: model ? true : false
            anchors.top: parent.top
            anchors.topMargin: 25
            width: parent.width
            onFocusChanged : if ( !focus ) model = null

            property var selectedHandler : function(){}
            property var cancelledHandler : function(index){}
        }


        Rectangle {
            width: container.width
            height: container.height + 5
            color: theme.colorScheme.middleground

            anchors.top: parent.top
            anchors.topMargin: objectContainerTitleWrap.height + topSpacing
            visible: !compact

            Column{
                id: container

                spacing: 5
                width: parentObjectContainer ? parentObjectContainer.width - (isForProperty? 140 : 20) : contentWidth + 20

                onChildrenChanged: recalculateContentWidth()

                property double heightSwap: -1

                height: {
                    if (compact)
                        return 0
                    if ( heightSwap !== -1 )
                        return heightSwap
                    var totalHeight = 0;
                    if ( children.length > 0 ){
                        for ( var i = 0; i < children.length; ++i ){
                            totalHeight += children[i].height
                        }
                    }
                    if ( children.length > 1 )
                        return totalHeight + (children.length - 1) * spacing
                    else
                        return totalHeight
                }

                function sortChildren(){
                    if (!objectContainerFrame.parent)
                        return
                    if (children.length === 0)
                        return

                    var childrenCopy = []
                    childrenCopy.push(children[0])
                    children[0].z = children.length

                    for (var i=1; i<children.length; ++i)
                    {
                        if (!children[i]) continue
                        if (children[i].objectName === "propertyContainer")
                        {
                            childrenCopy.push(children[i])
                            children[i].z = children.length - childrenCopy.length
                        }
                    }

                    for (var i=1; i<children.length; ++i)
                    {
                        if (!children[i]) continue
                        if (children[i].objectName === "objectContainer"){
                            children[i].topSpacing = 5
                            children[i].z = children.length - childrenCopy.length
                            childrenCopy.push(children[i])
                        }
                    }

                    heightSwap = height
                    children = childrenCopy
                    heightSwap = -1
                }

            }
        }



    }
}
