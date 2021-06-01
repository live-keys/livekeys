import QtQuick 2.3
import live 1.0
import workspace 1.0 as Workspace
import visual.input 1.0 as Input

Item{
    id: root
    width: 100
    height: 30

    property QtObject defaultStyle: inputBox.defaultStyle
    property QtObject style: defaultStyle

    property alias font: inputBox.font
    property string text: 'label'
    property real margins: 6
    property color textColor: 'white'
    property alias boxBackgroundColor: inputBox.color

    property bool isEditing : false

    signal rightClicked(var mouse)

    Input.Label{
        id: labelText
        anchors.fill: parent
        anchors.margins: parent.margins
        visible: !parent.isEditing
        text: parent.text
        textStyle: root.style.textStyle
    }

    Input.InputBox{
        id: inputBox
        text: parent.text
        visible: parent.isEditing
        margins: parent.margins
        width: parent.width
        height: parent.height
        style: root.style
        onKeyPressed: {
            if ( event.key === Qt.Key_Return ){
                event.accepted = true
                root.isEditing = !root.isEditing
                root.text = inputBox.text
            } else if ( event.key === Qt.Key_Escape ){
                root.isEditing = !root.isEditing
                event.accepted = true
            }
        }
    }

    MouseArea{
        anchors.fill: parent
        visible: !parent.isEditing
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onDoubleClicked: {
            parent.isEditing = !parent.isEditing
            if ( parent.isEditing )
                inputBox.selectAll()
        }
        onClicked: {
            if ( mouse.button === Qt.RightButton)
                root.rightClicked(mouse)
        }

    }
}

