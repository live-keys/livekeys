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

import QtQuick 2.11

Rectangle {
    id : root
    width: 100
    height: 30

    border.color : style.borderColor
    border.width : style.borderThickness
    color : style.backgroundColor
    radius: style.radius
    clip: true

    property real margins: 6

    property QtObject defaultStyle : InputBoxStyle{}
    property QtObject style: defaultStyle

    property alias text : textInput.text
    property alias font: textInput.font
    property alias inputActiveFocus: textInput.activeFocus
    property string textHint : ''

    property alias validator: textInput.validator

    property Item nextFocusItem: null
    property Item prevFocusItem: null

    signal keyPressed(var event)
    signal activeFocusLost()

    function selectAll(){
        textInput.selectAll()
    }
    function forceFocus(){
        textInput.forceActiveFocus()
    }

    TextInput{
        id : textInput
        anchors.fill: parent
        anchors.margins : root.margins
        font: root.style.textStyle.font
        text: ''
        color : root.style.textStyle.color
        selectByMouse: true
        selectionColor: root.style.textSelectionColor

        property bool touched : false

        Keys.onTabPressed: {
            if ( root.nextFocusItem ){
                if ( root.nextFocusItem.forceFocus )
                    root.nextFocusItem.forceFocus()
                else
                    root.nextFocusItem.forceActiveFocus()
                event.accepted = true
            }
        }
        Keys.onBacktabPressed: {
            if ( root.prevFocusItem ){
                if ( root.prevFocusItem.forceFocus )
                    root.prevFocusItem.forceFocus()
                else
                    root.prevFocusItem.forceActiveFocus()
                event.accepted = true
            }
        }

        Keys.onPressed : {
            if ( event.key === Qt.Key_Tab ){
                if ( root.nextFocusItem ){
                    if ( root.nextFocusItem.forceFocus )
                        root.nextFocusItem.forceFocus()
                    else
                        root.nextFocusItem.forceActiveFocus()
                    event.accepted = true
                }
            } else if ( event.ey === Qt.Key_Backtab ){
                if ( root.prevFocusItem ){
                    if ( root.prevFocusItem.forceFocus )
                        root.prevFocusItem.forceFocus()
                    else
                        root.prevFocusItem.forceActiveFocus()
                    event.accepted = true
                }
            }

            root.keyPressed(event)
        }

        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.IBeamCursor
        }

        onActiveFocusChanged: {
            if (!activeFocus)
                root.activeFocusLost()
        }
    }

    Text {
        anchors.fill: parent
        anchors.margins : textInput.anchors.margins
        text: root.textHint
        font: root.style.hintTextStyle.font
        color: root.style.hintTextStyle.color
        visible: !textInput.text && !textInput.activeFocus
    }
}

