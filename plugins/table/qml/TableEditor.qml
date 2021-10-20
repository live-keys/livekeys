import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.2 as Controls
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import base 1.0
import fs 1.0
import table 1.0
import workspace.icons 1.0 as Icons
import visual.input 1.0 as Input

Rectangle{
    id: root
    color: style.cellBorderColor
    objectName: "tableEditor"

    property var table: null
    property var tableModel: table ? table.model : null

    property QtObject style: TableEditorStyle{}

    onTableModelChanged: {
        if (!tableModel){
            return
        }
        headerTableView.forceLayout()
        rowTableView.forceLayout()
        contentTableView.forceLayout()
    }

    function createColumnName(n){
        var ordA = 'A'.charCodeAt(0);
        var ordZ = 'Z'.charCodeAt(0);
        var len = ordZ - ordA + 1;

        var s = "";
        while(n >= 0) {
            s = String.fromCharCode(n % len + ordA) + s;
            n = Math.floor(n / len) - 1;
        }
        return s;
    }

    function assignCell(row, column, value){
        var ob = {}
        ob[column] = value
        tableModel.updateRow(row, ob)
    }

    property var getContextMenuOptions: function(item, options){
        if ( !tableModel )
            return []
        if ( item.objectName === "tableEditor" ){
            var initial = [{
                name: "Add column",
                enabled: true,
                action: function(){
                    var fieldNo = tableModel.totalFields()
                    var fieldName = createColumnName(fieldNo)
                    tableModel.insertField(-1, {name: fieldName})
                }
            },
            {
                name: "Add row",
                enabled: true,
                action: function(){
                    root.tableModel.insertRow([])
                }
            },
            {
                name: "Clear table",
                enabled: true,
                action: function(){
                    root.tableModel.clearTable()
                }
            }]

            if ( tableModel && (tableModel.table instanceof LocalTable) ){
                initial.push({
                     name: "Save...",
                     enabled: true,
                     action: function(){
                         lk.layers.window.dialogs.saveFile(
                             { filters: [ "Csv Files (*.csv)", "All files (*)" ] },
                             function(url){
                                 var path = UrlInfo.toLocalFile(url)
                                 tableModel.table.writeToFile(path, {})
                             }
                         )
                     }
                 }, {
                     name: "Save With Header...",
                     enabled: true,
                     action: function(){
                         lk.layers.window.dialogs.saveFile(
                             { filters: [ "Csv Files (*.csv)", "All files (*)" ] },
                             function(url){
                                 var path = UrlInfo.toLocalFile(url)
                                 tableModel.table.writeToFile(path, {header: true})
                             }
                         )
                     }
                 }, {
                     name: "Open...",
                     enabled: true,
                     action: function(){
                         lk.layers.window.dialogs.openFile(
                             { filters: [ "Csv Files (*.csv)", "All files (*)" ] },
                             function(url){
                                 var path = UrlInfo.toLocalFile(url)
                                 tableModel.table.readFromFile(path)
                             }
                         )
                     }
                 })
            }

            return initial

        } else if ( item.objectName === "tableDelegate" ){
            return [
            {
                name: "Remove row",
                enabled: true,
                action: function(){
                    root.tableModel.removeRow(options.row)
                }
            },
            {
                name: "Remove column",
                enabled: true,
                action: function(){
                    root.tableModel.removeColumn(options.column)
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
         border.width: 1
         radius: 0
         style: root.style.cellInputStyle
         onActiveFocusLost: {
             root.assignCell(editCell.row, editCell.column, input.text)
             root.disableCellInput()
         }
         onKeyPressed: {
             if ( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ){
                 root.assignCell(editCell.row, editCell.column, input.text)
                 root.disableCellInput()
                 event.accepted = true
             } else if ( event.key === Qt.Key_Escape ){
                 root.disableCellInput()
                 event.accepted = true
             }
         }
     }

    Keys.onPressed: {
        if ( event.key === Qt.Key_Up ){
            if ( focusCell.row > -1 && focusCell.column > -1 ){
                if ( focusCell.row > 0 ){
                    focusCell = { row: focusCell.row - 1, column: focusCell.column }
                    tableModel.deselect()
                    tableModel.select(focusCell.column, focusCell.row)
                }
            }
        } else if ( event.key === Qt.Key_Down ){
            if ( focusCell.row > -1 && focusCell.column > -1 ){
                if ( focusCell.row < tableModel.totalRows() - 1 ){
                    focusCell = { row: focusCell.row + 1, column: focusCell.column }
                    tableModel.deselect()
                    tableModel.select(focusCell.column, focusCell.row)
                }
            }
        } else if ( event.key === Qt.Key_Left ){
            if ( focusCell.row > -1 && focusCell.column > -1 ){
                if ( focusCell.column > 0 ){
                    focusCell = { row: focusCell.row , column: focusCell.column - 1 }
                    tableModel.deselect()
                    tableModel.select(focusCell.column, focusCell.row)
                }
            }
        } else if ( event.key === Qt.Key_Right ){
            if ( focusCell.row > -1 && focusCell.column > -1 ){
                if ( focusCell.column < tableModel.totalFields() - 1 ){
                    focusCell = { row: focusCell.row , column: focusCell.column + 1 }
                    tableModel.deselect()
                    tableModel.select(focusCell.column, focusCell.row)
                }
            }
        }
    }

    property var focusCell: ({ row: -1, column: -1} )
    property var editCell: ({ row: -1, column: -1 })
    signal cellClicked(int row, int column, Item delegate)
    onCellClicked: {
//        console.log("CELL SELECT:" + column + ";" + row)
    }

    signal cellRightClicked(int row, int column, Item delegate)
    onCellRightClicked: {
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
        root.cellInputBox.parent = null
        editCell.row = -1
        editCell.column = -1
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
        width: root.style.headerCellWidth
        height: root.style.headerCellHeight

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
        width: root.style.headerCellWidth
        height: parent.height - root.style.headerCellHeight
        clip: true
        model: !tableModel || !tableModel.rowInfo ? 0 : tableModel.rowInfo

        anchors.top: tableOptions.bottom

        interactive: false
        rowHeightProvider: tableModel ? tableModel.rowInfo.rowHeight : null

        contentY: contentTableView.contentY
        delegate: Item{
            id: rowInfoDelegate
            implicitWidth: root.style.headerCellWidth

            Rectangle{
                id: rowInfoContainer
                width: parent.width
                height: parent.height - root.style.headerCellBorderSize
                color: root.style.headerCellBackgroundColor
                onHeightChanged: {
                    if ( !dragAreaY.drag.active )
                      separatorY.y = height
                }

                Text{
                    id: cellText
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.top: parent.top
                    anchors.topMargin: 3
                    text: model.value
                    color: root.style.headerCellTextStyle.color
                    font: root.style.headerCellTextStyle.font
                }

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {}
                }
            }


            Rectangle {
                id: separatorY
                z: 10

                height: root.style.headerCellBorderSize
                width: parent.width
                color: root.style.headerCellBorderColor

                Drag.active: dragAreaY.drag.active

                MouseArea {
                    id: dragAreaY
                    cursorShape: Qt.SizeVerCursor
                    anchors.fill: parent
                    drag.axis: Drag.YAxis
                    drag.target: parent
                    drag.minimumY: rowInfoContainer.y + root.style.minimumCellHeight
                }

                onYChanged: {
                    if (!Drag.active)
                        return

                    rowInfoContainer.height = y

                    var newHeight = y - rowInfoContainer.y + root.style.headerCellBorderSize
                    tableModel.rowInfo.updateRowHeight(index, newHeight)
                    rowTableView.forceLayout()
                    contentTableView.forceLayout()
                }
            }
        }

    }

    TableView {
        id: headerTableView
        width: parent.width - root.style.headerCellWidth
        height: root.style.headerCellHeight
        clip: true
        model: !tableModel || !tableModel.header ? null : tableModel.header
        anchors.left: tableOptions.right

        interactive: false
        columnWidthProvider: tableModel ? tableModel.header.columnWidth : null

        contentX: contentTableView.contentX
        delegate: Item{
            id: hedaerDelegate // width -> providerWidth
            implicitHeight: root.style.headerCellHeight

            Rectangle{
                id: headerColumnContainer
                width: parent.width - root.style.headerCellBorderSize
                onWidthChanged: {
                    if ( !dragAreaX.drag.active )
                        separatorX.x = width
                }
                color: root.style.headerCellBackgroundColor

                height: parent.height


                Text{
                    id: cellHeaderText
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.top: parent.top
                    anchors.topMargin: 3
                    text: model.name
                    color: root.style.headerCellTextStyle.color
                    font: root.style.headerCellTextStyle.font
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

                height: root.style.headerCellHeight
                width: root.style.headerCellBorderSize
                color: root.style.headerCellBorderColor

                Drag.active: dragAreaX.drag.active

                MouseArea {
                    id: dragAreaX
                    cursorShape: Qt.SizeHorCursor
                    anchors.fill: parent
                    drag.axis: Drag.XAxis
                    drag.target: parent
                    drag.minimumX: headerColumnContainer.x + root.style.minimumCellWidth
                }

                onXChanged: {
                    if (!Drag.active)
                        return

                    headerColumnContainer.width = x

                    var newWidth = x - headerColumnContainer.x + root.style.headerCellBorderSize
                    tableModel.header.updateColumnWidth(index, newWidth)
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

        height: contentTableView.contentHeight + 10 < parent.height - root.style.headerCellHeight
                ? contentTableView.contentHeight + 10
                : parent.height - root.style.headerCellHeight
        width: contentTableView.contentWidth + 10 < parent.width - root.style.headerCellWidth
               ? contentTableView.contentWidth + 10
               : parent.width - root.style.headerCellWidth

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

            contentWidth: tableModel ? tableModel.header.contentWidth : 0
            contentHeight: tableModel ? tableModel.rowInfo.contentHeight : 0

            clip: true
            model: tableModel

            columnSpacing: root.style.cellBorderSize
            columnWidthProvider: function(column){
                return tableModel.header.columnWidth(column) - 1
            }
            rowSpacing: root.style.cellBorderSize
            rowHeightProvider: function(row){
                return tableModel.rowInfo.rowHeight(row) - 1
            }

            delegate: Rectangle{
                id: tableDelegate
                implicitHeight: root.style.defaultCellHeight
                implicitWidth: root.style.defaultCellWidth
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
                    color: root.style.cellTextStyle.color
                    font: root.style.cellTextStyle.font
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
                                    root.focusCell = { column: column, row: row }
                                    tableModel.select(column, row)
                                }

                            } else {
                                tableModel.deselect()
                                tableModel.select(column, row)
                                root.focusCell = { column: column, row: row }
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
