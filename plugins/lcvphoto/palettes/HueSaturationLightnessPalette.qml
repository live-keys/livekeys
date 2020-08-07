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
import lcvphoto 1.0

CodePalette{
    id: palette

    type : "qml/lcvphoto#HueSaturationLightness"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    item: Rectangle{
        id: adjustmentBox
        width: 280
        height: 75
        color: 'transparent'

        property QtObject hsl: null

        HueSaturationLightnessSliders{

            property QtObject loadedStyle : palette.paletteStyle

            style: palette.paletteStyle ? loadedStyle : defaultStyle

            hue: hsl ? hsl.hue : 100
            onHueChanged: {
                if ( adjustmentBox.hsl )
                    adjustmentBox.hsl.hue = hue
            }

            saturation: hsl ? hsl.saturation : 100
            onSaturationChanged: {
                if ( adjustmentBox.hsl )
                    adjustmentBox.hsl.saturation = hue
            }

            lightness: hsl ? hsl.lightness : 100
            onLightnessChanged: {
                if ( adjustmentBox.hsl )
                    adjustmentBox.hsl.lightness = hue
            }
        }

    }

    onInit: {
        adjustmentBox.hsl = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.writeProperties({
                'hue' : palette.value.brightness,
                'contrast' : palette.value.contrast
            })
        }
    }
}
