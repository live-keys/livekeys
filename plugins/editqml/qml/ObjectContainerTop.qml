import QtQuick 2.3
import live 1.0
import workspace 1.0 as Workspace
import editqml 1.0
import workspace.icons 1.0 as Icons

Rectangle{
    id: objectContainerTitle
    y: topSpacing
    radius: 3

    property bool compact : true
    property bool isBuilder : false

    property var objectContainer: null

    signal toggleCompact()
    signal erase()
    signal rebuild()
    signal toggleConnections()
    signal assignFocus()
    signal addPalette()
    signal compose()
    signal paletteToPane()
    signal close()
    signal createObject()

    MouseArea{
        anchors.fill: parent
        onClicked: objectContainerTitle.assignFocus()
    }

    Item{
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        width: 15
        height: parent.height
        visible: !objectContainer.pane
        Triangle{
            width: 8
            height: 8
            color: '#9b9da0'
            anchors.verticalCenter: parent.verticalCenter
            rotation: compact ? Triangle.Right : Triangle.Bottom
        }
        MouseArea{
            id: compactObjectButton
            anchors.fill: parent
            onClicked: {
                objectContainerTitle.toggleCompact()
            }
        }
    }

    Text{
        anchors.left: parent.left
        anchors.leftMargin: 40
        width: parent.width - 140 + (closeObjectItem.visible ? 0 : 18)
        anchors.verticalCenter: parent.verticalCenter
        text: objectContainer.title
        clip: true
        elide: Text.ElideRight
        color: '#82909b'
    }

    Row{
        height: parent.height
        anchors.right: parent.right
        anchors.rightMargin: 5
        spacing: 3

        Item {
            id: createObjectButton
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 15

            Rectangle {
                visible: objectContainer.isForProperty && objectContainer.editingFragment && objectContainer.editingFragment.isNull
                color: "red"
                anchors.fill: parent
            }

            MouseArea{
                id: createObjectButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    objectContainerTitle.createObject()
                }
            }
            Workspace.Tooltip{
                mouseOver: createObjectButtonMouseArea.containsMouse
                text: "Create object"
            }
        }

        Item{
            id: paletteToPane
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
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
                id: paletteToPaneMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    objectContainerTitle.paletteToPane()
                }
            }
            Workspace.Tooltip{
                mouseOver: paletteToPaneMouseArea.containsMouse
                text: "Move to new pane"
            }
        }

        Item{
            id: eraseButton
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: titleHeight
            visible: objectContainer.editingFragment && !objectContainer.editingFragment.isOfFragmentType(QmlEditFragment.ReadOnly)
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-erase-object.png"
            }
            MouseArea{
                id: eraseButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    objectContainerTitle.erase()
                }
            }
            Workspace.Tooltip{
                mouseOver: eraseButtonMouseArea.containsMouse
                text: "Erase object"
            }
        }

        Item{
            id: rebuild
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            visible: objectContainerTitle.isBuilder
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-integrate.png"
            }
            MouseArea{
                id: rebuildMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    objectContainerTitle.rebuild()
                }
            }
            Workspace.Tooltip{
                mouseOver: rebuildMouseArea.containsMouse
                text: "Rebuild section"
            }
        }

        Item{
            id: connectionsButton
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            visible: objectContainer.editingFragment
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-connections.png"
            }
            MouseArea{
                id: connectionsButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    objectContainerTitle.toggleConnections()
                }
            }
            Workspace.Tooltip{
                mouseOver: connectionsButtonMouseArea.containsMouse
                text: "View connections"
            }
        }

        Item{
            id: paletteAddButton
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
                hoverEnabled: true
                onClicked: {
                    objectContainerTitle.addPalette()
                }
            }
            Workspace.Tooltip{
                mouseOver: paletteAddMouse.containsMouse
                text: "Open palette"
            }
        }

        Item{
            id: composeButton
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            visible: objectContainer.editingFragment
                  && objectContainer.editingFragment.type() !== 'qml/QtQuick#Component'
                  && !(objectContainer.editingFragment.isOfFragmentType(QmlEditFragment.ReadOnly)
                  && !objectContainer.editingFragment.isOfFragmentType(QmlEditFragment.Group))
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-add-property.png"
            }
            MouseArea{
                id: composeButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    objectContainerTitle.compose()
                }
            }
            Workspace.Tooltip{
                mouseOver: composeButtonMouseArea.containsMouse
                text: "Add content"
            }
        }

        Item{
            id: closeObjectItem
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            visible: !(objectContainer.editingFragment && objectContainer.editingFragment.parentFragment()) && !objectContainer.pane
            Icons.XIcon{
                anchors.centerIn: parent
                width: 8
                height: 8
                color: '#dbdede'
                strokeWidth: 1
            }

            MouseArea{
                id: paletteCloseArea
                anchors.fill: parent
                onClicked: {
                    objectContainerTitle.close()
                }
            }
        }
    }
}
