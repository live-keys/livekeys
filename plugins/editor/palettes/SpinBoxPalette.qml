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
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0 as L
import visual.shapes 1.0 as Vs
import visual.input 1.0 as Input

CodePalette{
    id: palette
    type : "qml/double"

    property QtObject theme: lk.layers.workspace.themes.current

    writer: function(){ editFragment.write(palette.value) }

    item: Item{
        width: 200
        height: 25

        Input.NumberSpinBox{
            id: spinbox
            anchors.fill: parent
            style: palette.theme.numberSpinBoxStyle
            onValueChanged: {
                palette.value = parseFloat(value)
                editFragment.writeCode(value)
            }
            from: 0
            to: 30000
        }
    }

    onInit: {
        editFragment.whenBinding = function(){
            editFragment.write(palette.value)
        }
    }
    onValueFromBindingChanged: {
        spinbox.initializeValue(value)
    }
}
