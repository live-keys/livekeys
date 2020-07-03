import QtQuick 2.3
import live 1.0

Item{
    id : root
    width : 20
    height : 20
    
    property bool isRunning: false
    signal clicked(bool value)

    property color color: "#aeaeae"

    Triangle{
        id: triangle
        anchors.centerIn: parent
        width: root.width  * 2 / 3
        height: root.height * 2 / 3
        color: root.color
        rotation: Triangle.Right
        visible: !root.isRunning
    }

    Text{
        id: text
        anchors.centerIn : parent
        text : '||'
        color : root.color
        font.pixelSize: root.height
        font.bold: true
        font.letterSpacing: -1
        visible: root.isRunning
    }

    MouseArea{
        anchors.fill : parent
        onClicked : {
            root.clicked(!root.isRunning)
        }
    }
}
