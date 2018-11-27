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
import editor 1.0
import editor.private 1.0
import base 1.0

Rectangle{
    id: root

    visible: false
    color: "transparent"

    property var entry: null
    property bool isFile : false


    onVisibleChanged: {
        if ( !visible ){
            close()
            canceled()
        }
    }

    function show(entry, isFile){
        root.entry = entry
        root.isFile = isFile
        root.visible = true
        addEntryInput.forceActiveFocus()
    }
    function close(){
        root.entry = null
        addEntryInput.text = ''
        visible = false
    }


    signal accepted(ProjectEntry entry, string name, bool isFile)
    signal canceled()

    MouseArea{
        anchors.fill: parent
        onClicked: mouse.accepted = true;
        onPressed: mouse.accepted = true;
        onReleased: mouse.accepted = true
        onDoubleClicked: mouse.accepted = true;
        onPositionChanged: mouse.accepted = true;
        onPressAndHold: mouse.accepted = true;
        onWheel: wheel.accepted = true
    }

    Rectangle{
        anchors.fill: parent
        color: "#030609"
        opacity: root.visible ? 0.92 : 0
        Behavior on opacity{ NumberAnimation{ duration: 250} }
    }

    Rectangle{
        clip: true
        width: parent.width > 570 ? 570 : parent.width - 10
        anchors.horizontalCenter: parent.horizontalCenter
        height: 80
        color:"#081621"
        border.width: 1
        border.color: "#0a1a26"

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
            color: "#0c1d2a"
            border.width: 1
            border.color: "#071520"

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
                    root.close()
                }
                Keys.onEscapePressed: {
                    root.close()
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
}
