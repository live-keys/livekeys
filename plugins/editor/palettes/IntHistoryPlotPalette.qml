import QtQuick 2.3
import editor 1.0
import live 1.0

CodePalette{
    id: palette
    type: "qml/int"

    item: Rectangle{
        width: 300
        height: 200
        color: 'transparent'

        ValueHistory{
            id: valueHistory
            anchors.fill: parent
        }
    }

    onInit: {
        valueHistory.currentValue = value
    }

    onEditFragmentChanged: {
        editFragment.whenBinding = function(){
            editFragment.write(palette.value)
        }
    }
}
