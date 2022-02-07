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

Rectangle {
    id : root
    width: 100
    height: 30
    border.color : "#323232"
    border.width : 1
    color : "#070b0f"
    
    property real margins: 6
    property color textColor : "#fff"
    property color hintTextColor : "#ccc"
    property color textSelectionColor : "#3d4856"

    property alias text : textInput.text
    property alias font: textInput.font

    property string textHint : ''
    clip: true

    signal keyPressed(var event)

    function selectAll(){
        textInput.selectAll()
    }

    TextInput{
        id : textInput
        anchors.fill: parent
        anchors.margins : root.margins
        font.family : "Ubuntu Mono, Courier New, Courier"
        font.pixelSize: 14
        font.weight : Font.Normal
        text: ''
        color : root.textColor
        selectByMouse: true
        selectionColor: root.textSelectionColor

        property bool touched : false

        Keys.onPressed : root.keyPressed(event)

        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.IBeamCursor
        }
    }

    Text {
        anchors.fill: parent
        anchors.margins : textInput.anchors.margins
        text: root.textHint
        font: textInput.font
        color: root.hintTextColor
        visible: !textInput.text && !textInput.activeFocus
    }
}

