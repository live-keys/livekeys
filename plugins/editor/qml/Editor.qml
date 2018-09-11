/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

Rectangle{
    id : editor

    signal editFragment(int position)
    signal adjustFragment(int position)
    signal bindProperties(int position, int length)

    property bool isDirtyMask: true
    property bool isDirty: document ? document.isDirty : false

    property alias text: editorArea.text
    property alias font: editorArea.font
    property alias internalActiveFocus : editorArea.activeFocus
    property alias internalFocus: editorArea.focus

    property var loadedPalettes: []
    property bool isEditingSection: false
    property var windowControls: null
    property var document: null

    onDocumentChanged: {
        if ( !document ){
            editor.text = ''
        }
        codeHandler.setDocument(document)
    }

    property color topColor: "#09121c"

    color : "#050b12"
    clip : true

    function save(){
        if ( !editor.document )
            return;
        if ( editor.document.file.name !== '' ){
            editor.document.save()
            if ( project.active && project.active !== editor.document ){
                engine.createObjectAsync(
                    project.active.content,
                    windowControls.runSpace,
                    project.active.file.pathUrl(),
                    project.active,
                    true
                );
            }
        } else {
            windowControls.saveFileDialog.open()
        }
    }

    function saveAs(){
        windowControls.saveFileDialog.open()
    }

    function closeDocument(){
        if ( !editor.document )
            return;
        if ( editor.document.isDirty ){
            var saveFunction = function(){
                if ( editor.document.file.name !== '' ){
                    editor.document.save()
                    editor.closeDocumentAction()
                } else {
                    windowControls.saveFileDialog.open()
                    windowControls.saveFileDialog.callback = function(){
                        if ( !editor.document.saveAs(windowControls.saveFileDialog.fileUrl) ){
                            windowControls.messageDialog.show(
                                'Failed to save file to: ' + windowControls.saveFileDialog.fileUrl,
                                {
                                    button3Name : 'Ok',
                                    button3Function : function(){ windowControls.messageDialog.close(); }
                                }
                            )
                            return;
                        }
                        editor.closeDocumentAction()
                    }
                }
                windowControls.messageDialog.close()
            }

            windowControls.messageDialog.show('File contains unsaved changes. Would you like to save them before closing?',
            {
                button1Name : 'Yes',
                button1Function : saveFunction,
                button2Name : 'No',
                button2Function : function(){
                    editor.closeDocumentAction()
                    editor.document = project.documentModel.lastOpened()
                    windowControls.messageDialog.close()
                },
                button3Name : 'Cancel',
                button3Function : function(){
                    windowControls.messageDialog.close()
                },
                returnPressed : saveFunction
            })
        } else
            editor.closeDocumentAction()
    }

    function getCursorFragment(){
        return codeHandler.contextBlockRange(editorArea.cursorPosition)
    }

    function closeDocumentAction(){
        project.closeFile(document.file.path)
        editor.document = project.documentModel.lastOpened()
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

        Text{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 15
            color: "#808691"
            text: {
                if ( editor.document ){
                    var filename = editor.document.file.name
                    if ( filename === '' )
                        filename = 'untitled'
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
            gradient: Gradient{
                GradientStop { position: 0.0;  color: "#050b12" }
                GradientStop { position: 0.30; color: "#050b12" }
            }
            Text{
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 11
                text:
                    (Math.floor(editorArea.cursorRectangle.y / editorMetrics.height) + 1) + ', ' +
                    (Math.floor(editorArea.cursorRectangle.x / editorMetrics.averageCharacterWidth) + 1)
                color: "#808691"
                anchors.left: parent.left
                anchors.leftMargin: 7
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle{
            anchors.right: parent.right
            width: 30
            height: parent.height
            gradient: Gradient{
                GradientStop { position: 0.0;  color: "#04131f" }
                GradientStop { position: 0.30; color: "#081019" }
            }

            Image{
                id : toggleNavigationImage
                anchors.centerIn: parent
                source : "qrc:/images/toggle-navigation.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: livecv.commands.execute('window.toggleNavigation')
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
            width: lineSurface.width
            color: editor.topColor
            clip: true

            Flickable{
                anchors.fill: parent
                anchors.bottomMargin: 10
                contentY: flick.flickableItem.contentY

                LineSurface{
                    id: lineSurface
                    color: "#808691"
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
                        color: "#061724"
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

                documentHandler: DocumentHandler {
                    id: codeHandler
                    // target: editorArea.textDocument
                    editorFocus: editorArea.activeFocus
                    onCursorPositionRequest : {
                        editorArea.forceActiveFocus()
                        editorArea.cursorPosition = position
                    }
                    onContentsChangedManually: {
                        if ( project.active === editor.document )
                        {
                            editor.windowControls.createTimer.restart();
                        }
                    }
                    onPaletteAboutToRemove: {
                        for ( var i = 0; i < loadedPalettes.length; ++i ){
                            if ( palette.item === loadedPalettes[i].paletteItem ){
                                var lp = loadedPalettes[i]
                                loadedPalettes.splice(i, 1)
                                lp.destroy()
                                return
                            }
                        }
                    }
                    onEditingStateChanged: {
                        editor.isEditingSection = isEditing
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

                objectName: "editor"

                property string objectCommandIndex : livecv.commands.add(editorArea, {
                    'saveFile' : editor.save,
                    'saveFileAs' : editor.saveAs,
                    'closeFile' : editor.closeDocument,
                    'assistCompletion' : editor.assistCompletion,
                    'toggleSize' : editor.toggleSize
                })

                color : "#fff"
                font.family: "Source Code Pro, Ubuntu Mono, Courier New, Courier"
                font.pixelSize: livecv.settings.file('editor').fontSize
                font.weight: Font.Normal

                selectByMouse: true
                mouseSelectionMode: NewTextEdit.SelectCharacters
                selectionColor: "#3d4856"

                textFormat: NewTextEdit.PlainText

                wrapMode: NewTextEdit.NoWrap
                height : Math.max( flick.height - 20, paintedHeight )
                width : Math.max( flick.width - 20, paintedWidth )

                readOnly: editor.document === null || editor.document.isMonitored

                Keys.onPressed: {
                    if ( (event.key === Qt.Key_BracketRight && (event.modifiers === Qt.ShiftModifier) ) ||
                         (event.key === Qt.Key_BraceRight) ){

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

                    } else if ( event.key === Qt.Key_Return && (event.modifiers & Qt.ControlModifier) ){
                        if ( editor.isEditingSection )
                            codeHandler.commitEdit()
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
                        } else if ( editor.loadedPalettes.length > 0 ){
                            var last = editor.loadedPalettes[editor.loadedPalettes.length - 1]
                            codeHandler.removePalette(last.paletteItem)
                        }

                        codeHandler.cancelEdit()
                    } else {
                        var command = livecv.keymap.locateCommand(event.key, event.modifiers)
                        if ( command !== '' ){
                            livecv.commands.execute(command)
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
                        contextMenu.popup()
                    }
                }

                Menu {
                    id: contextMenu
                    style: ContextMenuStyle{}
                    onAboutToHide: editorArea.clearSelectionOnFocus(true)
                    onAboutToShow: {
                        var cursorInfo = codeHandler.cursorInfo(
                            editorArea.selectionStart, editorArea.selectionEnd - editorArea.selectionStart
                        );
                        bindMenuItem.enabled = cursorInfo.canBind
                        unbindMenuItem.enabled = cursorInfo.canUnbind
                        editMenuItem.enabled = cursorInfo.canEdit
                        adjustMenuItem.enabled = cursorInfo.canAdjust
                    }

                    MenuItem {
                        id: editMenuItem
                        text: qsTr("Edit")
                        enabled: false
                        onTriggered: codeHandler.edit(editorArea.cursorPosition, editor.windowControls.runSpace.item)
                    }
                    MenuItem {
                        id: adjustMenuItem
                        text: qsTr("Adjust")
                        enabled: false
                        onTriggered: {
                            var palettes = codeHandler.findPalettes(editorArea.cursorPosition, editor.windowControls.runSpace.item)
                            var rect = editor.getCursorRectangle()
                            var position = Qt.point(editor.x, editor.y - flick.flickableItem.contentY)
                            if ( palettes.size() === 1 ){
                                var palette = palettes.loadAt(0)
                                codeHandler.openPalette(palette, editorArea.cursorPosition, editor.windowControls.runSpace.item)

                                var paletteBox = windowControls.editSpace.createPalette()

                                palette.item.x = 5
                                palette.item.y = 7

                                paletteBox.setChild(palette.item, rect, position, editSpace.placement.top)

                                paletteBox.color = "#02070b"
                                paletteBox.radius = 5
                                paletteBox.border.width = 1
                                paletteBox.border.color = "#061b24"
                                editor.loadedPalettes.push(paletteBox)
                            } else {
                                //Create editor box
                                var palList      = windowControls.editSpace.createPaletteList()
                                var palListBox   = windowControls.editSpace.createEditorBox(palList, rect, position, editSpace.placement.bottom)
                                palListBox.color = 'transparent'
                                palList.model    = palettes
                                editor.internalFocus = false
                                palList.forceActiveFocus()

                                palList.cancelled.connect(function(){
                                    palList.focus = false
                                    editor.forceFocus()
                                    palListBox.destroy()
                                })
                                palList.paletteSelected.connect(function(index){
                                    palList.focus = false
                                    editor.forceFocus()
                                    palListBox.destroy()

                                    var palette = palettes.loadAt(index)
                                    codeHandler.openPalette(palette, editorArea.cursorPosition, editor.windowControls.runSpace.item)

                                    var paletteBox = windowControls.editSpace.createPalette()

                                    palette.item.x = 5
                                    palette.item.y = 7

                                    paletteBox.setChild(palette.item, rect, position, editSpace.placement.top)

                                    paletteBox.color = "#02070b"
                                    paletteBox.radius = 5
                                    paletteBox.border.width = 1
                                    paletteBox.border.color = "#061b24"

                                    editor.loadedPalettes.push(paletteBox)
                                })
                            }
                        }
                    }
                    MenuItem {
                        id: bindMenuItem
                        text: qsTr("Bind")
                        enabled: false
                        onTriggered: codeHandler.bind(
                            editorArea.selectionStart,
                            editorArea.selectionEnd - editorArea.selectionStart,
                            editor.windowControls.runSpace.item
                        )
                    }

                    MenuItem {
                        id: unbindMenuItem
                        text: qsTr("Unbind")
                        enabled: false
                        onTriggered: codeHandler.unbind(
                            editorArea.selectionStart, editorArea.selectionEnd - editorArea.selectionStart
                        )
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

