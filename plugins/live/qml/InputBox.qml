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

Rectangle {
    id : root
    anchors.fill: parent
    width: 100
    height: 30
    border.color : "#031728"
    border.width : 1
    color : "#050e15"
    
    property color textColor : "#fff"
    property color textSelectionColor : "#3d4856"


    property alias text : textInput.text

    TextInput{
        id : textInput
        anchors.fill: parent
        anchors.margins : 6
        font.family : "Ubuntu Mono, Courier New, Courier"
        font.pixelSize: 14
        font.weight : Font.Normal
        text: 'Input Box'
        color : root.textColor
        selectByMouse: true
        selectionColor: root.textSelectionColor

        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.IBeamCursor
        }
    }
}

