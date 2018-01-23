/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

LivePalette{
    id: palette

    type : "Levels"
    serialize : LevelsSerializer{}

    item: LevelsSliders{
        id: levelsSliders

        property Levels levels: null
        onLevelsChanged: {
            if ( levels ){
                input = levels.input
                lightness = levels.lightness
                levelByChannel = levels.channels ? levels.channels : {}
            } else {
                input = cv.nullMat
                lightnews = []
                levelByChannel = {}
            }

            updateSliders()
        }

        color: 'transparent'
        input: cv.nullMat
        onLightnessChanged: {
            levels.lightness = lightness
            palette.value = levels
        }
        onLevelByChannelChanged: {
            levels.channels = levelByChannel
            palette.value = levels
        }
    }

    onInit: {
        levelsSliders.levels = value
    }
    onCodeChanged:{
        levelsSliders.levels = value
    }
}
