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
import QtQuick.Window 2.1
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.2
import visual.input 1.0 as Input

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
    color: style.background
    border.width: style.borderWidth
    border.color: style.borderColor
    radius: style.radius

    width: 400
    height: messageLabel.height < 100 ? messageLabel.height + 100 : 200
    focus: true
    opacity: root.visible ? 1 : 0

    property QtObject style: QtObject{
        property color background: "#02070b"
        property color borderColor: "#0c151c"
        property int borderWidth: 1
        property int radius: 5
        property color textColor: "#fff"
        property font font : Qt.font({
            family: 'Open Sans, sans-serif',
            weight: Font.Normal,
            italic: false,
            pixelSize: 12
        })
    }

    Behavior on opacity{ NumberAnimation{ duration: 250} }

    ScrollView{
        id: scrollView
        anchors.fill: parent
        anchors.bottomMargin: 60

        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.contentItem: Input.ScrollbarHandle{
            color: "#1f2227"
            visible: scrollView.contentHeight > scrollView.height
        }
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.horizontal.contentItem: Input.ScrollbarHandle{
            color: "#1f2227"
            visible: scrollView.contentWidth > scrollView.width
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

            color: root.style.textColor
            font: root.style.font
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
