import QtQuick 2.3
import visual.shapes 1.0
import workspace.icons 1.0 as Icons

Rectangle{
    id: root
    color: '#333'

    property color iconColor: "#ccc"

    property bool enableMove: false
    property var handleMovePress : function(mouse){}
    property var handleMovePositionChanged : function(mouse, currentpos, lastpos){}

    property bool canSwapPalette: false
    property bool canAddPalette: false
    property bool canRebuild : false
    property bool canViewConnections: false
    property bool canMoveToNewPane: false

    property string title: ''

    signal swapPalette()
    signal addPalette()
    signal viewConnections()
    signal moveToNewPane()
    signal collapse()
    signal rebuild()
    signal close()

    MouseArea{
        id: paletteBoxMoveArea
        anchors.fill: parent

        enabled: root.enableMove
        cursorShape: enabled ? Qt.SizeAllCursor : Qt.ArrowCursor

        property point lastMousePos : Qt.point(0, 0)
        onPressed: {
            root.handleMovePress(mouse)
            lastMousePos = paletteBoxMoveArea.mapToGlobal(mouse.x, mouse.y)
        }
        onPositionChanged: {
            if ( mouse.buttons & Qt.LeftButton ){
                var currentMousePos = item.mapToGlobal(mouse.x, mouse.y)
                root.handleMovePositionChanged(mouse, currentMousePos, lastMousePos)
                lastMousePos = currentMousePos
            }
        }
    }

    Item{
        id: paletteSwapButton
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        width: 15
        height: 20
        visible: root.canSwapPalette
        Image{
            anchors.centerIn: parent
            source: "qrc:/images/palette-swap.png"
        }
        MouseArea{
            id: paletteSwapMouse
            anchors.fill: parent
            onClicked: root.swapPalette()
        }
    }

    Item{
        id: paletteAddButton
        visible: root.canAddPalette
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
            onClicked: root.addPalette()
        }
    }


    Text{
        id: paletteBoxTitle
        anchors.left: parent.left
        anchors.leftMargin: root.canSwapPalette || root.canAddPalette ? 50 : 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 50
        clip: true
        text: root.title
        color: '#82909b'
    }


    Item{
        id: rebuild
        anchors.right: parent.right
        anchors.rightMargin: 60
        anchors.verticalCenter: parent.verticalCenter
        width: 15
        height: 20
        visible: root.canRebuild
        Image{
            anchors.centerIn: parent
            source: "qrc:/images/palette-integrate.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                root.rebuild()
            }
        }
    }

    Item{
        id: paletteConnectionsButton
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 40
        anchors.verticalCenter: parent.verticalCenter
        width: 11
        height: 11
        visible: root.canViewConnections

        Image{
            width: parent.width; height: parent.height
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/palette-connections.png"
        }
        MouseArea{
            id: paletteConnectionMouse
            anchors.fill: parent
            onClicked: root.viewConnections()
        }
    }

    Item{
        id: paletteNewPaneButton
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 40
        anchors.verticalCenter: parent.verticalCenter
        width: 12
        height: 20
        visible: root.canMoveToNewPane

        Rectangle{
            anchors.top: parent.top
            anchors.topMargin: 14
            anchors.left: parent.left
            anchors.leftMargin: 2
            width: 8
            height: 8
            color: "transparent"
            border.color: "#9b9da0"
            border.width: 1
            radius: 2
        }
        Rectangle{
            anchors.top: parent.top
            anchors.topMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 5
            width: 8
            height: 8
            radius: 2
            color: "#9b9da0"
        }
        MouseArea{
            id: paletteNewPaneMouse
            anchors.fill: parent
            onClicked: root.moveToNewPane()
        }
    }

    Item{
        id: minimizeButton
        width: 10
        height: 15
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20

        Triangle{
            width: 7
            height: 7
            color: root.iconColor
            anchors.centerIn: parent

            rotation: Triangle.Bottom
        }

        MouseArea{
            id: switchMinimized
            anchors.fill: parent
            onClicked: root.collapse()
        }
    }

    Item{
        id: closeButton
        width: 10
        height: 15
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 5

        Icons.XIcon{
            anchors.centerIn: parent
            width: 6
            height: 6
            color: root.iconColor
            strokeWidth: 1
        }
        MouseArea{
            id: paletteCloseArea
            anchors.fill: parent
            onClicked: root.close()
        }
    }
}
