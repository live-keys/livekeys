import QtQuick 2.3

Rectangle{
    id : root

    width : 100
    height : 50

    property string text : "Button"

    property string fontFamily : "Ubuntu Mono, Courier New, Courier"
    property int fontPixelSize : 14

    property color textColor : "#fff"
    property color backgroundHoverColor : "#082134"
    property color backgroundColor : "#061a29"

    signal clicked()

    color : buttonMouseArea.containsMouse ? backgroundHoverColor : backgroundColor

    Text{
        color : root.textColor
        text : root.text
        font.family: root.fontFamily
        font.pixelSize: root.fontPixelSize
        anchors.centerIn: parent
    }

    MouseArea{
        id : buttonMouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: root.clicked()
    }
}

