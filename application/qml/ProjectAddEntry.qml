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
import editor 1.0
import editor.private 1.0
import base 1.0
import visual.input 1.0 as Input

Rectangle{
    id: root

    property var entry: null
    property bool isFile : false

    signal accepted(ProjectEntry entry, string name, bool isFile)
    signal canceled()

    property QtObject style: QtObject{
        property color background: "#02070b"
        property double borderWidth: 1
        property color borderColor: "#0f1921"
        property double radius: 5

        property QtObject headerStyle: Input.TextStyle{}
        property QtObject inputStyle: Input.InputBoxStyle{}

        property QtObject applyButton: Item{}
    }

    clip: true
    width: 500
    height: 60 + heading.height

    radius: style.radius
    color: style.background
    border.width: style.borderWidth
    border.color: style.borderColor

    property string heading: 'Add ' + (root.isFile ? 'file' : 'directory') + ' in ' + (root.entry ? root.entry.path : '')
    property string extension: ''

    function __accept(){
        if ( inputBox.text === '' ){
            root.__cancel()
        } else {
            var txt = inputBox.text
            if ( root.extension.length > 0 ){
                if ( !txt.endsWith('.' + root.extension) ){
                    txt += '.' + root.extension
                }
            }

            root.accepted(root.entry, txt, root.isFile)
            inputBox.text = ''
            root.extension = ''
            root.parent.parent.box = null
        }
    }

    function __cancel(){
        root.parent.parent.box = null
        inputBox.text = ''
        root.canceled()
    }

    function setInitialValue(value){
        inputBox.text = value
    }

    opacity: root.visible ? 1 : 0
    Behavior on opacity{ NumberAnimation{ duration: 250} }

    Input.Label{
        id: heading
        anchors.top: parent.top
        anchors.topMargin: 14
        anchors.left: parent.left
        anchors.leftMargin: 14
        anchors.right: parent.right
        anchors.rightMargin: 14
        text: root.heading
        wrapMode: Text.WordWrap
        textStyle: root.style.headerStyle
    }

    Input.InputBox{
        id: inputBox
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 14
        width: parent.width - 55
        height: 28
        style: root.style.inputStyle

        Keys.onPressed: {
            if ( event.key === Qt.Key_Return || event.key === Qt.Key_Enter ){
                root.__accept()
            } else if ( event.key === Qt.Key_Escape ){
                root.__cancel()
            }
        }
    }

    Input.Button{
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 12
        width: 30
        height: 25
        content: root.style.applyButton
        onClicked: root.__accept()
    }
}
