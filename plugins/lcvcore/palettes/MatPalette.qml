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
import live 1.0
import editor 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img

CodePalette{
    id: palette

    type : "qml/lcvcore#Mat"

    property real lastX
    property real lastY
    property color selectedColor: "#fff"
    property var writable: null
    item: Cv.MatView{
        id: mView

        MouseArea{
            id: area
            anchors.fill: parent
            onPressed: {
                palette.lastX = mouseX
                palette.lastY = mouseY
            }
            onPositionChanged: {
                Img.Draw.line(
                    writable,
                    Qt.point(palette.lastX, palette.lastY),
                    Qt.point(mouseX, mouseY),
                    palette.selectedColor
                );
                palette.lastX = mouseX
                palette.lastY = mouseY
                mView.mat = writable.toMat()
                palette.value = mView.mat
            }
        }
    }

    onInit: {
        mView.mat = value
        writable = Cv.MatOp.createWritableFromMat(value)
    }
}
