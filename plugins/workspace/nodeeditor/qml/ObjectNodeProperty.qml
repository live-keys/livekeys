import QtQuick 2.3

Rectangle{
    id: propertyItem
    property string propertyName : ''
    
    property Item inPort : null
    property Item outPort : null
    property QtObject node : null
    property var editingFragment: null

    width: 340
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
}
