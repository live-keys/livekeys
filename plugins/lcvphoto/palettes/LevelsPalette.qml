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
import lcvcore 1.0 as Cv
import lcvphoto 1.0

CodePalette{
    id: palette

    type : "qml/Levels"

    item: LevelsSliders{
        id: levelsSliders

        property QtObject levels: null
        onLevelsChanged: {
            if ( levels ){
                lightness = levels.lightness
                levelByChannel = levels.channels ? levels.channels : {}
            } else {
                lightness = []
                levelByChannel = {}
            }

            updateSliders()
        }

        color: 'transparent'
        input: levels ? levels.input : Cv.MatOp.nullMat

        onLightnessChanged: {
            if ( !isBindingChange() ){
                if ( levels ){
                    levels.lightness = lightness
                }
                if ( editFragment && lightness ){
                    editFragment.writeProperties({
                        'lightness' : lightness
                    })
                }
            }
        }
        onLevelByChannelChanged: {
            if ( !isBindingChange() ){
                if ( levels ){
                    levels.channels = levelByChannel
                }
                if ( editFragment && levelByChannel ){
                    editFragment.writeProperties({
                        'channels' : levelByChannel
                    })
                }

            }
        }
    }

    onValueFromBindingChanged: {
        levelsSliders.levels = value
    }
    onInit: {
        levelsSliders.levels = value
    }
}
