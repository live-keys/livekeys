import QtQuick 2.3

Rectangle{
    id: root
    width: parent ? parent.buttonWidth : 100
    height: parent ? parent.buttonHeight : 30
    color: parent ? menuItemArea.containsMouse ? parent.style.highlightBackgroundColor : parent.style.backgroundColor : 'transparent'

    signal clicked(var mouse)
    property string text: ''

    Text{
        id: menuItemText
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        text: root.text
        font.family: root.parent ? menuItemArea.containsMouse ? root.parent.style.highlightTextStyle.font.family : root.parent.style.textStyle.font.family : 'sans-serif'
        font.pixelSize: root.parent ? menuItemArea.containsMouse ? root.parent.style.highlightTextStyle.font.pixelSize : root.parent.style.textStyle.font.pixelSize : 12
        font.weight: root.parent ? menuItemArea.containsMouse ? root.parent.style.highlightTextStyle.font.weight : root.parent.style.textStyle.font.weight : Font.Normal
        color: root.parent ? menuItemArea.containsMouse ? root.parent.style.highlightTextStyle.color : root.parent.style.textStyle.color : 'white'
    }
    MouseArea{
        id : menuItemArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked(mouse)
        }
    }
}
