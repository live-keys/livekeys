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

    property QtObject theme: lk.layers.workspace.themes.current

    item: Item{
        width: 280
        height: colorPicker.height

        Workspace.ColorPicker{
            id: colorPicker

            style: QtObject{
                property QtObject input: theme.inputStyle
                property double colorDisplayBoderWidth: 1
                property color  colorDisplayBoderColor: theme.inputStyle.borderColor
                property double colorDisplayRadius: 2
                property color adjustmentBackground: 'transparent'
                property color adjustmentBorderColor: 'transparent'
                property int adjustmentBorderWidth: 0
                property real adjustmentRadius: 3
                property QtObject labelStyle: theme.inputLabelStyle
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
