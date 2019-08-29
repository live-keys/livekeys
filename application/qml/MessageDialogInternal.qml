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
import QtQuick.Window 2.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

Rectangle{
    id: root

    property string message : ""

    signal ok()
    signal cancel()

    default property alias children: buttonContainer.children

    property var returnPressed : function(){}
    property var escapePressed : function(){}

    Keys.onReturnPressed: root.returnPressed(root)
    Keys.onEscapePressed: root.escapePressed(root)

    anchors.centerIn: parent
    color: "#fff"
    width: 400
    height: messageLabel.height < 100 ? messageLabel.height + 100 : 200
    focus: true
    opacity: root.visible ? 1 : 0

    Behavior on opacity{ NumberAnimation{ duration: 250} }

    ScrollView{
        anchors.fill: parent
        anchors.bottomMargin: 60

        style: ScrollViewStyle {
            transientScrollBars: false
            handle: Item {
                implicitWidth: 10
                implicitHeight: 10
                Rectangle {
                    color: "#222b38"
                    anchors.fill: parent
                }
            }
            scrollBarBackground: Item{
                implicitWidth: 10
                implicitHeight: 10
                Rectangle{
                    anchors.fill: parent
                    color: root.color
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Item{}
            corner: Rectangle{color: root.color}
        }

        Text{
            id: messageLabel
            anchors.top: parent.top
            anchors.topMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 12

            text: root.message

            width: root.width - 35

            wrapMode: Text.WordWrap

            color: "#ccc"
            font.pixelSize: 12
            font.family: "Open Sans, sans-serif"
            font.weight: Font.Light
        }
    }

    Item{
        id: buttonContainer
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: 15
        anchors.right: parent.right
        anchors.rightMargin: 15
        width: parent.width
        height: 30
    }

}
