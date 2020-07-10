import QtQuick 2.3
import live 1.0
import workspace 1.0 as Workspace

Rectangle{
    id: objectContainerTitle
    y: topSpacing
    radius: 3

    property bool compact : true
    property bool isBuilder : false

    signal toggleCompact()
    signal erase()
    signal rebuild()
    signal toggleConnections()
    signal addPalette()
    signal compose()
    signal paletteToPane()
    signal close()

    Item{
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        width: 15
        height: parent.height
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
            visible: !(objectContainer.editingFragment && objectContainer.editingFragment.parentFragment())
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
            anchors.top: parent.top
            width: 15
            height: 20
            visible: !(objectContainer.editingFragment && objectContainer.editingFragment.parentFragment())
            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                text: 'x'
                color: '#ffffff'
                font.pixelSize: 18
                font.family: "Open Sans"
                font.weight: Font.Light
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
