import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.2 as Controls
import QtQuick.Controls.Styles 1.2
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
        property var rowInfoWidth: 50
        property var headerWidth: 40
        property var borderWidth: 2
        property var minimumCellWidth: 40
        property var minimumCellHeight: 15
    }

    onTableChanged: {
        if (!table){
            return
        }
        headerTableView.forceLayout()
        rowTableView.forceLayout()
        contentTableView.forceLayout()
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
             root.table.assignCell(editCell.row, editCell.column, input.text)
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
        width: root.style.defaultCellWidth / 2
        height: root.height
        Rectangle {
            width: root.style.defaultCellWidth / 2
            height: root.style.headerHeight
            color: "red"

            Row {
                id: buttonTray
                width: 40
                height: 30

                Input.RectangleButton{
                    width: 20
                    height: 30

                    content: Input.Label {
                        text: "R"
                    }
                    onClicked: root.table.addRows(1)
                }

                Input.RectangleButton{
                    width: 20
                    height: 30

                    content: Input.Label {
                        text: "C"
                    }

                    onClicked: table.addColumns(1)
                }
            }
        }

        TableView {
            id: rowTableView
            width: parent.width
            height: parent.height - root.style.defaultCellHeight
            clip: true
            model: !table || !table.rowInfo ? 0 : table.rowInfo

            interactive: false
            rowHeightProvider: table.rowInfo.rowHeight

            contentY: contentTableView.contentY
            delegate: Item{
                id: rowInfoDelegate // width -> providerWidth
                implicitWidth: root.style.rowInfoWidth

                Item{
                    id: rowInfoContainer
                    width: parent.width
                    height: parent.height - root.style.borderWidth
                    onHeightChanged: {
                        if ( !dragArea.drag.active )
                          separator.y = height
                    }

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
                        onDoubleClicked: {}
                    }
                }


                Rectangle {
                    id: separator
                    z: 10

                    height: root.style.borderWidth
                    width: parent.width
                    color: root.style.separatorColor

                    Drag.active: dragArea.drag.active

                    MouseArea {
                        id: dragArea
                        cursorShape: Qt.SizeVerCursor
                        anchors.fill: parent
                        drag.axis: Drag.YAxis
                        drag.target: parent
                        drag.minimumY: headerCol.y + root.style.minimumCellHeight
                    }

                    onYChanged: {
                        if (!Drag.active)
                            return

                        headerCol.height = y

                        var newHeight = y - headerCol.y + root.style.borderWidth
                        table.rowInfo.updateRowHeight(index, newHeight)
                        rowTableView.forceLayout()
                        contentTableView.forceLayout()
                    }
                }
            }
        }
    }


    Column {
        id: colRoot
        width: root.width - root.style.defaultCellWidth / 2
        height: root.height
        anchors.left: parent.left
        anchors.leftMargin: root.style.defaultCellWidth / 2

        TableView {
            id: headerTableView
            width: parent.width
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

//            width: 500
            height: contentTableView.contentHeight + 10 < parent.height ? contentTableView.contentHeight + 10 : parent.height
            width: contentTableView.contentWidth + 10 < parent.width ? contentTableView.contentWidth + 10 : parent.width

            //            height: contentTableView.contentHeight < parent.height-root.style.headerHeight
            //                  ? contentTableView.contentHeight
            //                  : parent.height-root.style.headerHeight
            style: ScrollViewStyle {
                transientScrollBars: false
                handle: Item {
                    implicitWidth: 10
                    implicitHeight: 10
                    Rectangle {
                        color: "#1f2227"
                        anchors.fill: parent
                    }
                }
                scrollBarBackground: Item{
                    implicitWidth: 10
                    implicitHeight: 10
                    Rectangle{
                        anchors.fill: parent
                        color: root.color
                    }
                }
                decrementControl: null
                incrementControl: null
                frame: Item{}
                corner: Rectangle{color: root.color}
            }

            TableView {
                id: contentTableView
                anchors.fill: parent
//                contentWidth: 1headerTableView.contentWidth
//                contentHeight: 100//rowTableView.contentHeight
                clip: true
                model: table

                columnSpacing: root.style.cellBorderSize
                columnWidthProvider: function(column){
                    return table.header.columnWidth(column) - 1
                }
                rowSpacing: root.style.cellBorderSize
                rowHeightProvider: function(row){
                    return table.rowInfo.rowHeight(row) - 1
                }

                delegate: Rectangle{
                    id: tableDelegate
                    implicitHeight: root.style.defaultCellHeight
                    implicitWidth: 40
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
