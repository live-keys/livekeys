import QtQuick 2.0
import live 1.0
import editor 1.0
import editor.private 1.0

Item{
    id: root
    objectName: "objectContainer"

    property alias paletteGroup: objectContainer.paletteGroup
    property alias groupsContainer: container

    function recalculateContentWidth(){
        var max = 0
        for (var i=0; i<groupsContainer.children.length; ++i)
        {
            var child = groupsContainer.children[i]
            if (child.objectName === "objectContainer"){
                if (child.contentWidth + 20 > max)
                    max = child.contentWidth + 20
            } else if (child.objectName === "propertyContainer" && child.isAnObject){
                if (child.childObjectContainer &&
                    child.childObjectContainer.contentWidth + 140 > max)
                    max = child.childObjectContainer.contentWidth + 140
            } else {
                if (child.width + 20 > max)
                    max = child.width + 20
            }
        }

        if (max < 300)
            max = 300

        if (max !== contentWidth){
            contentWidth = max
        }
    }

    onContentWidthChanged: {
        if (parentObjectContainer){
            parentObjectContainer.recalculateContentWidth()
        }
    }


    property alias editingFragment : objectContainer.editingFragment
    property alias editor : objectContainer.editor
    property alias title : objectContainer.title
    property alias titleHeight : objectContainer.titleHeight
    property alias compact: objectContainer.compact
    property alias topSpacing: objectContainer.topSpacing
    property alias propertiesOpened: objectContainer.propertiesOpened

    property var parentObjectContainer: null
    property var isForProperty: false
    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls

    width: objectContainer.width
    height: objectContainer.pane ? 30 : objectContainer.height

    property int contentWidth: 0

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



    Item{
        id: objectContainer

        property string title: "Object"

        property Item paletteGroup : null
        property alias groupsContainer: container
        property QtObject editingFragment : null
        property Item editor: null

        property Item pane : null
        property Item wrapper : root

        property int titleHeight: 30
        property bool compact: true

        property int topSpacing: 0

        property var propertiesOpened: []

        property PaletteStyle paletteStyle: lk.layers.workspace.extensions.editqml.paletteStyle

        width: container.width < 260 ? 300 : container.width
        height: container.height < 10 || compact ? 40 : objectContainerTitleWrap.height + topSpacing + /*(paletteGroup ? paletteGroup.height : 0) +*/ container.height

        function closeAsPane(){
            objectContainerTitle.parent = objectContainerTitleWrap
            objectContainer.parent = objectContainer.wrapper
            objectContainer.pane.removePane()
            objectContainer.pane = null
            root.placeHolder.parent = null
        }

        function addObjectFragmentToContainer(ef){
            if (!ef) return

            var childObjectContainer =
                    paletteControls.addChildObjectContainer(root, ef)

            childObjectContainer.x = 20
            childObjectContainer.y = 10

            ef.incrementRefCount()
        }

        function addPropertyFragmentToContainer(ef){
            if (!ef) return

            paletteControls.addPropertyContainer(root, ef)
        }
        function expandOptions(options){
            var codeHandler = objectContainer.editor.documentHandler.codeHandler

            if ( 'properties' in options){
                var newProps = options['properties']
                for ( var i = 0; i < newProps.length; ++i ){

                    var propName = newProps[i][0]
                    var propType = codeHandler.propertyType(objectContainer.editingFragment, propName)

                    var propPalette = newProps[i].length > 1 ? newProps[i][1] : ''

                    if ( propType === '' ) continue

                    var ppos = codeHandler.addProperty(
                        objectContainer.editingFragment.valuePosition() + objectContainer.editingFragment.valueLength() - 1,
                        objectContainer.editingFragment.typeName(),
                        propType,
                        propName,
                        true
                    )

                    var ef = codeHandler.openNestedConnection(
                        objectContainer.editingFragment, ppos
                    )

                    if ( ef ){
                        paletteControls.addPropertyContainer(root, ef, propPalette)
                    }
                    else {
                        lk.layers.workspace.panes.focusPane('viewer').error.text += "<br>Error: Can't create a palette in a non-compiled program"
                        console.error("Error: Can't create a palette in a non-compiled program") // better message needed
                    }

                    container.sortChildren()
                }

                if (compact)
                    expand()
            }
        }

        function expand(){
            var objects = editor.documentHandler.codeHandler.openNestedObjects(editingFragment)
            for (var i=0; i < objects.length; ++i){
                addObjectFragmentToContainer(objects[i])
            }

            var properties = editor.documentHandler.codeHandler.openNestedProperties(editingFragment)

            for (var i=0; i < properties.length; ++i){
                addPropertyFragmentToContainer(properties[i])
            }

            var id = editingFragment.objectId()
            var check = (objectContainer.title.indexOf('#') === -1)
            if (id && check)
                objectContainer.title = objectContainer.title + "#" + id

            container.sortChildren()
            compact = false
        }

        function collapse(){
            for ( var i = 1; i < container.children.length; ++i ){
                var edit = container.children[i].editingFragment
                editor.documentHandler.codeHandler.removeConnection(edit)
            }

            for (var i=1; i < container.children.length; ++i)
                container.children[i].destroy()

            compact = true

            propertiesOpened = []
        }

        property Connections editingFragmentRemovals: Connections{
            target: editingFragment
            onConnectionChanged : {
                objectContainerTitle.isBuilder = root.editingFragment.isBuilder()
            }
            onAboutToBeRemoved : {
                var p = root.parent
                if (!p) return
                if ( p.objectName === 'editorBox' ){ // if this is root for the editor box
                    p.destroy()
                } else { // if this is nested
                    //TODO: Check if this is nested within a property container
                    if ( objectContainer.pane )
                        objectContainer.closeAsPane()
                    root.destroy()
                }
            }
        }

        property Connections addFragmentToContainerConn: Connections{
            target: editingFragment
            ignoreUnknownSignals: true
            onObjectAdded: {
                if (compact) expand()
                else objectContainer.addObjectFragmentToContainer(obj)
                container.sortChildren()
            }
            onPropertyAdded: {
                for (var i = 0; i < objectContainer.propertiesOpened.length; ++i){
                    if (objectContainer.propertiesOpened[i] === ef.identifier()){
                        if (compact) expand()
                        return
                    }
                }
                editor.documentHandler.codeHandler.populateNestedObjectsForFragment(editingFragment)

                if (compact) expand()
                else objectContainer.addPropertyFragmentToContainer(ef)
                container.sortChildren()
            }
        }

        Item{
            id: objectContainerTitleWrap
            y: topSpacing
            height: objectContainer.pane ? 0 : titleHeight
            width: parent.width + 10

            ObjectContainerTop{
                id: objectContainerTitle
                anchors.fill: parent
                compact: objectContainer.compact
                color: objectContainer.pane ? objectContainer.pane.topColor : objectContainer.paletteStyle.sectionHeaderBackgroundColor//'#062945'
                isBuilder: root.editingFragment ? root.editingFragment.isBuilder() : false

                onToggleCompact: {
                    if ( objectContainer.compact )
                        expand()
                    else
                        collapse()
                }
                onErase: {
                    editor.documentHandler.codeHandler.deleteObject(editingFragment)
                }
                onToggleConnections: {
                    if ( paletteConnection.model ){
                        paletteConnection.model = null
                    } else {
                        paletteConnection.forceActiveFocus()
                        paletteConnection.model = editingFragment.bindingModel(editor.documentHandler.codeHandler)
                    }
                }
                onRebuild : {
                    editingFragment.rebuild()
                }
                onPaletteToPane : {
                    if ( objectContainer.pane === null ){
                        var objectPane = lk.layers.workspace.panes.createPane('objectPalette', {}, [400, 400])
                        lk.layers.workspace.panes.splitPaneHorizontallyWith(
                            objectContainer.editor.parentSplitter,
                            objectContainer.editor.parentSplitterIndex(),
                            objectPane
                        )

                        objectContainerTitle.parent = objectPane.paneHeaderContent
                        objectPane.objectContainer = objectContainer
                        objectPane.title = objectContainer.title
                        objectContainer.pane = objectPane
                        root.placeHolder.parent = root
                    } else {
                        objectContainer.closeAsPane()
                    }
                }
                onClose : {
                    if ( objectContainer.pane )
                        objectContainer.closeAsPane()
                    var codeHandler = editor.documentHandler.codeHandler
                    collapse()
                    editor.documentHandler.codeHandler.removeConnection(editingFragment)

                    var rootPos = codeHandler.findRootPosition()
                    if (rootPos === editingFragment.position())
                        editor.editor.rootShaped = false

                    codeHandler.removeConnection(editingFragment)

                    var p = root.parent
                    if ( p.objectName === 'editorBox' ){ // if this is root for the editor box
                        p.destroy()
                    } else { // if this is nested
                        //TODO: Check if this is nested within a property container
                        if ( objectContainer.pane )
                            objectContainer.closeAsPane()
                        root.destroy()
                    }
                }
                onAddPalette: {
                    var editingFragment = objectContainer.editingFragment
                    if ( !editingFragment )
                        return

                    var palettes = editor.documentHandler.codeHandler.findPalettes(editingFragment.position(), true, true)
                    if (palettes.size() ){
                        var paletteList = paletteControls.createPaletteListView(objectContainer)
                        paletteList.forceActiveFocus()
                        paletteList.model = palettes
                        paletteList.width = 250
                        paletteList.anchors.topMargin = titleHeight + topSpacing
                        paletteList.cancelledHandler = function(){
                            paletteList.focus = false
                            paletteList.model = null
                            paletteList.destroy()
                        }
                        paletteList.selectedHandler = function(index){
                            paletteList.focus = false
                            paletteList.model = null

                            var palette = editor.documentHandler.codeHandler.openPalette(editingFragment, palettes, index)

                            var paletteBox = paletteControls.addPalette(palette,
                                                                        editingFragment,
                                                                        editor,
                                                                        paletteGroup,
                                                                        root)

                            if (paletteBox) paletteBox.moveEnabledSet = false
                            paletteList.destroy()

                        }
                    }
                }
                onCompose : {
                    paletteControls.compose(objectContainer, false)
                }
            }
        }

        PaletteConnection{
            id: paletteConnection
            visible: model ? true:false
            anchors.top: parent.top
            anchors.topMargin: 24
            width: parent.width
            color: "#0a141c"
            selectionColor: "#0d2639"
            fontSize: 10
            fontFamily: "Open Sans, sans-serif"
            onFocusChanged : if ( !focus ) model = null

            property var selectedHandler : function(){}
            property var cancelledHandler : function(index){}
        }


        Column{
            id: container

            anchors.top: parent.top
            anchors.topMargin: objectContainerTitleWrap.height + topSpacing
            visible: !compact
            spacing: 5
            width: parentObjectContainer ? parentObjectContainer.width - (isForProperty? 140 : 20) : contentWidth + 10

            onChildrenChanged: recalculateContentWidth()

            height: {
                if (compact) return 0
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

                if (!objectContainer.parent) return
                if (children.length === 0) return

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

                children = childrenCopy
            }
        }



    }
}
