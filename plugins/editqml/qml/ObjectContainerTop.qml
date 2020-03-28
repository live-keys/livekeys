import QtQuick 2.3
import live 1.0

Rectangle{
    id: objectContainerTitle
    y: topSpacing
    radius: 3

    property bool compact : true

    signal toggleCompact()
    signal erase()
    signal openConnections()
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

    Item{
        id: eraseButton
        anchors.right: parent.right
        anchors.rightMargin: 80 - (closeObjectItem.visible ? 0 : 18)
        anchors.verticalCenter: parent.verticalCenter
        width: 15
        height: titleHeight
        Image{
            anchors.centerIn: parent
            source: "qrc:/images/palette-erase-object.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                objectContainerTitle.erase()
            }
        }
    }


    Item{
        id: connectionsButton
        anchors.right: parent.right
        anchors.rightMargin: 60 - (closeObjectItem.visible ? 0 : 18)
        anchors.verticalCenter: parent.verticalCenter
        width: 15
        height: 20
        Image{
            anchors.centerIn: parent
            source: "qrc:/images/palette-connections.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
            }
        }
    }


    Item{
        id: paletteAddButton
        anchors.right: parent.right
        anchors.rightMargin: 40 - (closeObjectItem.visible ? 0 : 18)
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
            onClicked: {
                objectContainerTitle.addPalette()
            }
        }
    }

    Item{
        id: composeButton
        anchors.right: parent.right
        anchors.rightMargin: 22  - (closeObjectItem.visible ? 0 : 18)
        anchors.verticalCenter: parent.verticalCenter
        width: 15
        height: 20
        Image{
            anchors.centerIn: parent
            source: "qrc:/images/palette-add-property.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                objectContainerTitle.compose()
            }
        }
    }

    Item{
        id: paletteToPane
        anchors.right: parent.right
        anchors.rightMargin: 100 - (closeObjectItem.visible ? 0 : 18)
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
            anchors.fill: parent
            onClicked: {
                objectContainerTitle.paletteToPane()
            }
        }
    }

    Item{
        id: closeObjectItem
        anchors.right: parent.right
        anchors.rightMargin: -4
        anchors.top: parent.top
        width: 20
        height: 20
        visible: !(objectContainer.editingFragment && objectContainer.editingFragment.parentFragment())
        Text{
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
