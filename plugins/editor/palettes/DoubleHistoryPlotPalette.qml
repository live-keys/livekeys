import QtQuick 2.3
import editor 1.0
import live 1.0

LivePalette{
    id: palette

    type: "double"
    serialize: NativeValueCodeSerializer{}

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
    onCodeChanged: {
        valueHistory.currentValue = value
    }

}
