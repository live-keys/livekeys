import QtQuick 2.3
import live 1.0
import workspace 1.0

LabelOnRectangle{
    id: root

    property int mode: 0

    property var up: undefined
    property var down: undefined
    property var wheelEnabled: false
    Triangle{
        id: upTr
        anchors.top: parent.top
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 4
        visible: mode !== 0
        width: 7
        height: 5
        color: root.style.textStyle.color
        rotation: Triangle.Top

        MouseArea {
            anchors.fill: parent
            onClicked: {
               if (up) up()
            }
            onWheel: {
                if (!wheelEnabled) return
                if (wheel.angleDelta.y > 0 && up) up()
                if (wheel.angleDelta.y < 0 && down) down()
            }
        }

    }

    Triangle{
        anchors.top: upTr.bottom
        anchors.topMargin: 2
        anchors.left: parent.left
        anchors.leftMargin: 4
        visible: mode !== 0
        id: downTr
        width: 7
        height: 5
        color: root.style.textStyle.color
        rotation: Triangle.Bottom

        MouseArea {
            anchors.fill: parent
            onClicked: {
               if (down) down()
            }
            onWheel: {
                if (wheel.angleDelta.y > 0 && up) up()
                if (wheel.angleDelta.y < 0 && down) down()
            }
        }
    }


    MouseArea {
        anchors.fill: parent
        onWheel: {
            if (!wheelEnabled) return
            if (wheel.angleDelta.y > 0 && up) up()
            if (wheel.angleDelta.y < 0 && down) down()
        }
    }
}
