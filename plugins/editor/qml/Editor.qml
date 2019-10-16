/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import editor.private 1.0
import base 1.0

Pane{
    id : editor

    property bool isDirtyMask: true
    property bool isDirty: document ? document.isDirty : false

    property alias text: editorArea.text
    property alias font: editorArea.font
    property alias internalActiveFocus : editorArea.activeFocus
    property alias internalFocus: editorArea.focus
    onInternalActiveFocusChanged: {
        if ( panes.activePane !== editor ){
            panes.activateItem(textEdit, editor)
        }
    }

    property alias documentHandler: editorArea.documentHandler
    property alias textEdit: editorArea

    property var panes: null
    property var document: null

    paneType: 'editor'
    paneState : { return {} }
    paneInitialize : function(s){
        if ( s.document ){
            if (typeof s.document === 'string' || s.document instanceof String){
                var d = project.documentModel.documentByPathHash(s.document)
                document = d
            } else {
                document = s.document
            }
        }
    }

    paneHelp : function(){
        if ( documentHandler && documentHandler.has(DocumentHandler.LanguageHelp) ){
            var helpPath = ''
            if ( codeHandler.completionModel.isEnabled ){
                helpPath = qmlSuggestionBox.getDocumentation()
            } else {
                helpPath = documentHandler.codeHandler.help(editorArea.cursorPosition)
            }
            if ( helpPath.length > 0 ){
//                var docItem = lk.layers.workspace.documentation.load(helpPath)
                var docItem = null // documentation is currently disabled
                if ( docItem ){
                    var docPane = mainSplit.findPaneByType('documentation')
                    if ( !docPane ){
                        var storeWidth = editor.width
                        docPane = editor.panes.createPane('documentation', {}, [editor.width, editor.height])

                        console.log(docPane)

                        var index = editor.parentSplitterIndex()
                        editor.panes.splitPaneHorizontallyWith(editor.parentSplitter, index, docPane)

                        editor.width = storeWidth
                        docPane.width = storeWidth
                    }
                    docPane.pageTitle = helpPath
                    docPane.page = docItem
                }
            }

        }

        return null
    }

    paneFocusItem : editorArea
    paneClone: function(){
        return editor.panes.createPane('editor', paneState, [editor.width, editor.height])
    }

    property int fragmentStart: 0
    property int fragmentEnd: -1

    onDocumentChanged: {
        paneState = { document : document }
        codeHandler.setDocument(document)
    }

    property Theme currentTheme : lk.layers.workspace.themes.current

    property color topColor: currentTheme ? currentTheme.paneTopBackground : 'black'
    property color lineSurfaceColor: topColor
    property color lineInfoColor:  currentTheme ? currentTheme.paneTopBackgroundAlternate : 'black'
    property color optionsColor: currentTheme ? currentTheme.paneTopBackground : 'black'

    color : lk.layers.workspace.themes.current.paneBackground
    clip : true

    objectName: "editor"

    function hasActiveEditor(){
        return root.panes.activePane.objectName === 'editor'
    }

    function save(){
        if ( !editor.document )
            return;
        if ( editor.document.file.exists() ){
            editor.document.save()
        } else {
            saveAs()
        }
    }

    function saveAs(){
        lk.layers.window.dialogs.saveFile(
            { filters: [ "Qml files (*.qml)", "All files (*)" ] },
            function(url){
                var editordoc = editor.document
                if ( !editordoc.saveAs(url) ){
                    lk.layers.window.dialogs.message(
                        'Failed to save file to: ' + url,
                        {
                            button3Name : 'Ok',
                            button3Function : function(mbox){
                                mbox.close()
                            }
                        }
                    )
                    return
                }

                if ( !project.isDirProject() ){
                    project.openProject(url)
                } else if ( project.isFileInProject(url) ){
                    lk.layers.workspace.project.openFile(url, ProjectDocument.Edit)
                } else {
                    var fileUrl = url
                    lk.layers.window.dialogs.message(
                        'File is outside project scope. Would you like to open it as a new project?',
                    {
                        button1Name : 'Open as project',
                        button1Function : function(mbox){
                            var projectUrl = fileUrl
                            projectView.closeProject(
                                function(){
                                    project.openProject(projectUrl);
                                }
                            )
                            mbox.close()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(mbox){
                            mbox.close()
                        },
                        returnPressed : function(mbox){
                            var projectUrl = fileUrl
                            projectView.closeProject(
                                function(){
                                    project.openProject(projectUrl)
                                }
                            )
                            mbox.close()
                        }
                    })
                }
            }
        )
    }

    function closeDocument(){
        if ( !editor.document )
            return;
        if ( editor.document.isDirty ){
            var saveFunction = function(mbox){
                if ( editor.document.file.exists() ){
                    editor.document.save()
                    editor.closeDocumentAction()
                } else {
                    lk.layers.window.dialogs.saveFile(
                        { filters: [ "Qml files (*.qml)", "All files (*)" ] },
                        function(url){
                            if ( !editor.document.saveAs(url) ){
                                lk.layers.window.dialogs.message(
                                    'Failed to save file to: ' + url,
                                    {
                                        button3Name : 'Ok',
                                        button3Function : function(){ lk.layers.window.dialogs.messageClose(); }
                                    }
                                )
                                return;
                            }
                            editor.closeDocumentAction()
                        }
                    )
                }
                mbox.close()
            }

            lk.layers.window.dialogs.message('File contains unsaved changes. Would you like to save them before closing?',
            {
                button1Name : 'Yes',
                button1Function : function(mbox){
                    saveFunction(mbox)
                },
                button2Name : 'No',
                button2Function : function(mbox){
                    mbox.close()
                    editor.closeDocumentAction()
                    editor.document = project.documentModel.lastOpened()
                },
                button3Name : 'Cancel',
                button3Function : function(mbox){
                    mbox.close()
                },
                returnPressed : function(mbox){
                    saveFunction(mbox)
                },
            })
        } else
            editor.closeDocumentAction()
    }

    function getCursorFragment(){
        return codeHandler.contextBlockRange(editorArea.cursorPosition)
    }

    function closeDocumentAction(){
        if ( !project.isDirProject() && document.file.path === project.active.path ){
            lk.layers.window.dialogs.message(
                'Closing this file will also close this project. Would you like to close the project?',
            {
                button1Name : 'Yes',
                button1Function : function(mbox){
                    project.closeProject()
                    mbox.close()
                },
                button3Name : 'No',
                button3Function : function(mbox){
                    mbox.close()
                },
                returnPressed : function(mbox){
                    project.closeProject()
                    mbox.close()
                }
            })
        } else {
            project.closeFile(document.file.path)
            editor.document = project.documentModel.lastOpened()
        }
    }

    function getCursorRectangle(){
        return editorArea.cursorRectangle
    }

    function assistCompletion(){
        codeHandler.generateCompletion(editorArea.cursorPosition)
    }

    function toggleSize(){
        if ( editor.width < parent.width / 2)
            editor.width = parent.width - parent.width / 4
        else if ( editor.width === parent.width / 2 )
            editor.width = parent.width / 4
        else
            editor.width = parent.width / 2
    }

    function forceFocus(){
        editorArea.forceActiveFocus()
    }

    function cursorWindowCoords(){
        return Qt.point(editor.x, editor.y - flick.flickableItem.contentY);
    }

    Rectangle{
        anchors.left: parent.left
        anchors.top: parent.top

        width: parent.width
        height: 30

        color : editor.topColor
        gradient: Gradient{
            GradientStop { position: 0.0;  color: "#030d16" }
            GradientStop { position: 0.10; color: editor.topColor }
        }

        PaneDragItem{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            onDragStarted: root.panes.__dragStarted(editor)
            onDragFinished: root.panes.__dragFinished(editor)
            display: titleText.text
        }

        Text{
            id: titleText
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#808691"
            text: {
                if ( editor.document ){
                    var filename = editor.document.file.name
                    if ( !editor.document.file.exists() ){
                        var findex = filename.substring(2)
                        filename = 'untitled' + (findex === '0' ? '' : findex)
                    }
                    if ( editor.document.isDirty )
                        filename += '*'
                    return filename;
                } else {
                    return ''
                }
            }
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            font.weight: Font.Normal
        }

        Rectangle{
            color: 'transparent'
            width: 24
            height: parent.height
            anchors.right: parent.right
            anchors.rightMargin: 109
            visible : editor.document !== null

            Image{
                id : switchImage
                anchors.centerIn: parent
                source : "qrc:/images/switch-file.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    lk.layers.workspace.commands.execute('window.workspace.toggleNavigation')
                }
            }
        }

        Rectangle{
            color: 'transparent'
            width: 30
            height: parent.height
            anchors.right: parent.right
            anchors.rightMargin: 85
            visible : editor.document !== null
            Text{
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 16
                font.weight: Font.Light
                text: 'x'
                color: "#808691"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 3
            }
            MouseArea{
                anchors.fill: parent
                onClicked: editor.closeDocument()
            }
        }

        Rectangle{
            width: 50
            height: parent.height
            anchors.right: parent.right
            anchors.rightMargin: 31
            visible : editor.document !== null
            color: editor.lineInfoColor

            property bool lineAndColumn : true

            Text{
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 11
                text: parent.lineAndColumn
                         ? editorArea.lineNumber + ", " + editorArea.columnNumber
                         : editorArea.cursorPosition
                color: "#808691"
                anchors.left: parent.left
                anchors.leftMargin: 7
                anchors.verticalCenter: parent.verticalCenter
            }
            MouseArea{
                anchors.fill: parent
                onClicked: parent.lineAndColumn = !parent.lineAndColumn
            }
        }

        Rectangle{
            anchors.right: parent.right
            width: 30
            height: parent.height
            color: editor.optionsColor

            Image{
                id : paneOptions
                anchors.centerIn: parent
                source : "qrc:/images/pane-menu.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    editorAddRemoveMenu.visible = !editorAddRemoveMenu.visible
                }
            }
        }
    }

    Rectangle {
        id: editorAddRemoveMenu
        visible: false
        anchors.right: editor.right
        anchors.topMargin: 30
        anchors.top: editor.top
        property int buttonHeight: 30
        property int buttonWidth: 180
        opacity: visible ? 1.0 : 0
        z: 1000

        Behavior on opacity{ NumberAnimation{ duration: 200 } }


        Rectangle{
            id: addEditorButton
            anchors.top: parent.top
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: addEditorText
                text: qsTr("Split horizontally")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: addEditorArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : addEditorArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    editorAddRemoveMenu.visible = false
                    var clone = editor.paneClone()
                    var index = editor.parentSplitterIndex()
                    editor.panes.splitPaneHorizontallyWith(editor.parentSplitter, index, clone)
                }
            }
        }

        Rectangle{
            id: addVerticalEditorButton
            anchors.top: addEditorButton.bottom
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: addVerticalEditorText
                text: qsTr("Split vertically")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: addVerticalEditorArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : addVerticalEditorArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    editorAddRemoveMenu.visible = false
                    var clone = editor.paneClone()
                    var index = editor.parentSplitterIndex()
                    editor.panes.splitPaneVerticallyWith(editor.parentSplitter, index, clone)
                }
            }
        }

        Rectangle{
            id: newWindowEditorButton
            anchors.top: addVerticalEditorButton.bottom
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: newWindowEditorText
                text: qsTr("Move to new window")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: newWindowEditorArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : newWindowEditorArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    editorAddRemoveMenu.visible = false
                    editor.panes.movePaneToNewWindow(editor)
                }
            }
        }

        Rectangle{
            id: removeEditorButton
            anchors.top: newWindowEditorButton.bottom
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: removeEditorText
                text: qsTr("Remove Pane")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: removeEditorArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : removeEditorArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    editorAddRemoveMenu.visible = false
                    editor.panes.removePane(editor)
                }
            }
        }
    }

    FontMetrics{
        id: editorMetrics
        font: editorArea.font
    }

    Rectangle{
        anchors.fill: parent
        anchors.topMargin: 30
        color: editor.color

        Rectangle{
            id: lineSurfaceBackground
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: lineSurface.width + 5
            color: editor.lineSurfaceColor
            clip: true

            Flickable{
                anchors.fill: parent
                anchors.bottomMargin: 10
                anchors.leftMargin: 5
                contentY: flick.flickableItem.contentY
                interactive: false

                LineSurface{
                    id: lineSurface
                    color: "#3e464d"
                    Component.onCompleted: {
                        setComponents(editorArea);
                    }
                }
            }
        }

        ScrollView {
            id: flick

            anchors.fill: parent
            anchors.topMargin: 5
            anchors.leftMargin: 7 + lineSurfaceBackground.width
            anchors.rightMargin: 3

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
                        color: editor.color
                    }
                }
                decrementControl: null
                incrementControl: null
                frame: Item{}
                corner: Rectangle{color: editor.color}
            }

            frameVisible: false

            function ensureVisible(r){
                if (!editor.internalActiveFocus )
                    return;
                if (flickableItem.contentX >= r.x)
                    flickableItem.contentX = r.x;
                else if (flickableItem.contentX + width <= r.x + r.width + 20)
                    flickableItem.contentX = r.x + r.width - width + 20;
                if (flickableItem.contentY >= r.y)
                    flickableItem.contentY = r.y;
                else if (flickableItem.contentY + height <= r.y + r.height + 20)
                    flickableItem.contentY = r.y + r.height - height + 20;
            }

            NewTextEdit {
                id : editorArea

                anchors.left: parent.left
                anchors.leftMargin: 5

                fragmentStart: editor.fragmentStart
                fragmentEnd: editor.fragmentEnd

                documentHandler: DocumentHandler {
                    id: codeHandler
                    // target: editorArea.textDocument
                    editorFocus: editorArea.activeFocus
                    onCursorPositionRequest : {
                        editorArea.forceActiveFocus()
                        editorArea.cursorPosition = position
                    }
                }


                property int lastLength: 0

                onCursorRectangleChanged: {
                    flick.ensureVisible(cursorRectangle)
                }
                onCursorPositionChanged: {
                    /// disable the model if no text has changed, let the code handler decide otherwise
                    if ( length === lastLength )
                        codeHandler.completionModel.disable()
                    lastLength = length
                }

                focus : true

                color : "#fff"
                font.family: "Source Code Pro, Ubuntu Mono, Courier New, Courier"
                font.pixelSize: lk.settings.file('editor').fontSize
                font.weight: Font.Normal

                selectByMouse: true
                mouseSelectionMode: NewTextEdit.SelectCharacters
                selectionColor: "#3d4856"

                textFormat: NewTextEdit.PlainText

                wrapMode: NewTextEdit.NoWrap
                height : Math.max( flick.height - 10, paintedHeight + 15)
                width : Math.max( flick.width - 10, paintedWidth + 15)

                readOnly: editor.document === null || editor.document.isMonitored

                Keys.onPressed: {
                    if ( (event.key === Qt.Key_BracketRight && (event.modifiers === Qt.ShiftModifier) ) ||
                         (event.key === Qt.Key_BraceRight) ){

                        //TODO: Move to codehandler
                        if ( cursorPosition > 4 ){
                            var clastpos = cursorPosition
                            if( editorArea.text.substring(cursorPosition - 4, cursorPosition) === "    " ){
                                editorArea.text = editorArea.text.slice(0, clastpos - 4) + "}" + editorArea.text.slice(clastpos)
                                cursorPosition = clastpos - 3
                            } else {
                                editorArea.text = editorArea.text.slice(0, clastpos) + "}" + editorArea.text.slice(clastpos)
                                cursorPosition = clastpos + 1
                            }
                        }
                        event.accepted = true

                    } else if ( event.key === Qt.Key_PageUp ){
                        if ( codeHandler.completionModel.isEnabled ){
                            qmlSuggestionBox.highlightPrevPage()
                        } else {
                            var lines = flick.height / cursorRectangle.height
                            var prevLineStartPos = editorArea.text.lastIndexOf('\n', cursorPosition - 1)
                            while ( --lines > 0 ){
                                cursorPosition   = prevLineStartPos + 1
                                prevLineStartPos = editorArea.text.lastIndexOf('\n', cursorPosition - 2)
                                if ( prevLineStartPos === -1 ){
                                    cursorPosition = 0;
                                    break;
                                }
                            }
                        }
                        event.accepted = true
                    } else if ( event.key === Qt.Key_Tab ){
                        if ( event.modifiers & Qt.ShiftModifier ){
                            codeHandler.manageIndent(
                                editorArea.selectionStart, editorArea.selectionEnd - editorArea.selectionStart, true
                            )
                            event.accepted = true
                        } else if ( selectionStart !== selectionEnd ){
                            codeHandler.manageIndent(
                                editorArea.selectionStart, editorArea.selectionEnd - editorArea.selectionStart, false
                            )
                            event.accepted = true
                        } else {
                            var clastpost = cursorPosition
                            editorArea.text = editorArea.text.slice(0, clastpost) + "    " + editorArea.text.slice(clastpost)
                            editorArea.cursorPosition = clastpost + 4
                            event.accepted = true
                        }
                    } else if ( event.key === Qt.Key_Backtab ){
                        codeHandler.manageIndent(
                            editorArea.selectionStart, editorArea.selectionEnd - editorArea.selectionStart, true
                        )
                        event.accepted = true
                    } else if ( event.key === Qt.Key_PageDown ){
                        if ( codeHandler.completionModel.isEnabled ){
                            qmlSuggestionBox.highlightNextPage()
                        } else {
                            var lines = flick.height / cursorRectangle.height
                            var nextLineStartPos = editorArea.text.indexOf('\n', cursorPosition)
                            while ( lines-- > 0 && nextLineStartPos !== -1 ){
                                cursorPosition   = nextLineStartPos + 1
                                nextLineStartPos = editorArea.text.indexOf('\n', cursorPosition)
                            }
                        }
                        event.accepted = true
                    } else if ( event.key === Qt.Key_Down ){
                        if ( codeHandler.completionModel.isEnabled ){
                            event.accepted = true
                            qmlSuggestionBox.highlightNext()
                        }
                    } else if ( event.key === Qt.Key_Up ){
                        if ( codeHandler.completionModel.isEnabled ){
                            event.accepted = true
                            qmlSuggestionBox.highlightPrev()
                        }
                    } else if ( event.key === Qt.Key_Escape ){
                        if ( codeHandler.completionModel.isEnabled ){
                            codeHandler.completionModel.disable()
                        }
                    } else {
                        var command = lk.layers.workspace.keymap.locateCommand(event.key, event.modifiers)
                        if ( command !== '' ){
                            lk.layers.workspace.commands.execute(command)
                            event.accepted = true
                        }
                    }
                }

                Keys.onReturnPressed: {
                    event.accepted = false
                    if ( codeHandler.completionModel.isEnabled ){
                        codeHandler.insertCompletion(
                            codeHandler.completionModel.completionPosition,
                            cursorPosition,
                            qmlSuggestionBox.getCompletion()
                        )
                        event.accepted = true
                    }
                }

                Behavior on font.pixelSize {
                    NumberAnimation { duration: 40 }
                }

                Connections{
                    target: project.documentModel
                    onAboutToClose: {
                        if ( document === editor.document ){
                            editor.document = null
                        }
                    }
                }

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        editorArea.clearSelectionOnFocus(false)

                        for ( var i = 0; i < contextMenu.additionalItems.length; ++i ){
                            contextMenu.removeItem(contextMenu.additionalItems[i])
                        }
                        contextMenu.additionalItems = []

                        var res = lk.layers.workspace.interceptMenu(editor)
                        for ( var i = 0; i < res.length; ++i ){
                            var menuitem = contextMenu.insertItem(i, res[i].name)
                            menuitem.enabled = res[i].enabled
                            menuitem.triggered.connect(res[i].action)
                            contextMenu.additionalItems.push(menuitem)
                        }

                        contextMenu.popup()
                    }
                }

                Menu{
                    id: contextMenu
                    style: ContextMenuStyle{}
                    property var additionalItems : []
                    onAboutToHide: {
                        editorArea.clearSelectionOnFocus(true)
                    }

                    MenuItem {
                        text: qsTr("Cut")
                        shortcut: StandardKey.Cut
                        enabled: editorArea.selectionStart !== editorArea.selectionEnd
                        onTriggered: editorArea.cut()
                    }
                    MenuItem {
                        text: qsTr("Copy")
                        shortcut: StandardKey.Copy
                        enabled: editorArea.selectionStart !== editorArea.selectionEnd
                        onTriggered: editorArea.copy()
                    }
                    MenuItem {
                        text: qsTr("Paste")
                        shortcut: StandardKey.Paste
                        enabled: editorArea.canPaste
                        onTriggered: editorArea.paste()
                    }
                }
            }
        }

        SuggestionBox{
            id: qmlSuggestionBox

            fontFamily: editorArea.font.family
            fontSize: editorArea.font.pixelSize
            smallFontSize: editorArea.font.pixelSize - 2
            visible: codeHandler.completionModel.isEnabled && suggestionCount > 0
            opacity: visible ? 0.95 : 0

            y: {
                var calculatedY =
                    editorArea.cursorRectangle.y +
                    editorArea.cursorRectangle.height + 2 -
                    flick.flickableItem.contentY

                if ( calculatedY > flick.height - height )
                    calculatedY = editorArea.cursorRectangle.y - height - flick.flickableItem.contentY
                return calculatedY;
            }
            x: {
                var calculatedX =
                    editorArea.positionToRectangle(codeHandler.completionModel.completionPosition).x -
                    flick.flickableItem.contentX

                if ( calculatedX > flick.width - width)
                    calculatedX = flick.width - width
                return calculatedX;
            }
            model: codeHandler.completionModel

            Behavior on opacity {
                NumberAnimation { duration: 150 }
            }
        }
    }

}

