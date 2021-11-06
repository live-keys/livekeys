import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.2 as Controls
import editor 1.0
import live 1.0
import base 1.0
import visual.input 1.0 as Input
import table 1.0

CodePalette {
    id: root

    type: "qml/table#Table"

    property var table: null

    item: Item {
        id: paletteItem
        width: 450
        height: 250

        TableEditor{
            anchors.fill: parent
            tableModel: root.table ? root.table.model : null
            style: lk.layers.workspace.themes.current.tableStyle

            //TODO: Move to extension
//            handleContextMenu: function(item, options){
//                var pane = null
//                var p = parent
//                while ( p ){
//                    if ( p.paneType !== undefined ){
//                        break
//                    }
//                    p = p.parent
//                }

//                if (p){
//                    lk.layers.workspace.panes.activateItem(item, p)
//                    lk.layers.workspace.panes.openContextMenu(item, p)

//                }
//            }
        }

        ResizeArea{
            minimumHeight: 200
            minimumWidth: 200
        }
    }


    onValueFromBindingChanged: {
        root.table = value
    }

    onInit: {
        root.table = value
    }
}
