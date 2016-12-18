import QtQuick 2.3

Rectangle{
    id: root

    property alias text: buttonLabel.text
    property var callback : function(){}

    signal clicked()
    onClicked: callback()

    color: buttonMouseArea.containsMouse ? "#222240" : "#222230"
    width: 100
    height: 30

    Text{
        id: buttonLabel
        anchors.centerIn: parent
        text: ""
        color: "#ccc"
        font.pixelSize: 12
        font.family: "Open Sans, sans-serif"
        font.weight: Font.Light
    }
    MouseArea{
        id: buttonMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked()
        }
    }
}
