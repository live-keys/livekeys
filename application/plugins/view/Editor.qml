/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

import QtQuick 2.2
import Cv 1.0

Rectangle{
    id : editor

    signal save()
    signal open()
    signal toggleSize()

    property bool isDirty: false
    property alias text: editorArea.text


    color : "#041725"
    clip : true

    Flickable {
        id: flick

        anchors.fill: parent
        anchors.leftMargin: 9
        anchors.topMargin: 8
        anchors.bottomMargin: 30
        contentWidth: editorArea.paintedWidth
        contentHeight: editorArea.paintedHeight

        function ensureVisible(r){
            if (contentX >= r.x)
                contentX = r.x;
            else if (contentX + width <= r.x + r.width)
                contentX = r.x + r.width - width;
            if (contentY >= r.y)
                contentY = r.y;
            else if (contentY + height <= r.y + r.height)
                contentY = r.y + r.height - height;
        }

        TextEdit {
            id : editorArea
            property bool isDirty : false

            onCursorRectangleChanged: {
                flick.ensureVisible(cursorRectangle)
            }

            focus : false
            text : "Rectangle{\n}"
            color : "#eeeeee"
            font.family: "Courier New, Courier"
            font.pixelSize: 13

            selectByMouse: true
            mouseSelectionMode: TextEdit.SelectCharacters
            selectionColor: "#3d4856"

            textFormat: TextEdit.PlainText

            wrapMode: TextEdit.NoWrap
            height : Math.max( flick.height, paintedHeight )
            width : Math.max( flick.width, paintedWidth )

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
                } else if ( event.key === Qt.Key_S && (event.modifiers & Qt.ControlModifier ) ){
                    editorArea.save()
                    event.accepted = true
                } else if ( event.key === Qt.Key_O && (event.modifiers & Qt.ControlModifier ) ) {
                    editorArea.open()
                    event.accepted = true
                } else if ( event.key === Qt.Key_E && (event.modifiers & Qt.ControlModifier ) ){
                    editorArea.toggleSize()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageUp ){
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
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageDown ){
                    var lines = flick.height / cursorRectangle.height
                    var nextLineStartPos = editorArea.text.indexOf('\n', cursorPosition)
                    while ( lines-- > 0 && nextLineStartPos !== -1 ){
                        cursorPosition   = nextLineStartPos + 1
                        nextLineStartPos = editorArea.text.indexOf('\n', cursorPosition)
                    }
                    event.accepted = true
                } else
                    event.accepted = false
            }

            Keys.onReturnPressed: {
                event.accepted = true
                var clastpos = cursorPosition
                var append = ""
                var cpos = cursorPosition - 1
                var bracketSearchEnd = false
                while ( cpos > 0 ){
                    if ( !bracketSearchEnd ){
                        if ( editorArea.text.charAt(cpos) !== ' ' ){
                            if ( editorArea.text.charAt(cpos) === '{' ){
                                append += "    "
                                bracketSearchEnd = true
                            } else {
                                bracketSearchEnd = true
                            }
                        }
                    }// bracket search
                    if ( editorArea.text.charAt(cpos) === '\n' ){
                        ++cpos;
                        while( editorArea.text.charAt(cpos) === '\t' || editorArea.text.charAt(cpos) === ' ' ){
                            append += editorArea.text.charAt(cpos)
                            ++cpos
                        }
                        break;
                    }
                    --cpos;
                }
                editorArea.text = editorArea.text.slice(0, clastpos) + "\n" + append + editorArea.text.slice(clastpos)
                editorArea.cursorPosition = clastpos + 1 + append.length
            }
            Keys.onTabPressed: {
                event.accepted = true
                var clastpos = cursorPosition
                editorArea.text = editorArea.text.slice(0, clastpos) + "    " + editorArea.text.slice(clastpos)
                editorArea.cursorPosition = clastpos + 4
            }


            CodeHandler{
                id : codeH
                Component.onCompleted: {
                    codeH.target = parent.textDocument
                }
            }

            Behavior on font.pixelSize {
                NumberAnimation { duration: 40 }
            }
            Component.onCompleted: isDirty = false
        }

    }

    Rectangle{
        id : errorWrap
        anchors.bottom: parent.bottom
        height : error.text !== '' ? 30 : 0
        width : parent.width
        color : "#141a1a"
        Behavior on height {
            SpringAnimation { spring: 3; damping: 0.1 }
        }

        Rectangle{
            width : 14
            height : parent.height
            color : "#601818"
            visible: error.text === "" ? false : true
        }
        Text {
            id: error
            anchors.left : parent.left
            anchors.leftMargin: 25
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            font.pointSize: 25 * editorArea.fontScale
            text: ""
            onTextChanged : console.log(text)
            color: "#c5d0d7"
        }
    }

}

