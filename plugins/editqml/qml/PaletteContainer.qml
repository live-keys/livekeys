import QtQuick 2.3
import base 1.0
import live 1.0
import editor 1.0
import editor.private 1.0

Rectangle{
    id: paletteContainer
    width: (child && child.width > minimumWidth ? child.width : minimumWidth) + headerWidth
    height: child ? child.height + (compact ? 0 : normalHeaderHeight) +5 : 0
    objectName: "paletteContainer"

    property QtObject theme: lk.layers.workspace.themes.current
    property var editingFragment: null
    property double minimumWidth: compact ? 0 : 200
    property double headerWidth: compact ? compactHeaderWidth + 5 : 0
    property int compactHeaderWidth: rightButtons.width + 7
    property int normalHeaderHeight: 35

    property bool compact: true
    property bool isBuilder : false

    property Item child : null
    property QtObject palette : null

    color: "black"

    property string name : ''
    property string type : ''
    property string title : type + ' - ' + name
    property var cursorRectangle : null
    property var editorPosition : null
    property var editor: null

    property alias paletteSwapVisible : paletteSwapButton.visible
    property alias paletteAddVisible : paletteAddButton.visible
    property bool moveEnabledSet : true
    property alias moveEnabledGet: paletteBoxMoveArea.enabled

    property double titleLeftMargin : 50
    property double titleRightMargin : 50

    property DocumentHandler documentHandler : null

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls

    Component.onCompleted: {
        var paletteGroup = paletteContainer.parent

        if ( paletteGroup.editingFragment) {
            paletteContainer.editingFragment = paletteGroup.editingFragment
            paletteContainer.isBuilder = paletteGroup.editingFragment.isBuilder()
            paletteGroup.editingFragment.connectionChanged.connect(function(){
                paletteContainer.isBuilder = paletteGroup.editingFragment.isBuilder()
            })
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: paletteContainer.child.forceActiveFocus()
    }

    Rectangle{
        id: paletteBoxHeader
        height: compact && child
                    ? child.height
                    : normalHeaderHeight
        width: !compact && child
                    ? (paletteContainer.parent ? paletteContainer.parent.width : paletteContainer.width)
                    : compactHeaderWidth

        visible: !compact

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: compact && child ? child.width + 10: 0
        clip: true
        color: theme.colorScheme.middleground

        MouseArea{
            id: paletteBoxMoveArea
            anchors.fill: parent

            enabled: !compact && moveEnabledSet
            cursorShape: enabled ? Qt.SizeAllCursor : Qt.ArrowCursor

            property point lastMousePos : Qt.point(0, 0)
            onPressed: {
                paletteContainer.parent.parent.disableMoveBehavior()
                lastMousePos = paletteBoxMoveArea.mapToGlobal(mouse.x, mouse.y)
            }
            onPositionChanged: {
                if ( mouse.buttons & Qt.LeftButton ){
                    var currentMousePos = paletteBoxMoveArea.mapToGlobal(mouse.x, mouse.y)
                    var deltaX = currentMousePos.x - lastMousePos.x
                    var deltaY = currentMousePos.y - lastMousePos.y
                    paletteContainer.parent.parent.x += deltaX
                    paletteContainer.parent.parent.y += deltaY

                    lastMousePos = currentMousePos
                }
            }
        }

        function swapOrAddPalette(swap){
            var pane = editor.parent
            while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
                pane = pane.parent
            }

            var coords = paletteContainer.mapToItem(pane, 0, 0)
            coords.y -= 35;

            var paletteList = paletteControls.addPaletteList(
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
            if ( !editingFragment )
                return

            if ( paletteConnection.model ){
                paletteConnection.model = null
                paletteConnection.editingFragment = null
            } else {
                paletteConnection.forceActiveFocus()
                paletteConnection.model = editor.documentHandler.codeHandler.bindingChannels
                paletteConnection.editingFragment = editingFragment
            }
        }

        function rebuild(){
            var editingFragment = paletteContainer.parent.editingFragment
            if ( !editingFragment )
                return

            editingFragment.rebuild()
        }

        Item{
            id: paletteSwapButton
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            visible: !compact && moveEnabledSet
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-swap.png"
            }
            MouseArea{
                id: paletteSwapMouse
                anchors.fill: parent
                onClicked: paletteBoxHeader.swapOrAddPalette(PaletteControls.PaletteListSwap.Swap)
            }
        }
        Item{
            visible: !compact && moveEnabledSet
            id: paletteAddButton
            anchors.left: parent.left
            anchors.leftMargin: 25
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-add.png"
            }
            MouseArea{
                id: paletteAddMouse
                anchors.fill: parent
                onClicked: paletteBoxHeader.swapOrAddPalette(PaletteControls.PaletteListSwap.NoSwap)
            }
        }

        Text{
            visible: !compact
            id: paletteBoxTitle
            anchors.left: parent.left
            anchors.leftMargin: !compact && moveEnabledSet ? paletteContainer.titleLeftMargin : 10
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: paletteContainer.titleRightMargin
            clip: true
            text: paletteContainer.title
            color: '#82909b'
        }


        Item{
            id: rebuild
            anchors.right: parent.right
            anchors.rightMargin: 60
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            visible: paletteContainer.isBuilder
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-integrate.png"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    paletteBoxHeader.rebuild()
                }
            }
        }

        Item{
            id: paletteConnectionButton
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.rightMargin: 40
            anchors.verticalCenter: parent.verticalCenter
            width: 11
            height: 11
            visible: {
                var dontShow = paletteContainer.parent && paletteContainer.parent.parent && paletteContainer.parent.parent.objectName !== "editorBox"
                return !compact && !dontShow
            }

            Image{
                width: parent.width; height: parent.height
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/palette-connections.png"
            }
            MouseArea{
                id: paletteConnectionMouse
                anchors.fill: parent
                onClicked: {
                    paletteBoxHeader.viewPaletteConnections()
                }
            }
        }
    }


    Rectangle {
        id: rightButtons
        color: theme.colorScheme.middleground
        width: rightButtons.makeVertical ? 20 : 35
        height: compact && child ? child.height : 24
        radius: 2

        property bool makeVertical: child && child.height > 48 && compact

        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.right: parent.right
        anchors.rightMargin: 2

        Triangle{
            id: minimizeButton
            width: 8
            height: 8
            color: "#9b9da0"

            anchors.top: rightButtons.makeVertical ? closeButton.bottom : parent.top
            anchors.topMargin: 8
            anchors.left: parent.left
            anchors.leftMargin: 5

            rotation: Triangle.Top
            MouseArea{
                id: switchMinimized
                anchors.fill: parent
                onClicked: {
                    if (palette)
                        compact = !compact
                        if (compact) minimizeButton.rotation = Triangle.Top
                        else minimizeButton.rotation = Triangle.Bottom
                }
            }
        }

        Item{
            id: closeButton
            width: 10
            height: 15
            anchors.top: parent.top
            anchors.topMargin: 3
            anchors.left: parent.left
            anchors.leftMargin: rightButtons.makeVertical ? 5 : 20

            Text{
                anchors.verticalCenter: parent.verticalCenter
                text: 'x'
                color: '#ffffff'
            }
            MouseArea{
                id: paletteCloseArea
                anchors.fill: parent
                onClicked: {
                    if (paletteContainer.palette.type === "qml/import"){
                        paletteContainer.palette.item.editor.importsShaped = false
                    }
                    var p = parent
                    while (p && p.objectName !== "paletteGroup"){
                        p = p.parent
                    }
                    p.palettesOpened = p.palettesOpened.filter(function(name){ return name !== paletteContainer.palette.name })
                    documentHandler.codeHandler.removePalette(paletteContainer.palette)
                }
            }
        }
    }

    Item{
        id: paletteChild
        anchors.top: compact? parent.top : paletteBoxHeader.bottom
        anchors.left: parent.left
        width: parent.child ? parent.child.width: 0
        height: parent.child ? parent.child.height: 0
        children: parent.child ? [parent.child] : []
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
