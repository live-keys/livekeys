import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons

CodePalette {
    id: palette
    type: "qml/bool"

    property bool checked: false

    item: Rectangle{
        height: 24
        width: 24
        border.color: "#232b30"
        border.width: 1
        color: "transparent"

        Icons.CheckMarkIcon{
            visible: checked
            anchors.centerIn: parent
            width: 8
            height: 8
            strokeWidth: 2
            color: "#9b9da0"
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
            editFragment.write(palette.value)
        }
    }

    onInit: {
        checked = value
    }
    onValueFromBindingChanged: {
        checked = value
    }

}
