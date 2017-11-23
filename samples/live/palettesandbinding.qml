import QtQuick 2.3
import live 1.0

Rectangle{
    color: "#331133"
    width: 100
    height: 150
    x: 0
    y: 0 

    SequentialAnimation on x{
        id: xAnim
        running: false
        loops: Animation.Infinite
        
        NumberAnimation { from: 0; to: 200; duration: 500; easing.type: Easing.InOutQuad }
        NumberAnimation { from: 200; to: 0; duration: 500; easing.type: Easing.InOutQuad }
        PauseAnimation { duration: 250 }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: xAnim.start()
    }
}
