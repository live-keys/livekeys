import QtQuick 2.3

Rectangle {
    id: root

    width: 500
    height: 30

    property string key : "key"
    property string label : "Label"

    property alias editor : editorContainer.children

    signal dataChanged(string data)

    Rectangle{
        id: labelContainer
        width: 200
        height: parent.height

        Text{
            id : label

            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter : parent.verticalCenter

            text: root.label
            font.pixelSize : 13
            font.family : "Courier New, Courier"
            font.weight: Font.Normal
        }
    }


    Rectangle{
        id: editorContainer
        color: "#ff0000"
        height: parent.height
        width: parent.width - labelContainer.width
        anchors.right: parent.right

    }
}

