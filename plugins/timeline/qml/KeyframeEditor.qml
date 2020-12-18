import QtQuick 2.0
import timeline 1.0
import lcvcore 1.0
import live 1.0
import editor 1.0
import workspace 1.0 as Workspace

Rectangle{
    id: root
    width: 300
    height: 40
    color: '#03070a'

    property QtObject currentTheme: lk.layers.workspace ? lk.layers.workspace.themes.current : null

    property Keyframe currentSegment: null
    signal ready()

    property alias inputBox: inputBox

    Text{
        id: label
        anchors.top: parent.top
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 14
        text: 'Keyframe ' + (root.currentSegment ? (' at ' + root.currentSegment.position) : '')
        font.family: 'Open Sans, Arial, sans-serif'
        font.pixelSize: 12
        font.weight: Font.Normal
        color: "#afafaf"
    }

    Workspace.InputBox{
        id: inputBox
        anchors.right: parent.right
        anchors.rightMargin: 60
        anchors.top: parent.top
        anchors.topMargin: 4
        width: 100
        height: 25
        text: currentSegment ? currentSegment.value : 0
    }

    Workspace.Button{
        anchors.top: parent.top
        anchors.topMargin: 4
        anchors.right: parent.right
        anchors.rightMargin: 30
        width: 30
        height: 25
        content: root.currentTheme ? root.currentTheme.buttons.apply : null
        onClicked: {
            root.currentSegment.value = inputBox.text
            root.ready()
        }
    }


    TextButton{
        anchors.top: parent.top
        anchors.topMargin: 4
        anchors.right: parent.right
        anchors.rightMargin: 0
        radius: 5
        width: 30
        height: 25
        text: 'X'
        onClicked: {
            parent.ready()
        }
    }

}
