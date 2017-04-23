/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import Cv 1.0

Rectangle{
    id : editor

    signal save()
    signal open()
    signal closeFocusedFile()
    signal toggleSize()
    signal toggleProject()
    signal toggleVisibility()
    signal toggleNavigation()

    property bool isDirtyMask: true
    property bool isDirty: false

    property alias text: editorArea.text
    property alias font: editorArea.font

    color : "#050c13"

    clip : true

    function forceFocus(){
        editorArea.forceActiveFocus()
    }

    Rectangle{
        anchors.left: parent.left
        anchors.top: parent.top

        width: parent.width
        height: 30

        color : "#000"
        gradient: Gradient{
            GradientStop { position: 0.0;  color: "#08141f" }
            GradientStop { position: 0.10; color: "#071119" }
        }

        Text{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 15
            color: "#7b838b"
            text: {
                if ( project.inFocus ){
                    var filename = project.inFocus.file.name
                    if ( filename === '' )
                        filename = 'untitled'
                    if ( project.inFocus.file.isDirty )
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
            anchors.rightMargin: 35
            visible : project.inFocus !== null
            Text{
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 16
                font.weight: Font.Light
                text: 'x'
                color: "#7b838b"
                anchors.centerIn: parent
            }
            MouseArea{
                anchors.fill: parent
                onClicked: editor.closeFocusedFile()
            }
        }

        Rectangle{
            anchors.right: parent.right
            width: 30
            height: parent.height
            gradient: Gradient{
                GradientStop { position: 0.0;  color: "#08141f" }
                GradientStop { position: 0.30; color: "#08131c" }
            }

            Image{
                id : openProjectImage
                anchors.centerIn: parent
                source : "qrc:/images/toggle-navigation.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: editor.toggleNavigation()
            }
        }
    }

    Rectangle{
        anchors.fill: parent
        anchors.topMargin: 38
        color: editor.color

        ScrollView {
            id: flick
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
                frame: Rectangle{color: editor.color}
                corner: Rectangle{color: editor.color}
            }

            anchors.fill: parent
            anchors.leftMargin: 10

            frameVisible: false

            function ensureVisible(r){
                if (flickableItem.contentX >= r.x)
                    flickableItem.contentX = r.x;
                else if (flickableItem.contentX + width <= r.x + r.width + 20)
                    flickableItem.contentX = r.x + r.width - width + 20;
                if (flickableItem.contentY >= r.y)
                    flickableItem.contentY = r.y;
                else if (flickableItem.contentY + height <= r.y + r.height + 20)
                    flickableItem.contentY = r.y + r.height - height + 20;
            }

            TextEdit {
                id : editorArea

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
                onTextChanged: {
                    if ( editor.isDirtyMask )
                        editor.isDirtyMask = false
                    else {
                        editor.isDirty = true
                        if ( project.inFocus )
                            project.inFocus.file.isDirty = true
                    }
                }

                color : "#fff"
                font.family: "Source Code Pro, Ubuntu Mono, Courier New, Courier"
                font.pixelSize: 13
                font.weight: Font.Normal

                selectByMouse: true
                mouseSelectionMode: TextEdit.SelectCharacters
                selectionColor: "#3d4856"

                textFormat: TextEdit.PlainText

                wrapMode: TextEdit.NoWrap
                height : Math.max( flick.height - 20, paintedHeight )
                width : Math.max( flick.width - 20, paintedWidth )

                readOnly: project.inFocus === null || project.inFocus.file.isMonitored

                Keys.onPressed: {
                    if ( (event.key === Qt.Key_BracketRight && (event.modifiers & Qt.ShiftModifier) ) ||
                         (event.key === Qt.Key_BraceRight) ){

                        event.accepted = true
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
                    } else if ( event.key === Qt.Key_Space && (event.modifiers & Qt.ControlModifier ) ){
                        codeHandler.generateCompletion(cursorPosition)
                        event.accepted = true
                    } else if ( event.key === Qt.Key_Escape ){
                        codeHandler.completionModel.disable()
                    } else if ( event.key === Qt.Key_S && (event.modifiers & Qt.ControlModifier ) ){
                        editor.save()
                        event.accepted = true
                    } else if ( event.key === Qt.Key_K && (event.modifiers & Qt.ControlModifier ) ){
                        editor.toggleNavigation()
                        event.accepted = true
                    } else if ( event.key === Qt.Key_W && (event.modifiers & Qt.ControlModifier ) ){
                        editor.closeFocusedFile()
                        event.accepted = true
                    } else if ( event.key === Qt.Key_O && (event.modifiers & Qt.ControlModifier ) ) {
                        editor.open()
                        event.accepted = true
                    } else if ( event.key === Qt.Key_E && (event.modifiers & Qt.ControlModifier ) ){
                        editor.toggleSize()
                        event.accepted = true
                    } else if ( event.key === Qt.Key_T && (event.modifiers & Qt.ControlModifier ) ){
                        editor.toggleVisibility()
                        event.accepted = true
                    } else if ( event.key === Qt.Key_Backslash && (event.modifiers & Qt.ControlModifier ) ){
                        editor.toggleProject()
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
                    } else
                        event.accepted = false
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
                Keys.onTabPressed: {
                    event.accepted = true
                    var clastpos = cursorPosition
                    editorArea.text = editorArea.text.slice(0, clastpos) + "    " + editorArea.text.slice(clastpos)
                    editorArea.cursorPosition = clastpos + 4
                }

                Component.onCompleted: {
                    codeHandler.target = textDocument
                    if ( project.inFocus ){
                        editor.isDirtyMask = true
                        editor.isDirty = project.inFocus.file.isDirty
                        editor.text    = project.inFocus.content
                    }
                }

                Behavior on font.pixelSize {
                    NumberAnimation { duration: 40 }
                }

                Connections{
                    target: codeHandler
                    onCursorPositionRequest : editorArea.cursorPosition = position
                }

                Connections{
                    target: project
                    onInFocusChanged : {
                        editor.isDirtyMask = true
                        if ( project.inFocus ){
                            editor.isDirty = project.inFocus.file.isDirty
                            editor.text    = project.inFocus.content
                        } else {
                            editor.text = ''
                            editor.isDirty = false
                        }
                    }
                }

                Connections{
                    target: project.inFocus
                    onContentChanged : {
                        editor.isDirtyMask = true
                        editor.isDirty = project.inFocus.file.isDirty
                        editor.text    = project.inFocus.content
                    }
                }

                MouseArea{
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: Qt.IBeamCursor
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

