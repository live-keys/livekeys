import QtQuick 2.3
import editor 1.0
import live 1.0
import visual.shapes 1.0

CodePalette{
    id: palette
    type: "qml/double"

    property color lineColor: '#141b20'

    item: Rectangle{
        width: 300
        height: 200
        color: 'transparent'
        border.width: 1
        border.color: palette.lineColor

        LineGrid{
            color: palette.lineColor
            anchors.fill: parent
            colSpacing: parent.width / 12
            rowSpacing: parent.height / 8
        }

        ValueHistory{
            id: valueHistory
            anchors.fill: parent
        }
    }

    onInit: {
        valueHistory.currentValue = value
        editFragment.whenBinding = function(){
            editFragment.write(palette.value)
        }
    }
    onValueFromBindingChanged: {
        valueHistory.currentValue = value
    }

}
