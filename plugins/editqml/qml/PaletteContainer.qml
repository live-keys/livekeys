import QtQuick 2.3
import base 1.0
import live 1.0
import visual.shapes 1.0
import editor 1.0
import editor.private 1.0

Rectangle{
    id: paletteContainer

    width: pane ? minimumWidth + 100 : ( (child && child.width > minimumWidth ? child.width : minimumWidth) + headerWidth )
    height: pane
            ? normalHeaderHeight
            : (child
                ? child.height + (compact ? 0 : normalHeaderHeight) +5
                : 0)
    objectName: "paletteContainer"

    property QtObject theme: lk.layers.workspace.themes.current
    property var editFragment: null
    property double minimumWidth: compact ? 0 : 200
    property double headerWidth: compact ? compactHeaderWidth + 10 : 5
    property int compactHeaderWidth: isCompactVertical ? 20 : 35
    property int normalHeaderHeight: 35

    property bool compact: true
    property bool isBuilder : false

    property Item child : palette ? palette.item : null
    property QtObject palette : null

    property bool isCompactVertical: child && child.height > 48

    color: "black"

    property string name : palette ? palette.name : ''
    property string type : palette ? palette.type : ''
    property string title : type + ' - ' + name
    property var editor: null
    property Item pane: null
    property Item dragPane: null

    onEditFragmentChanged: {
        if (!editFragment)
            return
        editor = editFragment.codeHandler.documentHandler.textEdit().getEditor()
    }

    property bool paletteSwapVisible: false
    property bool paletteAddVisible: false

    property bool moveEnabledSet : true
    property bool moveEnabledGet: !compact && moveEnabledSet

    property double titleLeftMargin : 50
    property double titleRightMargin : 50

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls

    function swapOrAddPalette(swap){
        var pane = editor.parent
        while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
            pane = pane.parent
        }

        var coords = paletteContainer.mapToItem(pane, 0, 0)
        coords.y -= 35;

        var paletteList = paletteControls.views.openPaletetListBoxForContainer(
            paletteContainer,
            paletteContainer.parent,
            Qt.rect(coords.x - 180 / 2, coords.y, 30, 30),
            PaletteControls.PaletteListMode.PaletteContainer,
            swap)

        if (paletteList){
            paletteList.width = Qt.binding(function() { return paletteContainer.width })
            paletteList.x -= 70
        }
    }

    function viewPaletteConnections(){
        if ( !editFragment )
            return

        if ( paletteConnection.model ){
            paletteConnection.model = null
            paletteConnection.editFragment = null
        } else {
            paletteConnection.forceActiveFocus()
            paletteConnection.model = editFragment.codeHandler.bindingChannels
            paletteConnection.editFragment = editFragment
        }
    }

    function rebuild(){
        var editFragment = paletteContainer.parent.editFragment
        if ( !editFragment )
            return

        editFragment.rebuild()
    }

    function closeAsPane(){
        paletteContainer.pane.paletteContainer.parent = paletteContainer
        paletteContainer.pane.removePane()
        paletteContainer.pane = null
        paletteChild.y = 35
    }

    function paletteToPane(){
        if ( paletteContainer.pane ){
            paletteContainer.closeAsPane()
            return
        }

        var palettePane = lk.layers.workspace.panes.createPane('palette', {}, [400, 400])
        lk.layers.workspace.panes.splitPaneHorizontallyWith(
            paletteContainer.editor.parent.parentSplitView,
            paletteContainer.editor.parent.parentSplitViewIndex(),
            palettePane
        )

        paletteContainer.pane = palettePane

        palettePane.paletteContainer = paletteChild
        palettePane.title = paletteContainer.title
        paletteChild.y = 0
    }

    function closePalette(){
        if (paletteContainer.palette.type === "qml/import"){
            paletteContainer.palette.item.editor.importsShaped = false
        }
        var p = parent
        while (p && p.objectName !== "paletteGroup"){
            p = p.parent
        }
        p.palettesOpened = p.palettesOpened.filter(function(name){ return name !== paletteContainer.palette.name })
        editFragment.codeHandler.removePalette(paletteContainer.palette)
    }

    Component.onCompleted: {
        var paletteGroup = paletteContainer.parent

        if ( paletteGroup.editFragment) {
            paletteContainer.editFragment = paletteGroup.editFragment
            paletteContainer.isBuilder = paletteGroup.editFragment.isBuilder()
            paletteGroup.editFragment.connectionChanged.connect(function(){
                paletteContainer.isBuilder = paletteGroup.editFragment.isBuilder()
            })
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: paletteContainer.child.forceActiveFocus()
    }

    property Component paletteContainerHeader: PaletteContainerHeader{}
    property Component paletteContainerSide: PaletteContainerSide{}
    property Component paletteContainerPlaceHolder: PalettePlaceHolder{}

    Loader{
        id: headerLoader

        anchors.top: parent.top
        anchors.topMargin: compact && child ? 3 : 0
        anchors.left: parent.left
        anchors.leftMargin: compact && child ? child.width + 7: 0

        height: compact && child
                    ? child.height
                    : normalHeaderHeight
        width: !compact && child
                    ? (paletteContainer.parent ? paletteContainer.parent.width : paletteContainer.width)
                    : compactHeaderWidth
        clip: true

        sourceComponent: {
            paletteContainer.pane
                ? paletteContainer.paletteContainerPlaceHolder
                : ( paletteContainer.compact
                    ? paletteContainer.paletteContainerSide
                    : paletteContainer.paletteContainerHeader )
        }
        onItemChanged: {
            if ( paletteContainer.pane ){
                item.color = paletteContainer.theme.colorScheme.middleground
                item.title = paletteContainer.title
            } else if ( compact ){
                item.color = paletteContainer.theme.colorScheme.middleground
                item.iconColor = paletteContainer.theme.colorScheme.topIconColor
                item.isVertical = Qt.binding(function(){
                    return child && child.height > 48
                })

                item.expand.connect(function(){
                    paletteContainer.compact = false
                })
                item.close.connect(function(){
                    paletteContainer.closePalette()
                })
            } else {
                item.color = paletteContainer.theme.colorScheme.middleground
                item.iconColor = paletteContainer.theme.colorScheme.topIconColor
                item.title = paletteContainer.title

                item.canSwapPalette = paletteContainer.moveEnabledSet
                item.canAddPalette = paletteContainer.moveEnabledSet
                item.canRebuild = paletteContainer.isBuilder
                item.canViewConnections = paletteContainer.parent && paletteContainer.parent.parent && paletteContainer.parent.parent.objectName === "editorBox"
                item.canMoveToNewPane = paletteContainer.parent && paletteContainer.parent.parent && paletteContainer.parent.parent.objectName !== "editorBox"
                item.enableMove = paletteContainer.moveEnabledSet

                item.handleMovePress = function(mouse){
                    paletteContainer.parent.parent.disableMoveBehavior()
                }
                item.handleMovePositionChanged = function(mouse, currentMousePos,lastMousePos){
                    var deltaX = currentMousePos.x - lastMousePos.x
                    var deltaY = currentMousePos.y - lastMousePos.y
                    paletteContainer.parent.parent.x += deltaX
                    paletteContainer.parent.parent.y += deltaY
                }
                item.collapse.connect(function(){
                    paletteContainer.compact = true
                })
                item.close.connect(function(){
                    paletteContainer.closePalette()
                })
                item.swapPalette.connect(function(){
                    paletteContainer.swapOrAddPalette(PaletteControls.PaletteListSwap.Swap)
                })
                item.addPalette.connect(function(){
                    paletteContainer.swapOrAddPalette(PaletteControls.PaletteListSwap.NoSwap)
                })
                item.viewConnections.connect(function(){
                    paletteContainer.viewPaletteConnections()
                })
                item.rebuild.connect(function(){
                    paletteContainer.rebuild()
                })
                item.moveToNewPane.connect(function(){
                    paletteContainer.paletteToPane()
                })
                item.dragToNewPaneStarted.connect(function(){
                    lk.layers.workspace.panes.__dragStarted(function(){
                        var palettePane = lk.layers.workspace.panes.createPane('palette', {}, [400, 400])
                        paletteContainer.dragPane = palettePane
                        return palettePane
                    })
                })
                item.dragToNewPaneFinished.connect(function(){
                    lk.layers.workspace.panes.__dragFinished()

                    var palettePane = paletteContainer.dragPane
                    paletteContainer.dragPane = null

                    paletteContainer.pane = palettePane

                    palettePane.paletteContainer = paletteChild
                    palettePane.title = paletteContainer.title
                    paletteChild.y = 0
                })
            }
        }
    }

    Item{
        id: paletteChild
        anchors.top: paletteContainer.pane
                    ? undefined
                    : ( compact ? parent.top : headerLoader.bottom )
        anchors.left: parent.left
        width: paletteContainer.child ? paletteContainer.child.width: 0
        height: paletteContainer.child ? paletteContainer.child.height: 0
        children: paletteContainer.child ? [paletteContainer.child] : []

        function closeAsPane(){
            paletteContainer.closeAsPane()
        }
    }


    PaletteConnection{
        id: paletteConnection
        visible: model ? true : false
        anchors.top: parent.top
        anchors.topMargin: 24
        width: parent.width
        onFocusChanged : if ( !focus ) model = null

        property var selectedHandler : function(){}
        property var cancelledHandler : function(index){}
    }
}
