import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import base 1.0
import live 1.0
import editor 1.0
import workspace 1.0 as Workspace

Pane{
    id: root
    paneType: 'log'
    paneState : { return {} }
    objectName: "log"

    color: currentTheme.paneBackground

    signal itemAdded()

    property bool isInWindow : false

    property alias prefixWidth: prefixSplitDragHandle.x
    onPrefixWidthChanged: {
        logList.model.width = root.width - (root.prefixVisible ? root.prefixWidth : 0) - 5
    }

    property Theme currentTheme : lk.layers.workspace ? lk.layers.workspace.themes.current : null

    property int prefixPadding : 10
    property int fontSize: 12

    property bool prefixVisible : false
    function toggleLogPrefix(){
        if ( !prefixVisible ){
            prefixVisible = true
        } else {
            prefixVisible = false
        }
    }

    property var selection: []
    function clearSelection(){
        for ( var i = 0; i < selection.length; ++i ){
            var item = logList.itemAtIndex(selection[i])
            if ( item )
                item.bottomBorder.height = 1
        }
        selection = []
    }
    function __appendToSelection(index){
        selection.push(index)
        selection.sort()
    }

    function copySelection(){
        var messages = ''
        for ( var i = 0; i < selection.length; ++i ){
            if ( messages.length )
                messages += '\n'
            messages += logList.model.messageAt(selection[i])
        }
        clipboard.setText(messages)
    }
    function openSelectionLocation(){
        if ( !selection.length )
            return
        var location = logList.model.locationAt(root.selection[root.selection.length - 1])
        if ( !location.length )
            return

        var line = parseInt(location.substr(location.lastIndexOf(':') + 1))
        var file = location.substr(0, location.lastIndexOf(':'))
        if ( file.startsWith('memory') )
            file = 'file' + file.substr(6)
        var fe = lk.layers.workspace.project.openFile(file)
        if ( fe ){
            fe.editor.forceFocus()
            fe.editor.textEdit.cursorPosition = fe.editor.document.offsetAtLine(line)

        }
    }
    function openSelectionLocationExternally(){
        if ( !selection.length )
            return
        var location = logList.model.locationAt(root.selection[root.selection.length - 1])
        if ( !location.length )
            return

        var line = parseInt(location.substr(location.lastIndexOf(':') + 1))
        var file = location.substr(0, location.lastIndexOf(':'))
        if ( file.startsWith('memory') )
            file = 'file' + file.substr(6)
        Qt.openUrlExternally(file)
    }

    function reset(){
        clearSelection()
        lk.log.clearValues()
    }

    paneClone: function(){
        return lk.layers.workspace.panes.createPane('log', paneState, [root.width, root.height])
    }

    property Clipboard clipboard: Clipboard{}

    property color topColor: currentTheme ? currentTheme.paneTopBackground : 'black'

    property Component usedDelegate : prefixVisible ? prefixDelegate : noPrefixDelegate

    property Component noPrefixDelegate : Item{
        width: logList.model.width
        height: msg.height + 5
        children: [bottomBorder, msg, locationItem]

        property Rectangle bottomBorder: Rectangle{
            anchors.bottom: parent ? parent.bottom : undefined
            width: logList.model.width
            height: 1
            color: root.currentTheme.colorScheme.middleground
        }
        property Text locationItem: Text{
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            text: {
                var fpos = location.lastIndexOf('/')
                if ( fpos === -1 )
                    fpos = location.lastIndexOf('\\')
                return location.substr(fpos + 1)
            }
            color: root.currentTheme.smallLabelStyle.color
            font: root.currentTheme.smallLabelStyle.font
        }
    }

    property Component prefixDelegate : Item{
        width: logList.model.width
        height: msg.height + 2

        Text{
            y: 5
            visible: true
            color: '#ccc'
            font.family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier'
            font.pixelSize: 12
            text: prefix
            width: root.prefixWidth
            wrapMode: Text.NoWrap
            clip: true
        }

        Item{
            children: [msg]
            x: root.prefixWidth + root.prefixPadding
        }
    }

    Timer{
        id: searchTimer
        interval: 1000
        running: false
        repeat: false
        onTriggered: {
            var logFilterActive = prefixSearchBox.text !== '' || logSearchBox.text !== '' || tagSearchBox.text
            if ( logFilterActive ){
                logFilter.source = lk.log

                var search = logSearchBox.text
                if ( search.length > 1 && search.startsWith('/') && search.endsWith('/') ){
                    search = new RegExp(search.substring(1, search.length - 1))
                }

                var prefixSearch = prefixSearchBox.text
                if ( prefixSearch.length > 1 && prefixSearch.startsWith('/') && prefixSearch.endsWith('/') ){
                    prefixSearch = new RegExp(prefixSearch.substring(1, prefixSearch.length - 1))
                }

                logFilter.search = search
                logFilter.prefix = prefixSearch
                logFilter.tag    = tagSearchBox.text
                logList.model = logFilter
            } else {
                logList.model = lk.log
                logFilter.source = null
            }
        }
    }

    Rectangle{
        id: logViewHeader
        width: parent.width
        height: 30
        color : root.topColor

        PaneDragItem{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            onDragStarted: lk.layers.workspace.panes.__dragStarted(root)
            onDragFinished: lk.layers.workspace.panes.__dragFinished(root)
            display: "log"
        }

        InputBox{
            id: prefixSearchBox
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 3
            visible: root.prefixVisible
            anchors.fill: undefined
            width: prefixSplitDragHandle.x - root.prefixPadding - 5
            border.width: 0
            text: ''
            color: root.color
            onTextChanged: searchTimer.restart()
            height: 24
        }

        Rectangle{
            id: prefixSplitDragHandle
            color: '#1f2227'
            width: 2
            visible: root.prefixVisible
            x: 348
            anchors.top: parent.top
            anchors.topMargin: 3
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 3

            MouseArea{
                id: prefixSplit
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: 20
                drag.maximumX: root.width - 125
                cursorShape: Qt.SplitHCursor
            }
        }

        InputBox{
            id: logSearchBox
            anchors.left: parent.left
            anchors.leftMargin: prefixSearchBox.visible ? root.prefixWidth + 5 : 20
            anchors.right: tagSearchBox.visible ? tagSearchBox.left : parent.right
            anchors.rightMargin: tagSearchBox.visible ? 10 : 120
            anchors.top: parent.top
            anchors.topMargin: 3
            anchors.fill: undefined
            text: ''
            onTextChanged : searchTimer.restart()
            color: root.color
            height: 24
            border.width: 0
        }

        InputBox{
            id: tagSearchBox
            anchors.right: parent.right
            anchors.rightMargin: 120
            anchors.top: parent.top
            anchors.topMargin: 3
            anchors.fill: undefined
            width: 100
            visible: false
            text: ''
            onTextChanged : searchTimer.restart()
            border.width: 0
            color: root.color
            height: 24
        }

        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 90
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            height : clearLog.containsMouse ? parent.height : parent.height - 3
            width : 25
            color : "transparent"
            Image{
                id : clearLogImage
                anchors.centerIn: parent
                source : "qrc:/images/log-clear.png"
            }
            MouseArea{
                id : clearLog
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.reset()
            }
            Behavior on height{ NumberAnimation{  duration: 100 } }
        }

        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 60
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            height : prefixMouseArea.containsMouse ? parent.height : parent.height - 3
            width : 25
            color : "transparent"
            Image{
                id : prefixImage
                anchors.centerIn: parent
                source : "qrc:/images/log-view-prefix.png"
            }
            MouseArea{
                id : prefixMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.toggleLogPrefix()
            }
            Behavior on height{ NumberAnimation{  duration: 100 } }
        }

        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            height : tagSearchMouseArea.containsMouse ? parent.height : parent.height - 3
            width : 25
            color : "transparent"
            Image{
                id : tagSearchImage
                anchors.centerIn: parent
                source : "qrc:/images/log-search-tag.png"
            }
            MouseArea{
                id : tagSearchMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: tagSearchBox.visible = !tagSearchBox.visible
            }
            Behavior on height{ NumberAnimation{  duration: 100 } }
        }

        Item{
            anchors.right: parent.right
            width: 30
            height: parent.height

            Image{
                id : paneOptions
                anchors.centerIn: parent
                source : "qrc:/images/pane-menu.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    logMenu.visible = !logMenu.visible
                }
            }
        }

        VisualLogFilter{
            id: logFilter
        }
    }

    Workspace.PaneMenu{
        id: logMenu
        anchors.right: root.right
        anchors.topMargin: 30
        anchors.top: root.top

        style: root.currentTheme.popupMenuStyle

        width: 180

        Behavior on opacity{ NumberAnimation{ duration: 200 } }

        Workspace.PaneMenuItem{
            id: splitHorizontally
            text: qsTr("Split Horizontally")
            onClicked : {
                logMenu.visible = false
                var clone = root.paneClone()
                var index = root.parentSplitViewIndex()
                lk.layers.workspace.panes.splitPaneHorizontallyWith(root.parentSplitView, index, clone)
            }
        }

        Workspace.PaneMenuItem{
            id: splitVertically
            text: qsTr("Split Vertically")
            onClicked : {
                logMenu.visible = false
                var clone = root.paneClone()
                var index = root.parentSplitViewIndex()
                lk.layers.workspace.panes.splitPaneVerticallyWith(root.parentSplitView, index, clone)
            }
        }

        Workspace.PaneMenuItem{
            id: moveToNewWindow
            text: qsTr("Move to New Window")
            onClicked : {
                logMenu.visible = false
                lk.layers.workspace.panes.movePaneToNewWindow(root)
            }
        }

        Workspace.PaneMenuItem{
            id: removeLog
            text: qsTr("Remove Pane")
            onClicked : {
                logMenu.visible = false
                lk.layers.workspace.panes.removePane(root)
            }
        }
    }

    ScrollView{
        id: logScroll
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: logViewHeader.bottom
        anchors.bottom: parent.bottom

        anchors.margins: 5
        clip: true

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
            frame: Rectangle{color: root.color}
            corner: Rectangle{color: root.color}
        }

        ListView{
            id: logList
            model: lk.log
            anchors.fill: parent
            delegate: root.usedDelegate
            objectName: "logList"

            onWidthChanged: lk.log.width = width - (root.prefixVisible ? root.prefixWidth : 0) - root.prefixPadding

            Connections{
                target: lk.log
                function onRowsInserted(){
                    if ( root.visible && logScroll.flickableItem.contentHeight > logScroll.height )
                        logScroll.flickableItem.contentY = logScroll.flickableItem.contentHeight - logScroll.height
                    root.itemAdded()
                }
            }
            Component.onCompleted: {
                lk.log.style = {
                    infoColor: root.currentTheme.monoTextStyle.color,
                    warnColor: root.currentTheme.colorScheme.warningForeground,
                    errorColor: root.currentTheme.colorScheme.errorForeground,
                    font: root.currentTheme.monoTextStyle.font.family,
                    fontSize: root.currentTheme.monoTextStyle.font.pixelSize
                }
            }
        }

    }

    MouseArea{
        id: logArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 50
        anchors.top: logViewHeader.bottom
        anchors.bottom: parent.bottom
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if ( mouse.button === Qt.RightButton ){
                var index = logList.indexAt(mouseX, mouseY + logScroll.flickableItem.contentY)
                if ( index > -1 ){
                    if ( root.selection.length < 2 ){
                        root.clearSelection()
                        var item = logList.itemAtIndex(index)
                        var bb = item.bottomBorder
                        bb.height = item.height
                        root.__appendToSelection(index)
                    }
                    if ( lk.layers.workspace ){
                        lk.layers.workspace.panes.openContextMenu(logList, root)
                    }
                }
            } else if( mouse.button === Qt.LeftButton ){
                var index = logList.indexAt(mouseX, mouseY + logScroll.flickableItem.contentY)
                if ( index > -1 ){
                    if ( (mouse.modifiers & Qt.ShiftModifier) && root.selection.length > 0 ){
                        var prevSelection = root.selection[root.selection.length - 1]
                        var from = prevSelection > index ? index : prevSelection
                        var to   = prevSelection > index ? prevSelection : index
                        for ( var i = from; i <= to; ++i ){
                            var item = logList.itemAtIndex(i)
                            var bb = item.bottomBorder
                            bb.height = item.height
                            root.__appendToSelection(i)
                        }
                    } else {
                        if ( !(mouse.modifiers & Qt.ControlModifier) ){
                            root.clearSelection()
                        }
                        var item = logList.itemAtIndex(index)
                        var bb = item.bottomBorder
                        bb.height = item.height
                        root.__appendToSelection(index)
                    }
                }
            }
        }
        onDoubleClicked: {
            if ( mouse.modifiers & Qt.AltModifier )
                root.openSelectionLocationExternally()
            else
                root.openSelectionLocation()
        }
    }

}
