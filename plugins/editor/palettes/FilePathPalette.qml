import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import workspace 1.0
import visual.input 1.0 as Input

CodePalette{
    id: palette
    type : "qml/string"

    property QtObject theme: lk.layers.workspace.themes.current

    item: Item{
        width: 180
        height: 25

        Input.PathInputBox{
            id: inputBox
            width: parent.width
            height: 25
            style: QtObject{
                property QtObject inputBoxStyle: theme.inputStyle
                property QtObject buttonStyle: theme.formButtonStyle
            }

            onPathSelected: {
                palette.value = path
                if ( !palette.isBindingChange() )
                    editFragment.write(palette.value)
            }
        }
    }

    onValueFromBindingChanged: {
        inputBox.path = value
    }
    onInit: {
        inputBox.path = value
    }

}
