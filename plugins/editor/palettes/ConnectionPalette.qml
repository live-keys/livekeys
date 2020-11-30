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
import editqml 1.0 as QmlEdit
import live 1.0
import workspace 1.0 as Workspace

CodePalette{
    id: palette
    type : "qml/property"

    property font inputFont: Qt.font({
        family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
        pixelSize: 12,
        weight: Font.Normal
    })

    property QtObject theme: lk.layers.workspace.themes.current

    property CodeCompletionModel codeModel : CodeCompletionModel{}

    function writeBinding(){
        var text = input.text

        var ef = palette.editFragment
        var result = ef.bindExpression(text)

        if ( result ){
            ef.write({'__ref': text ? text : ef.defaultValue()}, codeHandler)
        }

        input.autoTextChange = true
        input.text = text
        input.autoTextChange = false
    }

    item: Rectangle{

        width: 280
        height: 25
        color: 'transparent'

        //TODO: ErrorBox when not binding

        Workspace.InputBox{
            id: input
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: 35
            anchors.top: parent.top
            height: 25

            style: theme.inputStyle

            onTextChanged: {
                if ( !autoTextChange ){
                   editFragment.suggestionsForExpression(input.text, palette.codeModel, false)
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
                        palette.writeBinding()
                    }

                } else if ( event.key === Qt.Key_Space && event.modifiers & Qt.AltModifier ){
                    editFragment.suggestionsForExpression(input.text, palette.codeModel, false)
                    event.accepted = true
                }
            }
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

        Workspace.Button{
            anchors.right: parent.right
            width: 30
            height: 25
            content: theme.buttons.connect
            onClicked: {
                palette.writeBinding()
            }
        }
    }

    onInit: {
        var contents = editFragment.readValueText()
        var tokens = QmlEdit.Tokenizer.scan(contents)

        var parsedContents = ''

        var isBindingExpression = true
        for ( var i = 0; i < tokens.length; ++i ){
            if ( tokens[i].kind !== QmlEdit.Tokenizer.tokenKind.dot &&
                 tokens[i].kind !== QmlEdit.Tokenizer.tokenKind.identifier )
            {
                isBindingExpression = false
            } else {
                parsedContents += contents.substr(tokens[i].position, tokens[i].length)
            }
        }

        if ( isBindingExpression ){
            input.autoTextChange = true
            input.text = parsedContents
            input.autoTextChange = false
        }

        input.forceActiveFocus()
    }

    onEditFragmentChanged: {
        editFragment.whenBinding = function(){
            editFragment.write(palette.value, codeHandler)
        }
    }
}
