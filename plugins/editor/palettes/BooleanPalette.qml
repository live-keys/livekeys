import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import workspace 1.0 as Workspace

CodePalette {
    id: palette
    type: "qml/bool"

    property bool checked: false

    item: Rectangle{
        height: 24
        width: 24
        border.color: "gray"
        border.width: 1
        color: "transparent"

        Workspace.CheckMarkIcon{
            visible: checked
            anchors.centerIn: parent
            width: 24
            height: 24
            strokeWidth: 1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                checked = !checked
            }
        }
    }

    onCheckedChanged: {
        palette.value = checked
        if ( !palette.isBindingChange() ){
            extension.write(palette.value)
        }
    }

    onInit: {
        checked = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }


}
