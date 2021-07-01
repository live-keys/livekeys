import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import visual.input 1.0 as Input

CodePalette {
    id: palette
    type: "qml/size"
    property QtObject theme: lk.layers.workspace.themes.current

    item: Item{
        height: 24
        width: 160

        Input.InputBox{
            id: widthInput
            anchors.left: parent.left
            width: 50
            height: 24

            style: theme.inputStyle

            validator: IntValidator {bottom: 0}

            onKeyPressed: {
                if ( event.key === Qt.Key_Return ){
                    palette.value = widthInput.text + "x" + heightInput.text
                    if ( !palette.isBindingChange() ){
                        editFragment.write(palette.value)
                    }
                    event.accepted = true
                }
            }
        }

        Text {
            anchors.left: widthInput.right
            anchors.leftMargin: 6
            anchors.top: parent.top
            anchors.topMargin: 3
            text: 'x'
            color: "white"
        }

        Input.InputBox{
            id: heightInput
            anchors.left: widthInput.right
            anchors.leftMargin: 20
            width: 50
            height: 24

            style: theme.inputStyle
            validator: IntValidator {bottom: 0}

            onKeyPressed: {
                if ( event.key === Qt.Key_Return ){
                    palette.value = widthInput.text + "x" + heightInput.text
                    if ( !palette.isBindingChange() ){
                        editFragment.write(palette.value)
                    }
                    event.accepted = true
                }
            }
        }

        Input.Button{
            anchors.right: parent.right
            width: 30
            height: 25
            content: theme.buttons.apply
            onClicked: {
                palette.value = widthInput.text + "x" + heightInput.text
                if ( !palette.isBindingChange() ){
                    editFragment.write(palette.value)
                }
            }
        }
    }

    onInit: {
        widthInput.text = value.width
        heightInput.text = value.height
        editFragment.whenBinding = function(){
            editFragment.write(palette.value)
        }
    }
    onValueFromBindingChanged: {
        widthInput.text = value.width
        heightInput.text = value.height
    }
}
