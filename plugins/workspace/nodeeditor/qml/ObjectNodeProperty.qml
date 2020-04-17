import QtQuick 2.3

Rectangle{
    id: propertyItem
    property string propertyName : ''
    
    property Item inPort : null
    property Item outPort : null
    property QtObject node : null
    property var editingFragment: null

    property var isForObject: editingFragment && editingFragment.isForObject()

    anchors.left: parent.left
    anchors.leftMargin: isForObject ? 30 : 0
    width: 340 - anchors.leftMargin
    height: 30
    radius: 5
    color: "#333"
    
    Text{
        color: 'white'
        font.family: "Open Sans"
        font.pixelSize: 12
        anchors.verticalCenter : parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        text: parent.propertyName
    }
    
    Connections {
        target: editingFragment
        onAboutToBeRemoved: {
            propertyItem.destroy()
        }
        ignoreUnknownSignals: true
    }

    Item{
        id: paletteAddButton
        anchors.right: parent.right
        anchors.rightMargin: 25
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
            }
        }
    }

    Item{
        visible: !isForObject
        id: closeObjectItem
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.top: parent.top
        width: 20
        height: 20
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
            }
        }
    }
}
