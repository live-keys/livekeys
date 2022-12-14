/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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
import base 1.0
import editqml 1.0
import visual.input 1.0 as Input

CodePalette{
    id: palette

    type: "qml/base#ValueFlowGraph"

    property var current : null
    property var valueFlow: null
    property var valueFlowWatcher: null

    property QtObject theme: lk.layers.workspace.themes.current

    property Component watcherFactory: Component{ Watcher{} }

    item: Item{
        id: execBox
        width: 200
        height: 25


        Input.TextButton{
            id: textButton
            visible: palette.valueFlow ? false : true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            height: 28
            width: 160
            style: theme.formButtonStyle
            text: "Watch Single ValueFlow"

            onClicked: {
                palette.valueFlow = palette.current.source.createObject()
                palette.valueFlowWatcher = palette.watcherFactory.createObject()
                palette.valueFlowWatcher.setDynamicTarget(palette.valueFlow, "valueFlowFromGraph")
            }
        }

        property bool commitButtonVisible: true

        Input.InputBox{
            id: valueInput
            anchors.left: parent.left
            width: parent.width - 30
            height: 25
            visible: palette.valueFlow ? true : false

            style: theme.inputStyle
            onTextChanged: execBox.commitButtonVisible = true

            onKeyPressed: {
                if ( event.key === Qt.Key_Return ){
                    palette.valueFlow.value = valueInput.text
                    execBox.commitButtonVisible = false
                    event.accepted = true
                }
            }
        }

        Input.Button{
            id: commitButton
            anchors.right: parent.right
            width: 30
            height: 25
            visible: valueInput.visible && execBox.commitButtonVisible
            content: theme.buttons.apply
            onClicked: {
                palette.valueFlow.value = valueInput.text
                execBox.commitButtonVisible = false
            }
        }


    }

    onValueFromBindingChanged: {
        palette.current = value
    }
    onInit: {
        palette.current = value
        editFragment.whenBinding = function(){}
    }

}
