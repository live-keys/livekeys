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

Rectangle{
    id : root

    width : 100
    height : 50

    property string text : "Button"

    property string fontFamily : "Ubuntu Mono, Courier New, Courier"
    property int fontPixelSize : 14

    property color textColor : "#fff"
    property color backgroundHoverColor : "#082134"
    property color backgroundColor : "#061a29"

    signal clicked()

    color : buttonMouseArea.containsMouse ? backgroundHoverColor : backgroundColor

    Text{
        color : root.textColor
        text : root.text
        font.family: root.fontFamily
        font.pixelSize: root.fontPixelSize
        anchors.centerIn: parent
    }

    MouseArea{
        id : buttonMouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: root.clicked()
    }
}

