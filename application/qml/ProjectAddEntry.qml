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
import editor 1.0
import editor.private 1.0
import base 1.0

Rectangle{
    id: root

    property var entry: null
    property bool isFile : false

    signal accepted(ProjectEntry entry, string name, bool isFile)
    signal canceled()

    clip: true
    width: 500
    height: 80
    radius: 5
    color:"#02070b"
    border.width: 1
    border.color: "#0f1921"

    opacity: root.visible ? 1 : 0
    Behavior on opacity{ NumberAnimation{ duration: 250} }

    Text{
        anchors.top: parent.top
        anchors.topMargin: 14
        anchors.left: parent.left
        anchors.leftMargin: 14
        text: 'Add ' + (root.isFile ? 'file' : 'directory') + ' in ' + (root.entry ? root.entry.path : '')
        font.family: 'Open Sans, Arial, sans-serif'
        font.pixelSize: 12
        font.weight: Font.Normal
        color: "#afafaf"
    }
    Rectangle{
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 14
        width: parent.width - 28
        height: 28
        color: "#0b1319"
        border.width: 1
        border.color: "#0c1720"

        TextInput{
            id: addEntryInput
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            color: '#aaa'
            text: ''
            font.family: 'Open Sans, Arial, sans-serif'
            font.pixelSize: 12
            font.weight: Font.Normal
            selectByMouse: true

            Keys.onReturnPressed: {
                root.accepted(root.entry, text, root.isFile)
                root.parent.parent.box = null
            }
            Keys.onEscapePressed: {
                root.parent.parent.box = null
                root.canceled()
            }
            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: Qt.IBeamCursor
            }
        }
    }
}
