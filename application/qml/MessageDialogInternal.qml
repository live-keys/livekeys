
import QtQuick 2.3
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2

Rectangle{
    id: root
    property string message : ""

    visible: false
    onVisibleChanged: if (!visible) message = "";

    signal ok()
    signal cancel()

    default property alias children: buttonContainer.children

    color: "transparent"
    property alias backgroudColor: box.color

    MouseArea{
        anchors.fill: parent
        onClicked: mouse.accepted = true;
        onPressed: mouse.accepted = true;
        onReleased: mouse.accepted = true
        onDoubleClicked: mouse.accepted = true;
        onPositionChanged: mouse.accepted = true;
        onPressAndHold: mouse.accepted = true;
        onWheel: wheel.accepted = true
    }

    Rectangle{
        anchors.fill: parent
        color: "#000"
        opacity: root.visible ? 0.7 : 0
        Behavior on opacity{ NumberAnimation{ duration: 250} }
    }

    Rectangle{
        id: box
        anchors.centerIn: parent
        color: "#333"
        width: 400
        height: messageLabel.height < 100 ? messageLabel.height + 100 : 200
        focus: true
        opacity: root.visible ? 1 : 0
        Behavior on opacity{ NumberAnimation{ duration: 250} }

        Keys.onEscapePressed: {
            root.visible = false
            root.cancel()
        }

        ScrollView{
            anchors.fill: parent
            anchors.bottomMargin: 60

            Text{
                id: messageLabel
                anchors.top: parent.top
                anchors.topMargin: 12
                anchors.left: parent.left
                anchors.leftMargin: 12

                text: root.message

                width: box.width - 35

                wrapMode: Text.WordWrap

                color: "#ccc"
                font.pixelSize: 12
                font.family: "Open Sans, sans-serif"
                font.weight: Font.Light
            }

        }

        Rectangle{
            id: buttonContainer
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.right: parent.right
            anchors.rightMargin: 15
            width: parent.width
            height: 30
            color: "transparent"
        }

    }
}
