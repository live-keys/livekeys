import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.2 as Controls
import editor 1.0
import live 1.0
import base 1.0
import visual.input 1.0 as Input

Rectangle{
    id: root
    color: style.cellBorderColor

    property var table: null

    property QtObject style: QtObject{
        property color cellBackgroundColor: '#fff'
        property color cellBorderColor: '#aaa'
        property double cellBorderSize: 1
        property QtObject cellTextStyle: Input.TextStyle{}

        property color selectedCellBorderColor: '#333'
        property color selectedCellBackgroundColor: '#aaa'
        property QtObject selectedCellTextStyle: Input.TextStyle{}

        property QtObject cellInputStyle: Input.InputBoxStyle{}

        property color headerCellBackgroundColor: '#ccc'
        property color headerCellBorderColor: '#aaa'
        property double headerCellBorderSize: 2
        property QtObject headerCellTextStyle: Input.TextStyle{}

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

    onTableChanged: {
        if (!table){
            return
        }
        rows = table.rowCount()
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
                ++cols
                headerTableView.forceLayout()
                contentTableView.forceLayout()

            }
        }
    }

    property Item cellInputBox: Input.InputBox {
         id: input
         width: 100
         height: 30
         color: "lightgray"
         border.width: 1
         radius: 0
         style: root.style.cellInputStyle
         onActiveFocusLost: {
             //TODO: Assign table value
//             model.value = input.text
//             input.enabled = false
             root.disableCellInput()
         }
     }

    property var editCell: ({ row: -1, column: -1 })
    signal cellRightClicked(int row, int column, Item delegate)
    signal cellDoubleClicked(int row, int column, Item delegate)
    onCellDoubleClicked: {
        enableCellInput(delegate, row, column)
    }

    function enableCellInput(delegate, row, column){
        editCell.row = row
        editCell.column = column

        root.cellInputBox.parent = contentTableView
        root.cellInputBox.x = Qt.binding(function(){ return delegate.x - contentTableView.contentX })
        root.cellInputBox.y = Qt.binding(function(){ return delegate.y - contentTableView.contentY })
        root.cellInputBox.width = Qt.binding(function(){ return delegate.width })
        root.cellInputBox.height = Qt.binding(function(){ return delegate.height })
        root.cellInputBox.text = delegate.value
        root.cellInputBox.forceFocus()
    }

    function disableCellInput(){
        editCell.row = -1
        editCell.column = -1
        root.cellInputBox.parent = null
    }

    function __notifyEditOutOfView(delegate){
        root.cellInputBox.x = -root.cellInputBox.width
        root.cellInputBox.y = -root.cellInputBox.height
    }
    function __notifyEditInView(delegate){
        root.cellInputBox.x = Qt.binding(function(){ return delegate.x - contentTableView.contentX })
        root.cellInputBox.y = Qt.binding(function(){ return delegate.y - contentTableView.contentY })
        root.cellInputBox.width = Qt.binding(function(){ return delegate.width })
        root.cellInputBox.height = Qt.binding(function(){ return delegate.height })
    }


    Column {
        id: colRoot
        width: root.width
        height: root.height - buttonTray.height
        anchors.top: buttonTray.bottom


        TableView {
            id: headerTableView
            width: root.width
            height: root.style.headerHeight
            clip: true
            model: !table || !table.header ? null : table.header

            interactive: false
            columnWidthProvider: table.header.columnWidth

            contentX: contentTableView.contentX
            delegate: Item{
                id: hedaerDelegate // width -> providerWidth
                implicitHeight: root.style.headerHeight

                Item {
                    id: headerColumnContainer
                    width: parent.width - root.style.borderWidth
                    onWidthChanged: {
                        if ( !dragArea.drag.active )
                            separator.x = width
                    }

                    height: parent.height

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
                        text: model.name
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

                    Drag.active: dragArea.drag.active

                    MouseArea {
                        id: dragArea
                        cursorShape: Qt.SizeHorCursor
                        anchors.fill: parent
                        drag.axis: Drag.XAxis
                        drag.target: parent
                        drag.minimumX: headerCol.x + root.style.minimumCellWidth
                    }

                    onXChanged: {
                        if (!Drag.active)
                            return

                        headerCol.width = x

                        var newWidth = x - headerCol.x + root.style.borderWidth
                        table.header.updateColumnWidth(index, newWidth)
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

                columnSpacing: root.style.cellBorderSize
                columnWidthProvider: table.header.columnWidth

                delegate: Rectangle{
                    id: tableDelegate
                    implicitHeight: root.style.defaultCellHeight
                    color: selected ? root.style.selectedCellBackgroundColor : root.style.cellBackgroundColor
                    border.width : selected ? 1 : 0
                    border.color: root.style.selectedCellBorderColor

                    property string value: model.value
                    property bool selected: false


                    Text{
                        id: cellText
                        anchors.left: parent.left
                        anchors.leftMargin: 3
                        anchors.top: parent.top
                        anchors.topMargin: 3
                        text: model.value
                    }

                    MouseArea {
                        id: cellMouseArea
                        anchors.fill: parent
                        onDoubleClicked: root.cellDoubleClicked(row, column, tableDelegate)
                        onClicked: tableDelegate.forceActiveFocus()
                    }

                    Component.onCompleted: {
                        if ( row === root.editCell.row && column === root.editCell.column){
                            root.__notifyEditInView(tableDelegate)
                        }
                    }
                    Component.onDestruction: {
                        if ( row === root.editCell.row && column === root.editCell.column){
                            root.__notifyEditOutOfView(tableDelegate)
                        }
                    }

                    TableView.onReused: {
                        if ( row === root.editCell.row && column === root.editCell.column){
                            root.__notifyEditInView(tableDelegate)
                        }
                    }
                    TableView.onPooled: {
                        if ( row === root.editCell.row && column === root.editCell.column){
                            root.__notifyEditOutOfView(tableDelegate)
                        }
                    }
                }

            }
        }

    }

}
