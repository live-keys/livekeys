import QtQuick 2.3
import editor 1.0
import live 1.0

CodePalette{
    id: palette
    type: "qml/double"

    item: Rectangle{
        width: 300
        height: 200
        color: 'transparent'

        ValueHistory{
            id: valueHistory
            anchors.fill: parent
        }
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }

    onInit: {
        valueHistory.currentValue = value
    }

}
