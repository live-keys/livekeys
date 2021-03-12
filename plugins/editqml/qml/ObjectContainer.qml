import QtQuick 2.0
import live 1.0
import editor 1.0
import editor.private 1.0

Item{
    id: root
    objectName: "objectContainer"

    property alias paletteGroup: objectContainer.paletteGroup
    property alias groupsContainer: container
    property alias pane: objectContainer.pane

    function recalculateContentWidth(){
        var max = 0
        for (var i=0; i<groupsContainer.children.length; ++i)
        {
            var child = groupsContainer.children[i]
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

    onContentWidthChanged: {
        if (parentObjectContainer){
            parentObjectContainer.recalculateContentWidth()
        }
    }

    property double containerContentWidth : 0
    property double editorContentWidth: editor && !parentObjectContainer ? editor.width - editor.editor.lineSurfaceWidth - 50 : 0

    property alias editingFragment : objectContainer.editingFragment
    property alias editor : objectContainer.editor
    property alias title : objectContainer.title
    property alias titleHeight : objectContainer.titleHeight
    property alias compact: objectContainer.compact
    property alias topSpacing: objectContainer.topSpacing
    property alias propertiesOpened: objectContainer.propertiesOpened
    property var sortChildren: groupsContainer.sortChildren

    property var parentObjectContainer: null
    property var isForProperty: false
    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls
    property QtObject theme: lk.layers.workspace.themes.current

    width: objectContainer.width
    height: objectContainer.pane ? 30 : objectContainer.height

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
            text: objectContainer.title
            clip: true
            elide: Text.ElideRight
            color: '#82909b'
        }
    }

    function expand(){
        objectContainer.expand()
    }

    function collapse(){
        objectContainer.collapse()
    }

    function expandOptions(options){
        objectContainer.expandOptions(options)
    }

    function addObjectFragmentToContainer(ef){
        if (!ef) return

        var childObjectContainer =
                paletteControls.addChildObjectContainer(root, ef)

        childObjectContainer.x = 20
        childObjectContainer.y = 10

        ef.incrementRefCount()

        return childObjectContainer
    }

    function addPropertyFragmentToContainer(ef, expandDefault){
        if (!ef) return

        paletteControls.addPropertyContainer(root, ef, expandDefault)
    }

    function destroyObjectContainer(oc){
        for (var pi = 0; pi < oc.groupsContainer.children.length; ++pi){
            var child = oc.groupsContainer.children[pi]
            if (child.objectName === "paletteGroup"){
                var pg = child
                for (var xi = 0; xi < pg.children.length; ++xi)
                    if (pg.children[xi].objectName === "paletteContainer")
                        pg.children[xi].destroy()
                pg.destroy()
            }
            if (child.objectName === "propertyContainer"){
                var pc = child
                if (pc.valueContainer.objectName === "paletteGroup"){
                    var pg = pc.valueContainer
                    for (var xi = 0; xi < pg.children.length; ++xi)
                        if (pg.children[xi].objectName === "paletteContainer")
                            pg.children[xi].destroy()
                    pg.destroy()
                }
                if (pc.valueContainer.objectName === "objectContainer"){
                    oc.destroyObjectContainer(pc.valueContainer)
                }

                pc.destroy()
            }
        }
        oc.destroy()
    }

    Item{
        id: objectContainer
        objectName: "objectContainerFrame"

        property string title: "Object"

        property Item paletteGroup : null
        property alias groupsContainer: container
        property QtObject editingFragment : null
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
            objectContainer.parent = objectContainer.wrapper
            objectContainer.pane.removePane()
            objectContainer.pane = null
            root.placeHolder.parent = null
        }

        function expandOptions(options){
            var codeHandler = objectContainer.editor.documentHandler.codeHandler

            if ( 'palettes' in options){
                var palettes = options['palettes']
                for ( var i = 0; i < palettes.length; ++i){
                    if (paletteGroup.palettesOpened.indexOf(palettes[i]) !== -1) continue
                    paletteControls.openPaletteByName(palettes[i], objectContainer.editingFragment, editor, paletteGroup)
                }
            }

            if ( 'properties' in options){
                var newProps = options['properties']
                for ( var i = 0; i < newProps.length; ++i ){

                    var propName = newProps[i][0]
                    var propType = codeHandler.propertyType(objectContainer.editingFragment, propName)

                    var propPalette = newProps[i].length > 1 ? newProps[i][1] : ''

                    if ( propType === '' ) continue


                    var ef = null
                    if (newProps[i].length > 2)
                    {
                        ef = codeHandler.createReadOnlyPropertyFragment(root.editingFragment, propName)
                    } else {

                        var ppos = codeHandler.addProperty(
                            root.editingFragment.valuePosition() + root.editingFragment.valueLength() - 1,
                            root.editingFragment.typeName(),
                            propType,
                            propName,
                            true
                        )
                        ef = codeHandler.openNestedConnection(
                            root.editingFragment, ppos
                        )
                    }

                    if (ef) {
                        objectContainer.editingFragment.signalPropertyAdded(ef, false)
                        if (propPalette.length === 0) continue

                        for (var j = 0; j < container.children.length; ++j)
                        {
                            var child = container.children[j]
                            if (child.objectName !== "propertyContainer") continue
                            if (child.title !== propName) continue

                            if (child.valueContainer.palettesOpened.indexOf(propPalette) !== -1) break
                            paletteControls.openPaletteByName(propPalette, ef, editor, child.valueContainer)
                            break
                        }

                    } else {
                        lk.layers.workspace.messages.pushError("ObjectContainer: Can't open declared palette for property " + propName, 1)
                    }

                    container.sortChildren()
                }

                if (compact)
                    expand()
            }
        }

        function expand(){
            if (!compact) return
            compact = false
            if (paletteControls.instructionsShaping) return
            paletteControls.openEmptyNestedObjects(root)
            // paletteControls.openDefaults(root)

            var id = editingFragment.objectId()
            var check = (objectContainer.title.indexOf('#') === -1)
            if (id && check)
                objectContainer.title = objectContainer.title + "#" + id

            container.sortChildren()
        }

        function collapse(){
            if (compact) return
            for ( var i = 1; i < container.children.length; ++i ){
                var edit = container.children[i].editingFragment
                editor.documentHandler.codeHandler.removeConnection(edit)
            }

            for (var i=1; i < container.children.length; ++i)
                container.children[i].destroy()

            compact = true

            propertiesOpened.length = 0
        }

        property Connections editingFragmentRemovals: Connections{
            target: editingFragment
            function onConnectionChanged(){
                objectContainerTitle.isBuilder = root.editingFragment.isBuilder()
            }
            function onAboutToBeRemoved(){
                if (isForProperty) return
                var p = root.parent

                if (editingFragment.position() === rootPosition)
                    editor.editor.rootShaped = false

                if (!p) return
                if ( p.objectName === 'editorBox' ){ // if this is root for the editor box
                    destroyObjectContainer(root)
                    p.destroy()
                } else { // if this is nested
                    //TODO: Check if this is nested within a property container
                    if ( objectContainer.pane )
                        objectContainer.closeAsPane()
                    destroyObjectContainer(root)
                }
            }
        }

        property Connections addFragmentToContainerConn: Connections{
            target: editingFragment
            ignoreUnknownSignals: true
            function onObjectAdded(obj, cursorCoords){
                if (compact)
                    expand()
                else
                    addObjectFragmentToContainer(obj)

                var child = container.children[container.children.length-1]
                var codeHandler = objectContainer.editor.documentHandler.codeHandler
                var id = child.editingFragment.objectId()
                child.title = child.editingFragment.typeName() + (id ? "#"+id : "")

                // paletteControls.openDefaultPalette(child.editingFragment, editor, child.paletteGroup, child)

                container.sortChildren()
            }
            function onPropertyAdded(ef, expandDefault){
                for (var i = 0; i < objectContainer.propertiesOpened.length; ++i){
                    if (objectContainer.propertiesOpened[i] === ef.identifier()){
                        if (compact) expand()
                        return
                    }
                }
                editor.documentHandler.codeHandler.populateNestedObjectsForFragment(editingFragment)

                if (compact) expand()
                addPropertyFragmentToContainer(ef, expandDefault)
                container.sortChildren()
            }
        }

        Item{
            id: objectContainerTitleWrap
            y: topSpacing
            height: objectContainer.pane ? 0 : titleHeight
            width: parent.width

            ObjectContainerTop{
                id: objectContainerTitle
                anchors.fill: parent
                compact: objectContainer.compact
                objectContainer: root
                color: {
                    return objectContainer.pane ? objectContainer.pane.topColor
                                                : objectContainer.activeFocus
                                                  ? theme.colorScheme.middlegroundOverlayDominant
                                                  : theme.colorScheme.middlegroundOverlay
                }
                isBuilder: root.editingFragment ? root.editingFragment.isBuilder() : false

                onToggleCompact: {
                    if ( objectContainer.compact )
                        expand()
                    else
                        collapse()
                }
                onErase: {
                    try{
                        paletteControls.eraseObject(root)
                    } catch ( e ){
                        var eunwrap = lk.engine.unwrapError(e)
                        lk.layers.workspace.messages.pushError(eunwrap.message, eunwrap.code)
                    }
                }
                onToggleConnections: {
                    if ( paletteConnection.model ){
                        paletteConnection.model = null
                        paletteConnection.editingFragment = null
                    } else {
                        paletteConnection.forceActiveFocus()
                        paletteConnection.model = editor.documentHandler.codeHandler.bindingChannels
                        paletteConnection.editingFragment = editingFragment
                    }
                }
                onAssignFocus: {
                    lk.layers.workspace.panes.activateItem(objectContainer, objectContainer.editor)
                }

                onRebuild : {
                    editingFragment.rebuild()
                }
                onPaletteToPane : {
                    paletteControls.paletteToPane(objectContainer)
                }
                onClose : {
                    paletteControls.closeObjectContainer(objectContainer)
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

                    var paletteList = paletteControls.addPaletteList(
                        objectContainer,
                        paletteGroup,
                        Qt.rect(coords.x + objectContainerTitle.width - (180 / 2), coords.y, 30, 30),
                        PaletteControls.PaletteListMode.ObjectContainer
                    )
                    if (paletteList){
                        paletteList.width = 250
                        paletteList.x -= 70
                    }
                }
                onCompose : {
                    paletteControls.compose(objectContainer, false)
                }

                onCreateObject: {
                    var codeHandler = objectContainer.editor.documentHandler.codeHandler
                    codeHandler.addObjectForProperty(editingFragment)
                    codeHandler.addItemToRuntime(editingFragment)
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

                    if (!objectContainer.parent)
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
