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

    property variant model: []

    property color backgroundColor: "#061a29"
    property color highlightColor: "#071825"
    property color textColor: "#fff"

    property int dropBoxHeight: 80

    property alias selectedItem: chosenItemText.text
    property alias selectedIndex: listView.currentIndex

    signal comboClicked

    z: 100

    Rectangle {
        id: chosenItem
        width: parent.width
        height: root.height
        color: chosenItemMouse.containsMouse ? root.highlightColor : root.backgroundColor
        Text{
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            id: chosenItemText
            text: root.model ? root.model[0] : ''
            font.family: "Ubuntu Mono, Arial, Helvetica, sans-serif"
            font.pixelSize: 13
            color: root.textColor
        }

        MouseArea{
            id: chosenItemMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                root.state = root.state === "dropDown" ? "" : "dropDown"
            }
            cursorShape: Qt.PointingHandCursor
        }
    }

    Rectangle {
        id: dropDown
        width: root.width
        height: 0
        clip: true
        anchors.top: chosenItem.bottom
        color: root.backgroundColor

        ListView {
            id: listView
            height: root.dropBoxHeight
            model: root.model
            currentIndex: 0
            delegate: Rectangle {
                width: root.width
                height: root.height
                color: delegateArea.containsMouse ? root.highlightColor : "transparent"

                Text{
                    text: modelData
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 10
                    color: root.textColor
                    font.family: "Ubuntu Mono, Arial, Helvetica, sans-serif"
                    font.pixelSize: 13
                }
                MouseArea{
                    id: delegateArea
                    anchors.fill: parent
                    onClicked: {
                        root.state = ""
                        var prevSelection = chosenItemText.text
                        chosenItemText.text = modelData
                        if (chosenItemText.text != prevSelection) {
                            root.comboClicked()
                        }
                        listView.currentIndex = index
                    }
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }

    states: State {
        name: "dropDown"
        PropertyChanges {
            target: dropDown
            height: root.height * root.model.length
        }
    }

    transitions: Transition {
        NumberAnimation {
            target: dropDown
            properties: "height"
            easing.type: Easing.OutExpo
            duration: 1000
        }
    }
}


