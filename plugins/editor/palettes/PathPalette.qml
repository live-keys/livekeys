import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import workspace 1.0

CodePalette{
    id: palette
    type : "qml/string"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    item: Item{
        width: 180
        height: 25

        PathInputBox{
            id: inputBox
            width: parent.width
            height: 25
            style: QtObject{
                property QtObject inputBoxStyle: palette.paletteStyle ? palette.paletteStyle.inputStyle : inputBox.defaultStyle.inputBoxStyle
                property QtObject buttonStyle: palette.paletteStyle ? palette.paletteStyle.buttonStyle : inputBox.defaultStyle.buttonStyle
            }

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
