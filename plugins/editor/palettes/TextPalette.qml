import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import visual.input 1.0 as Input

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

        Input.InputBox{
            id: pathInput
            anchors.left: parent.left
            width: parent.width - 30
            height: 25

            style: theme.inputStyle
            onTextChanged: commitButton.visible = true

            onKeyPressed: {
                if ( event.key === Qt.Key_Return ){
                    palette.value = pathInput.text
                    if ( !palette.isBindingChange() ){
                        editFragment.write(palette.value)
                    }
                    commitButton.visible = false
                    event.accepted = true
                }
            }
        }

        Input.Button{
            id: commitButton
            anchors.right: parent.right
            width: 30
            height: 25
            content: theme.buttons.apply
            onClicked: {
                palette.value = pathInput.text
                if ( !palette.isBindingChange() ){
                    editFragment.write(palette.value)
                }
                commitButton.visible = false
            }
        }

    }

    onValueFromBindingChanged: {
        root.path = value
    }
    onInit: {
        root.path = value
        editFragment.whenBinding = function(){
            editFragment.write(palette.value)
        }
    }

}
