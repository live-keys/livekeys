/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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
import live 1.0
import editor 1.0
import lcvcore 1.0
import lcvimgproc 1.0

LivePalette{
    id: palette

    type : "Mat"
    serialize : QmlObjectCodeSerializer{}

    property real lastX
    property real lastY
    property color selectedColor: "#fff"

    item: MatView{
        id: mView

        MouseArea{
            id: area
            anchors.fill: parent
            onPressed: {
                palette.lastX = mouseX
                palette.lastY = mouseY
            }
            onPositionChanged: {
                palette.drawing.lineOn(
                    mView.mat,
                    Qt.point(palette.lastX, palette.lastY),
                    Qt.point(mouseX, mouseY),
                    palette.selectedColor
                );
                palette.lastX = mouseX
                palette.lastY = mouseY
                mView.mat = mView.mat
                palette.value = mView.mat
            }
        }
    }

    property MatDraw drawing : MatDraw{}

    onInit: {
        mView.mat = value
    }
    onCodeChanged:{
        mView.mat = value
    }
}
