/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

MouseArea {
    id: selectArea
    anchors.fill: parent

    property color selectionColor : "#ff0000"
    property real selectionOpacity : 0.35

    signal selected(int x, int y, int width, int height)

    onPressed: {
        if (highlightItem !== null)
            highlightItem.destroy();

        highlightItem = highlightComponent.createObject(selectArea, {
            "x" : mouse.x,
            "y" : mouse.y,
            "ox" : mouse.x,
            "oy" : mouse.y
        })
    }
    onPositionChanged: {
        var mouseX = mouse.x < 0 ? 0 : mouse.x > selectArea.width ? selectArea.width : mouse.x
        var mouseY = mouse.y < 0 ? 0 : mouse.y > selectArea.height ? selectArea.height : mouse.y

        highlightItem.width  = Math.abs(mouseX - highlightItem.ox)
        highlightItem.height = Math.abs(mouseY - highlightItem.oy)

        if ( mouseX < highlightItem.ox )
            highlightItem.x = mouseX
        if ( mouseY < highlightItem.oy )
            highlightItem.y = mouseY
    }
    onReleased : {
        selectArea.selected(highlightItem.x, highlightItem.y, highlightItem.width, highlightItem.height)
    }

    property Rectangle highlightItem : null

    Component {
        id: highlightComponent

        Rectangle{
            property int ox : 0
            property int oy : 0

            color: selectArea.selectionColor
            opacity: selectArea.selectionOpacity
        }
    }
}

