import QtQuick 2.3

Rectangle{
    property string propertyName : ''
    
    property Item inPort : null
    property Item outPort : null
    property QtObject node : null
    
    width: 174
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
    
}