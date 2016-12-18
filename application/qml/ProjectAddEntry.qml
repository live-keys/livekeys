import QtQuick 2.3
import Cv 1.0

Rectangle{
    id: root

    visible: false
    color: "transparent"

    property var entry: null
    property bool isFile : false


    onVisibleChanged: {
        if ( !visible ){
            close()
            canceled()
        }
    }

    function show(entry, isFile){
        root.entry = entry
        root.isFile = isFile
        root.visible = true
        addEntryInput.forceActiveFocus()
    }
    function close(){
        root.entry = null
        addEntryInput.text = ''
        visible = false
    }


    signal accepted(ProjectEntry entry, string name, bool isFile)
    signal canceled()

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
        width: parent.width
        height: 60
        color:"#555"

        opacity: root.visible ? 1 : 0
        Behavior on opacity{ NumberAnimation{ duration: 250} }

        Text{
            anchors.left: parent.left
            width: parent.width
            text: 'Add ' + (root.isFile ? 'file' : 'directory') + ' in ' + (root.entry ? root.entry.path : '')
            font.family: 'Open Sans, Arial, sans-serif'
            font.pixelSize: 12
            font.weight: Font.Light
        }
        Rectangle{
            anchors.bottom: parent.bottom
            width: parent.width
            height: 30
            color: "#333"

            TextInput{
                id: addEntryInput
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.top: parent.top
                color: '#aaa'
                text: ''
                font.family: 'Open Sans, Arial, sans-serif'
                font.pixelSize: 12
                font.weight: Font.Light
                selectByMouse: true

                Keys.onReturnPressed: {
                    root.accepted(root.entry, text, root.isFile)
                    root.close()
                }
                Keys.onEscapePressed: {
                    root.close()
                    root.canceled()
                }
                MouseArea{
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: Qt.IBeamCursor
                }
            }
        }
    }
}
