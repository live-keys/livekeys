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
import lcvphoto 1.0

CodePalette{
    id: palette

    type : "qml/lcvphoto#HueSaturationLightness"

    property QtObject theme: lk.layers.workspace.themes.current

    item: Rectangle{
        id: adjustmentBox
        width: 280
        height: 75
        color: 'transparent'

        property QtObject hsl: null

        HueSaturationLightnessSliders{

            style: QtObject {
                property QtObject labelStyle : theme.inputLabelStyle
            }

            hue: hsl ? hsl.hue : 100
            onHueChanged: {
                if ( !isBindingChange() ){
                    if ( adjustmentBox.hsl && adjustmentBox.hsl.input ){
                        adjustmentBox.hsl.hue = hue
                    }
                    if ( editFragment ){
                        editFragment.writeProperties({
                            'hue' : hue
                        })
                    }
                }
            }

            saturation: hsl ? hsl.saturation : 100
            onSaturationChanged: {
                if ( !isBindingChange() ){
                    if ( adjustmentBox.hsl && adjustmentBox.hsl.input ){
                        adjustmentBox.hsl.saturation = saturation
                    }
                    if ( editFragment ){
                        editFragment.writeProperties({
                            'saturation' : saturation
                        })
                    }
                }
            }

            lightness: hsl ? hsl.lightness : 100
            onLightnessChanged: {
                if ( !isBindingChange() ){
                    if ( adjustmentBox.hsl && adjustmentBox.hsl.input ){
                        adjustmentBox.hsl.lightness = lightness
                    }
                    if ( editFragment ){
                        editFragment.writeProperties({
                            'lightness' : lightness
                        })
                    }
                }
            }
        }

    }

    onInit: {
        adjustmentBox.hsl = value
        editFragment.whenBinding = function(){
            editFragment.writeProperties({
                'hue' : palette.value.hue,
                'saturation' : palette.value.saturation,
                'lightness': palette.value.lightness
            })
        }
    }
    onValueFromBindingChanged: {
        adjustmentBox.hsl = value
    }
}
