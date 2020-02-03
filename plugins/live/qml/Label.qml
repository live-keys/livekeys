import QtQuick 2.5
import live 1.0

Rectangle{
    width: textRectangle.width + containerRectangle.width
    height: 22
    color: '#0c121d'
    radius: 5

    property alias text: labelText.text
    property alias textColor: labelText.color
    property int   mode: 0
    property var   up: undefined
    property var   down: undefined

    Rectangle {
        id: containerRectangle
        height: parent.height
        width: mode !== 0 ? 16 : 0
        color: 'transparent'
        anchors.left: parent.left
        anchors.leftMargin: {
            switch (mode) {
                case 0: return 0
                case 1: return 0
                case 2: return 38
            }
        }

        Triangle{
            id: upTr
            anchors.top: parent.top
            anchors.topMargin: 4
            anchors.left: parent.left
            anchors.leftMargin: 4
            visible: mode !== 0
            width: 8
            height: 5
            color: '#9b9da0'
            rotation: Triangle.Top

            MouseArea {
                anchors.fill: parent
                onClicked: {
                   if (up) up()
                }
                onWheel: {
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
            width: 8
            height: 5
            color: '#9b9da0'
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

    }


    Rectangle {
        id: textRectangle
        width: 38
        height: parent.height
        color: 'transparent'

        anchors.left: parent.left
        anchors.leftMargin: {
            switch (mode) {
                case 0: return 0
                case 1: return 16
                case 2: return 0
            }
        }
        Text{
            id: labelText
            anchors.centerIn: parent
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 11
            font.weight: Font.Light
            text: intSlider.minimumValue
            color: '#dbdede'
        }

        MouseArea {
            anchors.fill: parent
            onWheel: {
                if (mode === 0) return
                if (wheel.angleDelta.y > 0 && up) up()
                if (wheel.angleDelta.y < 0 && down) down()
            }
        }
    }

}
