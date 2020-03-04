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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0

CodePalette{
    id: palette
    type : "qml/property"

    property font inputFont: Qt.font({
        family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
        pixelSize: 12,
        weight: Font.Normal
    })

    property CodeCompletionModel codeModel : CodeCompletionModel{}

    item: Rectangle{

        width: 280
        height: 40
        color: 'transparent'

        //TODO: ErrorBox when not binding

        InputBox{
            id: input
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 5
            height: 25
            border.width: 1
            border.color: "#031728"
            text: ''
            onTextChanged: {
                if ( !autoTextChange ){
                   extension.suggestionsForExpression(input.text, palette.codeModel, false)
               }
            }

            property bool autoTextChange : false

            onKeyPressed: {
                if ( event.key === Qt.Key_PageUp ){
                    if ( palette.codeModel.isEnabled ){
                        qmlSuggestionBox.highlightPrevPage()
                    }
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageDown ){
                    if ( palette.codeModel.isEnabled ){
                        qmlSuggestionBox.highlightNextPage()
                    }
                    event.accepted = true
                } else if ( event.key === Qt.Key_Down ){
                    if ( palette.codeModel.isEnabled ){
                        event.accepted = true
                        qmlSuggestionBox.highlightNext()
                    }
                } else if ( event.key === Qt.Key_Up ){
                    if ( palette.codeModel.isEnabled ){
                        event.accepted = true
                        qmlSuggestionBox.highlightPrev()
                    }
                } else if ( event.key === Qt.Key_Escape ){
                    if ( palette.codeModel.isEnabled ){
                        palette.codeModel.disable()
                    }
                } else if ( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ){

                    if ( palette.codeModel.isEnabled ){
                        autoTextChange = true
                        var beforeText = text.substr(0, palette.codeModel.completionPosition)
                        text = beforeText + qmlSuggestionBox.getCompletion()
                        palette.codeModel.disable()
                        autoTextChange = false
                        event.accepted = true
                    } else {
                        var result = extension.bindExpression(input.text)
                        if ( result ){
                            extension.write({'__ref': input.text})
                        }
                    }

                } else if ( event.key === Qt.Key_Space && event.modifiers & Qt.AltModifier ){
                    extension.suggestionsForExpression(input.text, palette.codeModel, false)
                    event.accepted = true
                }
            }

            color: '#050e15'

            font.family: palette.inputFont.family
            font.pixelSize: palette.inputFont.pixelSize
        }

        SuggestionBox{
            id: qmlSuggestionBox
            anchors.left: input.left
            anchors.top: input.bottom
            width: input.width

            fontFamily: palette.inputFont.family
            fontSize: palette.inputFont.pixelSize
            smallFontSize: palette.inputFont.pixelSize - 2
            visible: palette.codeModel.isEnabled
            opacity: visible ? 0.95 : 0

            model: palette.codeModel

            Behavior on opacity {
                NumberAnimation { duration: 150 }
            }
        }
    }

    onInit: {
        input.forceActiveFocus()
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }
}
