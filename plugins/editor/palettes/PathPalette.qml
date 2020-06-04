import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0

CodePalette{
    id: palette
    type : "qml/string"

    item: Item{
        width: 180
        height: 30
        PathInputBox{
            id: inputBox
            anchors.fill: parent

            onPathSelected: {
                palette.value = path
                if ( !palette.isBindingChange() )
                    extension.write(palette.value)
            }
        }
    }

    onInit: {
        inputBox.path = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }
}
