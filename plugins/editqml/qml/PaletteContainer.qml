import QtQuick 2.3
import base 1.0
import live 1.0
import editor 1.0
import editor.private 1.0

Item{
    id: paletteContainer
    width: child ? child.width  + (compact? compactHeaderWidth: 0) : 0
    height: child ? child.height + (compact? 0 : normalHeaderHeight) : 0
    objectName: "paletteContainer"

    property int compactHeaderWidth: 40
    property int normalHeaderHeight: 35

    property bool compact: true

    property Item child : null
    property QtObject palette : null

    property string name : ''
    property string type : ''
    property string title : type + ' - ' + name
    property var cursorRectangle : null
    property var editorPosition : null

    property alias paletteSwapVisible : paletteSwapButton.visible
    property alias paletteAddVisible : paletteAddButton.visible
    property bool moveEnabledSet : true
    property alias moveEnabledGet: paletteBoxMoveArea.enabled

    property double titleLeftMargin : 50
    property double titleRightMargin : 50

    property DocumentHandler documentHandler : null
    property var paletteContainerFactory : null

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
                    ? (paletteContainer.parent ? paletteContainer.parent.width : paletteContainer.width) + 10
                    : compactHeaderWidth

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: compact && child ? child.width + 10: 0
        clip: true
        color: compact? "black":"#141c25"

        MouseArea{
            enabled: !compact && moveEnabledSet
            id: paletteBoxMoveArea
            anchors.fill: parent
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
            var editingFragment = paletteContainer.parent.editingFragment
            if ( !editingFragment )
                return

            var palettes = documentHandler.codeHandler.findPalettes(editingFragment.position(), true)
            if (palettes.size() ){
                paletteHeaderList.forceActiveFocus()
                paletteHeaderList.model = palettes
                paletteHeaderList.cancelledHandler = function(){
                    paletteHeaderList.focus = false
                    paletteHeaderList.model = null
                }
                paletteHeaderList.selectedHandler = function(index){
                    paletteHeaderList.focus = false
                    paletteHeaderList.model = null

                    var paletteGroup = paletteContainer.parent;
                    var editorBox = paletteGroup.parent

                    var palette = documentHandler.codeHandler.openPalette(editingFragment, palettes, index)
                    var newPaletteBox = paletteContainer.paletteContainerFactory(paletteGroup)

                    palette.item.x = 5
                    palette.item.y = 7

                    newPaletteBox.child = palette.item
                    newPaletteBox.palette = palette

                    newPaletteBox.name = palette.name
                    newPaletteBox.type = palette.type
                    newPaletteBox.moveEnabled = paletteContainer.moveEnabledGet
                    newPaletteBox.documentHandler = documentHandler
                    newPaletteBox.cursorRectangle = paletteContainer.cursorRectangle
                    newPaletteBox.editorPosition = paletteContainer.editorPosition
                    newPaletteBox.paletteContainerFactory = paletteContainer.paletteContainerFactory

                    if (swap){
                        paletteContainer.parent = null
                        paletteContainer.documentHandler.codeHandler.removePalette(paletteContainer.palette)
                        paletteContainer.destroy()
                    }
                }
            }
        }

        function viewPaletteConnections(){
            var editingFragment = paletteContainer.parent.editingFragment
            if ( !editingFragment )
                return

            if ( paletteConnection.model ){
                paletteConnection.model = null
            } else {
                paletteConnection.forceActiveFocus()
                paletteConnection.model = editingFragment.bindingModel(documentHandler.codeHandler)
            }
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
                onClicked: paletteBoxHeader.swapOrAddPalette(true)
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
                onClicked: paletteBoxHeader.swapOrAddPalette(false)
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
            id: paletteConnectionButton
            anchors.right: parent.right
            anchors.rightMargin: 40
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            visible: !compact

            Image{
                anchors.centerIn: parent
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


        Rectangle {
            id: rightButtons
            color: compact? "#131a24" : "transparent"
            width: 35
            height: 24
            radius: compact? 5: 0

            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.right: parent.right
            anchors.rightMargin: 3


            Triangle{
                id: minimizeButton
                width: 8
                height: 8
                color: "#9b9da0"

                anchors.top: parent.top
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
                anchors.leftMargin: 20

                Text{
                    anchors.verticalCenter: parent.verticalCenter
                    text: 'x'
                    color: '#ffffff'
                }
                MouseArea{
                    id: paletteCloseArea
                    anchors.fill: parent
                    onClicked: {
                        documentHandler.codeHandler.removePalette(paletteContainer.palette)
                    }
                }
            }
        }
    }

    Item{
        id: paletteChild
        anchors.top: compact? parent.top : paletteBoxHeader.top
        anchors.topMargin: compact? 0:paletteBoxHeader.height
        anchors.left: parent.left
        children: parent.child ? [parent.child] : []
    }

    PaletteListView{
        id: paletteHeaderList
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

        onPaletteSelected: selectedHandler(index)
        onCancelled : cancelledHandler()
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
}
