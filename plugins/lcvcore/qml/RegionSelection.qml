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

import QtQuick 2.0

Item{

    id : root

    property Item item : Item{}
    property color borderColor: "#ff0000"

    MouseArea{
        anchors.fill : parent
        onClicked : {
            root.item.regionX = mouse.x - 10
            root.item.regionY = mouse.y - 10
        }
    }
    Rectangle{
        width : root.item.regionWidth
        height : root.item.regionHeight
        color : "transparent"
        border.width : 1
        border.color : root.borderColor
        x: root.item.regionX
        y: root.item.regionY
    }

}
