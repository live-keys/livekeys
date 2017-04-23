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

Rectangle{
    id: root

    property alias text: buttonLabel.text
    property var callback : function(){}

    signal clicked()
    onClicked: callback()

    color: buttonMouseArea.containsMouse ? "#0e2435" : "#0a1a27"
    width: 100
    height: 30

    Text{
        id: buttonLabel
        anchors.centerIn: parent
        text: ""
        color: "#ccc"
        font.pixelSize: 12
        font.family: "Open Sans, sans-serif"
        font.weight: Font.Light
    }
    MouseArea{
        id: buttonMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked()
        }
    }
}
