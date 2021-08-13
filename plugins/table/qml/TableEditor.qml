import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.2 as Controls
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import base 1.0
import workspace.icons 1.0 as Icons
import visual.input 1.0 as Input

Rectangle{
    id: root
    color: style.cellBorderColor
    objectName: "tableEditor"

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

        property color iconColor: 'black'

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

    property var getContextMenuOptions: function(item, options){
        if ( item.objectName === "tableEditor" ){
            return [{
                name: "Add column",
                enabled: true,
                action: function(){
                    table.addColumns(1)
                }
            },
            {
                name: "Add row",
                enabled: true,
                action: function(){
                    root.table.addRows(1)
                }
            }]
        } else if ( item.objectName === "tableDelegate" ){
            return [{
                name: "Remove row",
                enabled: true,
                action: function(){
                    //TODO
                }
            },{
                name: "Remove row",
                enabled: true,
                action: function(){
                    //TODO
                }
            }]
        }

        return []
    }

    property var handleContextMenu: function(item, options){
        var menuOptions = getContextMenuOptions(item, options)

        for ( var i = 0; i < contextMenu.toClear.length; ++i ){
            contextMenu.removeItem(contextMenu.toClear[i])
        }

        for ( var i = 0; i < menuOptions.length; ++i ){
            var menuitem = contextMenu.insertItem(i, menuOptions[i].name)
            menuitem.enabled = menuOptions[i].enabled
            menuitem.triggered.connect(menuOptions[i].action)
            contextMenu.toClear.push(menuitem)
        }

        contextMenu.popup()
    }

    Controls.Menu{
        id: contextMenu
        property var toClear: []
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
    signal cellClicked(int row, int column, Item delegate)
    onCellClicked: {
//        console.log("CELL SELECT:" + column + ";" + row)
    }

    signal cellRightClicked(int row, int column, Item delegate)
    onCellRightClicked: {
        console.log("CELL RIGHT CLICKED")
        handleContextMenu(delegate, {row: row, column: column})
    }

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

    Item{
        id: tableOptions
        width: root.style.rowInfoWidth
        height: root.style.headerHeight

        Icons.MenuIcon{
            anchors.centerIn: parent
            height: 10
            width: 10
            color: root.style.iconColor
        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                root.handleContextMenu(root)
            }
        }
    }

    TableView {
        id: rowTableView
        width: root.style.rowInfoWidth
        height: parent.height - root.style.headerHeight
        clip: true
        model: !table || !table.rowInfo ? 0 : table.rowInfo

        anchors.top: tableOptions.bottom

        interactive: false
        rowHeightProvider: table ? table.rowInfo.rowHeight : null

        contentY: contentTableView.contentY
        delegate: Item{
            id: rowInfoDelegate
            implicitWidth: root.style.rowInfoWidth

            Item{
                id: rowInfoContainer
                width: parent.width
                height: parent.height - root.style.borderWidth
                onHeightChanged: {
                    if ( !dragAreaY.drag.active )
                      separatorY.y = height
                }

                Input.InputBox {
                    id: rowBox
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
                id: separatorY
                z: 10

                height: root.style.borderWidth
                width: parent.width
                color: root.style.separatorColor

                Drag.active: dragAreaY.drag.active

                MouseArea {
                    id: dragAreaY
                    cursorShape: Qt.SizeVerCursor
                    anchors.fill: parent
                    drag.axis: Drag.YAxis
                    drag.target: parent
                    drag.minimumY: rowBox.y + root.style.minimumCellHeight
                }

                onYChanged: {
                    if (!Drag.active)
                        return

                    rowBox.height = y

                    var newHeight = y - rowBox.y + root.style.borderWidth
                    table.rowInfo.updateRowHeight(index, newHeight)
                    rowTableView.forceLayout()
                    contentTableView.forceLayout()
                }
            }
        }

    }

    TableView {
        id: headerTableView
        width: parent.width - root.style.rowInfoWidth
        height: root.style.headerHeight
        clip: true
        model: !table || !table.header ? null : table.header
        anchors.left: tableOptions.right

        interactive: false
        columnWidthProvider: table ? table.header.columnWidth : null

        contentX: contentTableView.contentX
        delegate: Item{
            id: hedaerDelegate // width -> providerWidth
            implicitHeight: root.style.headerHeight

            Item {
                id: headerColumnContainer
                width: parent.width - root.style.borderWidth
                onWidthChanged: {
                    if ( !dragAreaX.drag.active )
                        separatorX.x = width
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
                id: separatorX
                z: 10

                height: root.style.headerHeight
                width: root.style.borderWidth
                color: root.style.separatorColor

                Drag.active: dragAreaX.drag.active

                MouseArea {
                    id: dragAreaX
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
        anchors.left: rowTableView.right
        anchors.top: headerTableView.bottom

        height: contentTableView.contentHeight + 10 < parent.height - root.style.headerHeight
                ? contentTableView.contentHeight + 10
                : parent.height - root.style.headerHeight
        width: contentTableView.contentWidth + 10 < parent.width - root.style.rowInfoWidth
               ? contentTableView.contentWidth + 10
               : parent.width - root.style.rowInfoWidth

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
                    color: root.style.cellBackgroundColor
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Item{}
            corner: Rectangle{color: root.style.cellBackgroundColor}
        }

        TableView {
            id: contentTableView
            anchors.fill: parent

            contentWidth: table ? table.header.contentWidth : 0
            contentHeight: table ? table.rowInfo.contentHeight : 0

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
                objectName: "tableDelegate"
                color: root.style.cellBackgroundColor
                border.width : selected ? 1 : 0
                border.color: root.style.selectedCellBorderColor

                property string value: model.value
                property bool selected: model.isSelected

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
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onDoubleClicked: root.cellDoubleClicked(row, column, tableDelegate)
                    onClicked: {
                        if ( mouse.button === Qt.RightButton ){
                            root.cellRightClicked(row, column, tableDelegate)
                        } else if ( mouse.button === Qt.LeftButton ){
                            if (mouse.modifiers & Qt.ControlModifier){
                                if ( model.isSelected ){
                                    //TODO
                                } else {
                                    table.select(column, row)
                                }

                            } else {
                                table.deselect()
                                table.select(column, row)
                            }

                            root.cellClicked(row, column, tableDelegate)
                            tableDelegate.forceActiveFocus()
                        }

                    }
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
