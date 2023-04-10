import QtQuick 2.3
import base 1.0
import visual.shapes 1.0
import editor 1.0
import editor.private 1.0

Rectangle{
    id: paletteContainer

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
    // sizeInfo is propagated from the palette to the root, the root then decides how to update
    // itself and its children downwards
    property var sizeInfo: calculateChildSizeInfo(child)
    onChildChanged: {
        sizeInfo = calculateChildSizeInfo(child)
        if ( parent && parent.measureSizeInfo )
            parent.measureSizeInfo()

        if ( child && child.hasOwnProperty('sizeInfo') ){
            child.onSizeInfoChanged.connect(function(){
                sizeInfo = calculateChildSizeInfo(child)
                if ( parent && parent.measureSizeInfo )
                    parent.measureSizeInfo()
            })
        }
    }

    property Item pane: null
    property Item dragPane: null

    onEditFragmentChanged: {
        if (!editFragment)
            return
        editor = editFragment.language.code.textEdit().getEditor()
    }

    property bool paletteSwapVisible: false
    property bool paletteAddVisible: false

    property bool moveEnabledSet : true
    property bool moveEnabledGet: !compact && moveEnabledSet

    property double titleLeftMargin : 50
    property double titleRightMargin : 50

    property var paletteFunctions: lk.layers.workspace.extensions.editqml.paletteFunctions

    function adjustSize(){
        if ( child ){
            var contentWidth = paletteContainer.parent.sizeInfo.contentWidth
            if ( sizeInfo.maxWidth >= 0 && sizeInfo.maxWidth < contentWidth){
                contentWidth = sizeInfo.maxWidth
            }
            paletteContainer.width = contentWidth
            if ( paletteContainer.pane ){
                paletteContainer.height = 30

                var childWidth = paletteContainer.pane.width - 20
                if ( childWidth < sizeInfo.minWidth ){
                    childWidth = sizeInfo.minWidth
                } else if ( sizeInfo.maxWidth > 0 && childWidth > sizeInfo.maxWidth ){
                    childWidth = sizeInfo.maxWidth
                }

                var childHeight = paletteContainer.pane.height - 20
                if ( childHeight < sizeInfo.minHeight ){
                    childHeight = sizeInfo.minHeight
                } else if ( sizeInfo.maxHeight > 0 && childHeight > sizeInfo.maxHeight ){
                    childHeight = sizeInfo.maxHeight
                }

                child.width = childWidth
                child.height = childHeight

            } else {
                paletteContainer.height = sizeInfo.minHeight

                var childWidth = contentWidth - headerWidth
                child.width = childWidth
                child.height = sizeInfo.minHeight - (compact ? 0 : 30)
            }
        }
    }

    function calculateChildSizeInfo(child){
        var info = child && child.sizeInfo ? child.sizeInfo : {}
        var hasWidth = false
        var hasHeight = false
        var size = { maxWidth: -1, maxHeight: -1, minWidth: 0, minHeight: -1 }
        if ( !child )
            return size
        if ( info.hasOwnProperty('maxWidth') ){
            hasWidth = true
            size.maxWidth = info.maxWidth
        }
        if ( info.hasOwnProperty('minWidth') ){
            hasWidth = true
            size.minWidth = info.minWidth
        }
        if ( info.hasOwnProperty('maxHeight') ){
            hasHeight = true
            size.maxHeight = info.maxHeight
        }
        if ( info.hasOwnProperty('minHeight') ){
            hasHeight = true
            size.minHeight = info.minHeight
        }
        if ( !hasWidth ){
            size.maxWidth = child.width
            size.minWidth = child.width
        }
        if ( !hasHeight ){
            size.maxHeight = child.height
            size.minHeight = child.height
        }
        size.minWidth += headerWidth
        if ( size.maxWidth >= 0 )
            size.maxWidth += headerWidth
        if ( size.minWidth < 200 ){
            size.minWidth = 200
        }

        if ( !compact ){
            size.minHeight += 30
            if ( size.maxHeight >= 0 )
                size.maxHeight += 30
        } else if ( size.minHeight < 30 ){
            size.minHeight = 30
        }
        vlog.i(size)
        return size
    }

    function swapOrAddPalette(swap){
        var pane = editor.parent
        while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
            pane = pane.parent
        }

        var coords = paletteContainer.mapToItem(pane, 0, 0)
        coords.y -= 35;

        var paletteList = paletteFunctions.views.openPaletetListBoxForContainer(
            paletteContainer,
            paletteContainer.parent,
            Qt.rect(coords.x - 180 / 2, coords.y, 30, 30),
            PaletteFunctions.PaletteListMode.PaletteContainer,
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
            paletteConnection.model = editFragment.language.bindingChannels
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
        paletteContainer.adjustSize()
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
        paletteContainer.adjustSize()
    }

    function closePalette(){
        if (paletteContainer.palette.type === "qml/import"){
            editFragment.language.importsFragment = null
        }
        var p = parent
        while (p && p.objectName !== "paletteGroup"){
            p = p.parent
        }
        p.palettesOpened = p.palettesOpened.filter(function(name){ return name !== paletteContainer.palette.name })
        editFragment.language.removePalette(paletteContainer.palette)
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
                    paletteContainer.sizeInfo = paletteContainer.calculateChildSizeInfo(paletteContainer.child)
                    if ( paletteContainer.parent && paletteContainer.parent.measureSizeInfo ){
                        paletteContainer.parent.measureSizeInfo()
                    }
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
                    paletteContainer.sizeInfo = paletteContainer.calculateChildSizeInfo(paletteContainer.child)
                    if ( paletteContainer.parent && paletteContainer.parent.measureSizeInfo ){
                        paletteContainer.parent.measureSizeInfo()
                    }
                })
                item.close.connect(function(){
                    paletteContainer.closePalette()
                })
                item.swapPalette.connect(function(){
                    paletteContainer.swapOrAddPalette(PaletteFunctions.PaletteListSwap.Swap)
                })
                item.addPalette.connect(function(){
                    paletteContainer.swapOrAddPalette(PaletteFunctions.PaletteListSwap.NoSwap)
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

        property alias container: paletteContainer

        function closeAsPane(){
            paletteContainer.closeAsPane()
        }
    }

    PaletteConnection{
        id: paletteConnection
        visible: model ? true : false
        anchors.top: parent.top
        anchors.topMargin: 30
        width: parent.width > 200 ? parent.width : 200
        onFocusChanged : if ( !focus ) model = null

        property var selectedHandler : function(){}
        property var cancelledHandler : function(index){}
    }
}
