import QtQuick 2.3
import QtQuick.Controls 2.2
import editor 1.0
import base 1.0
import fs 1.0 as Fs
import visual.input 1.0 as Input
import workspace 1.0 as Workspace

Workspace.Pane{
    id: root
    objectName : 'palette'
    paneType: 'palette'
    paneState : { return {} }

    property var panes: null
    property Theme currentTheme : lk.layers.workspace ? lk.layers.workspace.themes.current : null
    property color topColor: currentTheme ? currentTheme.paneTopBackground : 'black'
    property color backgroundColor: currentTheme ? currentTheme.paneTopBackground : 'black'

    property Item paletteContainer : null
    property alias paneHeaderContent : paneHeaderContent

    property string title : ''

    function removePane(){
        lk.layers.workspace.panes.removePane(root)
    }

    onWidthChanged: {
        if ( root.paletteContainer ){
            root.paletteContainer.container.adjustSize()
        }
    }

    Rectangle{
        id: paneHeader
        anchors.left: parent.left
        anchors.top: parent.top

        width: parent.width > 250 ? parent.width : 250
        height: 30
        color: root.topColor

        Workspace.PaneDragItem{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            onDragStarted: lk.layers.workspace.panes.__dragStarted(root)
            onDragFinished: lk.layers.workspace.panes.__dragFinished(root)
            display: root.title
        }

        Item{
            id: paneHeaderContent
            anchors.right: parent.right
            anchors.rightMargin: 35
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.fill: parent

            Text{
                id: paletteBoxTitle
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 50
                clip: true
                text: root.title
                color: '#82909b'
            }

            Item{
                id: paletteNewPaneButton
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.verticalCenter: parent.verticalCenter
                width: 15
                height: 15

                Rectangle{
                    anchors.top: parent.top
                    anchors.topMargin: 5
                    width: 10
                    height: 10
                    color: "transparent"
                    border.color: "#9b9da0"
                    border.width: 1
                    radius: 2
                }
                Rectangle{
                    anchors.top: parent.top
                    anchors.topMargin: 2
                    anchors.left: parent.left
                    anchors.leftMargin: 3
                    width: 10
                    height: 10
                    radius: 2
                    color: "#9b9da0"
                }
                MouseArea{
                    id: paletteNewPaneMouse
                    anchors.fill: parent
                    onClicked: {
                        if (root.paletteContainer)
                            root.paletteContainer.closeAsPane()
                        else
                            root.panes.clearPane(root)
                    }
                }
            }
        }

        Rectangle{
            anchors.right: parent.right
            width: 30
            height: parent.height
            color: 'transparent'

            Image{
                id : paneOptions
                anchors.centerIn: parent
                source : "qrc:/images/pane-menu.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    objectPalettePaneMenu.visible = !objectPalettePaneMenu.visible
                }
            }
        }
    }

    Workspace.PaneMenu{
        id: objectPalettePaneMenu
        visible: false
        anchors.right: root.right
        anchors.topMargin: 30
        anchors.top: root.top

        property bool supportsShaping: false

        style: root.currentTheme.popupMenuStyle


        Workspace.PaneMenuItem{
            text: qsTr("Move to new window")
            onClicked: {
                objectPalettePaneMenu.visible = false
                root.panes.movePaneToNewWindow(root)
            }
        }
        Workspace.PaneMenuItem{
            text: qsTr("Remove Pane")
            onClicked: {
                objectPalettePaneMenu.visible = false
                if (root.aletteContainer)
                    root.paletteContainer.closeAsPane()
                else
                    lk.layers.workspace.panes.clearPane(root)
            }
        }
    }

    Rectangle{
        id: paletteContainer
        anchors.topMargin: 30
        anchors.top: parent.top
        anchors.fill: parent
        color: root.backgroundColor

        ScrollView{
            id: flick

            anchors.fill: parent
            anchors.margins: 3

            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            ScrollBar.vertical.contentItem: Input.ScrollbarHandle{
                color: root.currentTheme.scrollStyle.handleColor
                visible: flick.contentHeight > flick.height
            }
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
            ScrollBar.horizontal.contentItem: Input.ScrollbarHandle{
                color: root.currentTheme.scrollStyle.handleColor
                visible: flick.contentWidth > flick.width
            }

            clip: true
            contentHeight: root.paletteContainer ? root.paletteContainer.height + 15: 0
            contentWidth: root.paletteContainer ? root.paletteContainer.width + 15: 0

            Flickable {
                Rectangle {
                    color: root.currentTheme.colorScheme.background
                    width: root.paletteContainer ? root.paletteContainer.width + 5 : 0
                    height: root.paletteContainer ? root.paletteContainer.height + 5: 0

                    children : root.paletteContainer ? [root.paletteContainer] : []

                }
            }

        }
    }

}
