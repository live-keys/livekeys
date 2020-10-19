import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import workspace 1.0 as Workspace

CodePalette {
    id: palette
    type: "qml/size"
    property QtObject theme: lk.layers.workspace.themes.current

    item: Item{
        height: 24
        width: 160

        Workspace.InputBox{
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
                        extension.write(palette.value)
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

        Workspace.InputBox{
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
                palette.value = widthInput.text + "x" + heightInput.text
                if ( !palette.isBindingChange() ){
                    extension.write(palette.value)
                }
            }
        }
    }

    onInit: {
        widthInput.text = value.width
        heightInput.text = value.height
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }


}
