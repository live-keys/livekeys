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
import QtGraphicalEffects 1.0
import live 1.0
import editor 1.0
import visual.input 1.0 as Input

CodePalette{
    id: palette
    type: "qml/color"

    property QtObject theme: lk.layers.workspace.themes.current

    property color selectedColor: 'white'
    function colorPicked(color){
        selectedColor = color
        palette.value = selectedColor
        if ( !palette.isBindingChange() ){
            editFragment.write(palette.value.toString())
        }
    }

    function toggleColorPicker(){
        colorPicker.visible = !colorPicker.visible
    }

    item: Item{
        width: colorPicker.visible ? colorPicker.width : (colorDisplay.width + (inputLoader.visible ? input.width + 2 : 0))
        height: 25 + (colorPicker.visible ? colorPicker.height + 2 : 0)

        Rectangle{
            id: colorDisplay
            anchors.top: parent.top
            anchors.left: parent.left
            width: 40
            height: 25
            color: 'transparent'
            radius: 3
            border.width: 1
            border.color: palette.theme.colorScheme.backgroundBorder

            Rectangle{
                anchors.centerIn: parent
                width: parent.width - 6
                height: parent.height - 6
                color: palette.selectedColor
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    palette.toggleColorPicker()
                }
            }
        }

        Loader{
            id: inputLoader
            visible: true
            active: true
            anchors.left: parent.left
            anchors.leftMargin: colorDisplay.width + 2

            Input.InputBox{
                id: input
                height: 25
                anchors.top: parent.top
                style: palette.theme.inputStyle

                text: palette.selectedColor
                onKeyPressed: {
                    if ( event.key === Qt.Key_Return )
                        palette.colorPicked(text)
                }
            }
        }

        Input.ColorPicker{
            id: colorPicker
            anchors.top: parent.top
            anchors.topMargin: 27
            visible: false
            height: 180

            style: Input.ColorPickerStyle{
                backgroundColor: theme.colorScheme.backgroundOverlay
                colorPanelBorderColor: theme.colorScheme.backgroundBorder
                inputBoxStyle: theme.inputStyle
                labelStyle: theme.inputStyle.textStyle
            }

            onSelectedColorChanged: {
                palette.colorPicked(selectedColor)
            }
        }
    }

    onInit: {
        palette.selectedColor = value
        colorPicker.color = value
        editFragment.whenBinding = function(){
            editFragment.write(palette.value)
        }
    }
    onValueFromBindingChanged: {
        palette.selectedColor = value
        colorPicker.color = value
    }
}
