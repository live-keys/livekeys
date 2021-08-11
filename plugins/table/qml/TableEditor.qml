import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.2 as Controls
import editor 1.0
import live 1.0
import base 1.0
import visual.input 1.0 as Input

Item {
    id: root

    property var table: null

    property QtObject __theme: lk.layers.workspace.themes.current

    property QtObject style: QtObject{
        property Component addButton: __theme.buttons.add
        property color headerColor: "#333333"
        property color separatorColor: "white"
        property color borderColor: "black"
        property var defaultCellWidth: 90
        property var defaultCellHeight: 25
        property var headerHeight: 25
        property var headerWidth: 40
        property var borderWidth: 2
        property var minimumCellWidth: 40
    }

    property int rows: 3
    property int cols: 5

    onTableChanged: {
        if (!table){
            rows = cols = 0
            return
        }
        rows = table.rowCount()
        cols = table.columnCount()
        var colWidths = []
        for (var i=0; i < cols; ++i){
            colWidths.push(root.style.defaultCellWidth + root.style.borderWidth)
        }
        headerTableView.columnWidths = colWidths
        headerTableView.contentWidth = cols > 0 ? cols*colWidths[0] : 0
        headerTableView.forceLayout()
        contentTableView.forceLayout()
    }

    Row {
        id: buttonTray
        width: root.width
        height: 50

        Input.RectangleButton{
            width: 90
            height: 40

            content: Input.Label {
                text: "Add row"
            }

            onClicked: {
                table.addRow()
                ++rows
           }
        }

        Input.RectangleButton{
            width: 90
            height: 40

            content: Input.Label {
                text: "Add column"
            }

            onClicked: {
                table.addColumn()
                headerTableView.columnWidths.push(root.style.defaultCellWidth + root.style.borderWidth)
                ++cols

                headerTableView.contentWidth += root.style.defaultCellWidth + root.style.borderWidth
                headerTableView.forceLayout()
                contentTableView.forceLayout()

            }
        }
    }

    Column {
        id: colRoot
        width: root.width
        height: root.height-buttonTray.height
        anchors.top: buttonTray.bottom


        TableView {
            id: headerTableView
            width: root.width
            height: root.style.headerHeight
            clip: true
            model: !table || !table.headerModel ? 0 : table.headerModel

            property var columnWidths: []
            property var contentWidth: 0

            interactive: false
            columnWidthProvider: function(column){
                return columnWidths[column]
            }

            contentX: contentTableView.contentX
            delegate: Row {

                Item {
                    width: parent.width - root.style.borderWidth
                    height: root.style.headerHeight

                    Input.InputBox {
                        id: headerCol
                        anchors.fill: parent
                        color: root.style.headerColor
                        border.width: 0
                        radius: 0
                        style: Input.InputBoxStyle {
                            textStyle: Input.TextStyle{
                                color: "white"
                            }

                        }
                        text: model.value
                        enabled: false

                    }

                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
//                            table.removeColumn(column)
//                            --cols
                        }
                    }
                }


                Rectangle {
                    id: separator
                    z: 10
                    height: root.style.headerHeight
                    width: root.style.borderWidth
                    color: root.style.separatorColor

                    property var __tempContentWidth: 0


                    Drag.active: dragArea.drag.active

                    Drag.onActiveChanged: {
                        if (Drag.active){
                            __tempContentWidth = 0
                            for (var i=0; i < headerTableView.columnWidths.length; ++i){
                                if (i === index) continue
                                __tempContentWidth += headerTableView.columnWidths[i]
                            }
                        }
                    }

                    MouseArea {
                        id: dragArea
                        cursorShape: Qt.SizeHorCursor
                        anchors.fill: parent
                        drag.axis: Drag.XAxis
                        drag.target: parent
                        drag.minimumX: headerCol.x + root.style.minimumCellWidth
                    }

                    onXChanged: {
                        if (!Drag.active) return
                        headerTableView.columnWidths[index] = x - headerCol.x + root.style.borderWidth

                        headerTableView.contentWidth = __tempContentWidth + headerTableView.columnWidths[index]

                        headerTableView.forceLayout()
                        contentTableView.forceLayout()

                    }
                }
            }
        }

        Controls.ScrollView {
            id: scrollView
            width: contentTableView.contentWidth < root.width ? contentTableView.contentWidth : root.width
            height: colRoot.height-root.style.headerHeight
            TableView {
                id: contentTableView
                contentWidth: headerTableView.contentWidth
                contentHeight: rows*(root.style.defaultCellHeight + root.style.borderWidth)
                clip: true
                model: table

                columnWidthProvider: function(column){
                    return headerTableView.columnWidths[column]

                }

                delegate: Item {

                    height: root.style.defaultCellHeight
                    implicitHeight: height
                    Input.InputBox {
                        id: input
                        anchors.fill: parent
                        color: enabled ? "lightgray" : "white"
                        border.width: 1
                        radius: 0
                        style: Input.InputBoxStyle {
                            textStyle: Input.TextStyle{
                                color: "black"
                            }

                        }
                        text: model.value
                        enabled: false

                        onActiveFocusLost: {
                            model.value = input.text
                            input.enabled = false
                        }
                    }


                    MouseArea {
                        id: ma
                        enabled: !input.enabled
                        anchors.fill: parent
                        onDoubleClicked: {
                            input.enabled = true
                            input.forceFocus()
                        }


                    }

                    Keys.onReturnPressed: {
                        model.value = input.text
                        input.enabled = false
                    }
                }

            }
        }

    }

}
