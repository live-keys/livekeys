import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import base 1.0
import live 1.0
import fs 1.0 as Fs
import workspace 1.0 as Workspace

Pane{
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

    Rectangle{
        id: paneHeader
        anchors.left: parent.left
        anchors.top: parent.top

        width: parent.width > 250 ? parent.width : 250
        height: 30
        color: root.topColor

        PaneDragItem{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            onDragStarted: root.panes.__dragStarted(root)
            onDragFinished: root.panes.__dragFinished(root)
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
                    root.panes.clearPane(root)
            }
        }
    }

    Rectangle{
        id: paletteContainer
        anchors.topMargin: 30
        anchors.top: parent.top
        anchors.fill: parent
        color: root.backgroundColor

        ScrollView {
            id: flick

            anchors.fill: parent
            anchors.margins: 3

            style: ScrollViewStyle {
                transientScrollBars: false
                handle: Item {
                    implicitWidth: 10
                    implicitHeight: 10
                    Rectangle {
                        color: "#1f2227"
                        anchors.fill: parent
                    }
                }
                scrollBarBackground: Item{
                    implicitWidth: 10
                    implicitHeight: 10
                    Rectangle{
                        anchors.fill: parent
                        color: root.backgroundColor
                    }
                }
                decrementControl: null
                incrementControl: null
                frame: Item{}
                corner: Rectangle{color: root.backgroundColor}
            }

            flickableItem.contentHeight: root.objectContainer ? root.objectContainer.height: 0
            flickableItem.contentWidth: root.objectContainer ? root.objectContainer.width : 0

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
