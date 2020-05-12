import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0

CodePalette{
    id: palette
    type : "qml/string"

    item: Item{
        width: 300
        height: 30
        Item{
            id: root
            height: 30
            width: 280

            property alias path: pathInput.text
            property alias font: pathInput.font

            InputBox{
                id: pathInput
                anchors.left: parent.left
                width: parent.width - 50
                radius: 5
                height: 30
                font.family: "Open Sans, sans-serif"
                textColor: '#afafaf'

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

            TextButton{
                anchors.right: parent.right
                radius: 5
                width: 40
                height: 30
                text: 'Apply'
                fontFamily: "Open Sans, sans-serif"
                fontPixelSize: 12

                onClicked: {
                    palette.value = pathInput.text
                    if ( !palette.isBindingChange() ){
                        extension.write(palette.value)
                    }
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
