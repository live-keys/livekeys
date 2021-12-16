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

    property QtObject theme: lk.layers.workspace.themes.current

    function writeBinding(){
        var text = editor.textEdit.text

        try{
            if ( text ){
                var fn = palette.editFragment.compileFunctionInContext(text)
                palette.assignFunction(fn)
                palette.editFragment.write({'__ref': "(" + text + ")"})
            }
        } catch (e){
            var error = engine.unwrapError(e)
            lk.layers.workspace.messages.pushError(error.message, error.code)
        }
    }

    item: Item{

        width: 300
        height: 80

        //TODO: ErrorBox when not compiling

        Editor{
            id: editor
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.rightMargin: 35
            anchors.top: parent.top
            height: parent.height - 5
            style: theme.editorStyle
            document: lk.layers.workspace.project.createDocument({fileSystem: false, type: 'text'})
            onKeyPress: {
                if ( (event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
                        && (event.modifiers & Qt.ControlModifier) )
                {
                    palette.writeBinding()
                    commitButton.visible = false
                }
            }

            Connections{
                target: editor.textEdit
                function onTextChanged(){
                    commitButton.visible = true
                }
            }
        }

//        Input.InputBox{
//            id: input
//            anchors.left: parent.left
//            anchors.right: parent.right
//            anchors.rightMargin: 35
//            anchors.top: parent.top
//            height: 25

//            style: theme.inputStyle

//            onTextChanged: {
//               commitButton.visible = true
//            }

//            onKeyPressed: {
//                if ( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ){
//                    palette.writeBinding()
//                    commitButton.visible = false
//                }
//            }
//        }

        Input.Button{
            id: commitButton
            anchors.right: parent.right
            width: 30
            height: 25
            visible: false
            content: theme.buttons.apply
            onClicked: {
                palette.writeBinding()
                visible = false
            }
        }

        ResizeArea{
            minimumHeight: 50
            minimumWidth: 150
        }
    }

    onInit: {
        var contents = editFragment.readValueText()
        if ( contents ){
            editor.textEdit.text = contents
        }
    }
}
