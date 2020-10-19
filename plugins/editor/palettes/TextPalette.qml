import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import workspace 1.0 as Workspace

CodePalette{
    id: palette
    type : "qml/string"

    property QtObject theme: lk.layers.workspace.themes.current

    item: Item{
        id: root
        width: 300
        height: 25

        property alias path: pathInput.text
        property alias font: pathInput.font

        Workspace.InputBox{
            id: pathInput
            anchors.left: parent.left
            width: parent.width - 30
            height: 25

            style: theme.inputStyle

            onKeyPressed: {
                if ( event.key === Qt.Key_Return ){
                    palette.value = pathInput.text
                    if ( !palette.isBindingChange() ){
                        extension.write(palette.value)
                    }
                    event.accepted = true
                }
            }
        }

        Workspace.Button{
            anchors.right: parent.right
            width: 30
            height: 25
            content: theme.buttons.apply
            onClicked: {
                palette.value = pathInput.text
                if ( !palette.isBindingChange() ){
                    extension.write(palette.value)
                }
            }
        }

    }

    onInit: {
        root.path = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }
}
