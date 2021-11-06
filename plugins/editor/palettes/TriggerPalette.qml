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
import visual.input 1.0 as Input

CodePalette{
    id: palette
    type : "qml/property"

    property font inputFont: Qt.font({
        family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
        pixelSize: 12,
        weight: Font.Normal
    })

    property var paletteFunctions: lk.layers.workspace.extensions.editqml.paletteFunctions


    property QtObject theme: lk.layers.workspace.themes.current

    property CodeCompletionModel codeModel : CodeCompletionModel{}

    property var qmlSuggestionBox: null

    function writeBinding(){
        var text = input.text

        var ef = palette.editFragment

        try{
            var result = ef.bindFunctionExpression(text)

            if ( result ){
                ef.write({'__ref': text ? text : ef.defaultValue()})
                commitButton.visible = false
            } else {
                lk.layers.workspace.messages.pushError("Failed to match expression: " + text)
                return
            }

            input.autoTextChange = true
            input.text = text
            input.autoTextChange = false

        } catch (e){
            var error = engine.unwrapError(e)
            lk.layers.workspace.messages.pushError(error.message, error.code)
        }


    }

    item: Rectangle{

        width: 280
        height: 40
        color: 'transparent'

        //TODO: ErrorBox when not binding

        Input.InputBox{
            id: input
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: 35
            anchors.top: parent.top
            height: 25

            style: theme.inputStyle

            onTextChanged: {
                if ( !autoTextChange ){
                   editFragment.suggestionsForExpression(input.text, palette.codeModel, true)
               }
               commitButton.visible = true
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
                    editFragment.suggestionsForExpression(input.text, palette.codeModel, true)
                    event.accepted = true
                } else {
                    var activePane = lk.layers.workspace.panes.activePane
                    var paneCoords = activePane.mapGlobalPosition()
                    var coords = input.mapToItem(activePane, 0, 0)

                    var rect = Qt.rect(coords.x, coords.y, input.width, input.height)

                    if (qmlSuggestionBox){
                        qmlSuggestionBox.parent.updatePlacement(rect, Qt.point(paneCoords.x + 113, paneCoords.y - 75), lk.layers.editor.environment.placement.bottom)
                        return
                    }
                    qmlSuggestionBox = lk.layers.editor.environment.createSuggestionBox(null, palette.inputFont)
                    qmlSuggestionBox.width = Qt.binding(function(){ return input.width })
                    qmlSuggestionBox.height = 200
                    qmlSuggestionBox.x = 0

                    qmlSuggestionBox.visible = Qt.binding(function(){ return palette.codeModel.isEnabled })

                    qmlSuggestionBox.opacity = Qt.binding(function(){
                        return palette.codeModel.isEnabled ? 0.95 : 0
                    })
                    qmlSuggestionBox.model = Qt.binding(function(){ return palette.codeModel })

                    var editorBox = lk.layers.editor.environment.createEditorBox(
                        qmlSuggestionBox, rect, Qt.point(paneCoords.x + 120, paneCoords.y - 32), lk.layers.editor.environment.placement.bottom
                    )
                    editorBox.color = 'transparent'
                }
            }
        }

        Input.Button{
            id: commitButton
            anchors.right: parent.right
            width: 30
            height: 25
            visible: false
            content: theme.buttons.connect
            onClicked: palette.writeBinding()
        }
    }

    onInit: {
        var contents = editFragment.readValueText()
        var tokens = QmlEdit.Tokenizer.scan(contents)

        var parsedContents = ''

        var isBindingExpression = true

        var leftParanthesisIndex = -1
        var rightParanthesisIndex = -1

        for ( var i = 0; i < tokens.length; ++i ){
            if ( tokens[i].kind === QmlEdit.Tokenizer.tokenKind.leftParenthesis )
            {
                if ( leftParanthesisIndex < 0 )
                    leftParanthesisIndex = i
                parsedContents += contents.substr(tokens[i].position, tokens[i].length)

            } else if ( tokens[i].kind === QmlEdit.Tokenizer.tokenKind.rightParenthesis ){
                if ( rightParanthesisIndex < 0 )
                    rightParanthesisIndex = i
                parsedContents += contents.substr(tokens[i].position, tokens[i].length)

            } else if ( tokens[i].kind === QmlEdit.Tokenizer.tokenKind.dot ||
                        tokens[i].kind === QmlEdit.Tokenizer.tokenKind.identifier )
            {
                parsedContents += contents.substr(tokens[i].position, tokens[i].length)
            } else {
                isBindingExpression = false
            }
        }

        if ( leftParanthesisIndex < 0 || rightParanthesisIndex !== leftParanthesisIndex + 1 || rightParanthesisIndex !== tokens.length - 1){
            isBindingExpression = false
        }

        if ( isBindingExpression ){
            input.autoTextChange = true
            input.text = parsedContents
            input.autoTextChange = false
        }

        input.forceActiveFocus()
        editFragment.whenBinding = function(){
            editFragment.write(palette.value)
        }
    }

    Component.onDestruction: {
        if (!qmlSuggestionBox) return
        var par = qmlSuggestionBox.parent
        qmlSuggestionBox.destroy()
        qmlSuggestionBox = null
        par.destroy()
    }
}
