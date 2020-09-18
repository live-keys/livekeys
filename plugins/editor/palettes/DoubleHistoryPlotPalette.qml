import QtQuick 2.3
import editor 1.0
import live 1.0

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

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }

    onInit: {
        valueHistory.currentValue = value
    }

}
