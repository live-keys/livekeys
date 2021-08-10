import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.2 as Controls
import editor 1.0
import live 1.0
import base 1.0
import visual.input 1.0 as Input

CodePalette {
    id: root

    type: "qml/table#Table"

    property var table: null

    item: Item {
        id: paletteItem
        width: 600
        height: 300

        TableEditor {
            anchors.fill: parent
            table: root.table
        }
    }


    onValueFromBindingChanged: {
        root.table = value
    }

    onInit: {
        root.table = value
    }
}
