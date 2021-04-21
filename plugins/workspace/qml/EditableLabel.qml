import QtQuick 2.3
import live 1.0
import workspace 1.0 as Workspace

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
    
    Workspace.Label{
        id: labelText
        anchors.fill: parent
        anchors.margins: parent.margins
        visible: !parent.isEditing
        text: parent.text
        textStyle: root.style.textStyle
    }
    
    Workspace.InputBox{
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
        onDoubleClicked: {
            parent.isEditing = !parent.isEditing
            if ( parent.isEditing )
                inputBox.selectAll()
        }
    }
}

