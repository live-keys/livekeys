import QtQuick 2.3

Rectangle {
    width: 100
    height: 30
    border.width : 1
    border.color : "#ff0000"

    anchors.fill: parent

    property alias text : textInput.text

    TextInput{
        id : textInput
        anchors.fill: parent
        anchors.margins : 6
        font.family : "Courier New, Courier"
        font.pixelSize: 14
        font.weight : Font.Normal
        text: 'asda'
        selectByMouse: true

        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.IBeamCursor
        }
    }
}

