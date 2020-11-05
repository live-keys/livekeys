import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import base 1.0
import live 1.0
import fs 1.0 as Fs

Pane{
    id: root
    objectName : 'objectPalette'
    paneType: 'objectPalette'
    paneState : { return {} }

    property var panes: null
    property Theme currentTheme : lk.layers.workspace ? lk.layers.workspace.themes.current : null
    property color topColor: currentTheme ? currentTheme.paneTopBackground : 'black'
    property color backgroundColor: currentTheme ? currentTheme.paneTopBackground : 'black'

    property Item objectContainer : null
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
            anchors.fill: parent
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

    Rectangle {
        id: objectPalettePaneMenu
        visible: false
        anchors.right: root.right
        anchors.topMargin: 30
        anchors.top: root.top
        property int buttonHeight: 30
        property int buttonWidth: 180
        opacity: visible ? 1.0 : 0
        z: 1000

        Behavior on opacity{ NumberAnimation{ duration: 200 } }

        Rectangle{
            id: newWindowEditorButton
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: newWindowEditorText
                text: qsTr("Move to new window")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: newWindowEditorArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : newWindowEditorArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    objectPalettePaneMenu.visible = false
                    root.panes.movePaneToNewWindow(root)
                }
            }
        }

        Rectangle{
            id: removeEditorButton
            anchors.top: newWindowEditorButton.bottom
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: removeEditorText
                text: qsTr("Remove Pane")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: removeEditorArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : removeEditorArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    objectPalettePaneMenu.visible = false
                    if (objectContainer)
                        objectContainer.closeAsPane()
                    else root.panes.clearPane(root)
                }
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
                Item {
                    width: root.objectContainer ? root.objectContainer.width : 0
                    height: root.objectContainer ? root.objectContainer.height: 0

                    children : root.objectContainer ? [root.objectContainer] : []

                }
            }

        }
    }

}
