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
import QtGraphicalEffects 1.0
import live 1.0
import editor 1.0
import workspace 1.0 as Workspace

CodePalette{
    id: palette
    type: "qml/color"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    item: Item{
        width: 280
        height: colorPicker.height

        Workspace.ColorPicker{
            id: colorPicker

            style: QtObject{
                property QtObject input: paletteStyle ? paletteStyle.inputStyle : colorPicker.defaultStyle.input
                property double colorDisplayBoderWidth: 1
                property color  colorDisplayBoderColor: paletteStyle ? paletteStyle.inputStyle.borderColor : colorPicker.defaultStyle.colorDisplayBoderColor
                property double colorDisplayRadius: 2
                property QtObject labelStyle: paletteStyle ? paletteStyle.labelStyle : colorPicker.defaultStyle.labelStyle
            }

            onSelectedColorChanged: {
                palette.value = selectedColor
                if ( !palette.isBindingChange() )
                    extension.write(palette.value.toString())
            }
        }
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }

    onInit: {
        colorPicker.selectedColor = value
    }
}
