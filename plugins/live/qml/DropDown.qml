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

Item{
    id: root

    property variant model: []

    property color backgroundColor: "#1b242c"
    property color highlightColor: "#071825"
    property color textColor: "#c7c7c7"

    property int dropBoxHeight: 80

    property alias selectedItem: chosenItemText.text
    property alias selectedIndex: listView.currentIndex

    signal comboClicked

    z: 100

    Rectangle {
        id: chosenItem
        width: parent.width
        height: root.height
        radius: height/2
        color: chosenItemMouse.containsMouse ? root.highlightColor : root.backgroundColor
        Text{
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            id: chosenItemText
            text: root.model ? root.model[0] : ''
            font.family: "Open Sans, sans-serif"
            font.weight: Font.Light
            font.pixelSize: 12
            color: root.textColor
        }

        Text{
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            id: dropDownGlyph
            text: 'v'
            font.family: "Open Sans"
            font.pixelSize: 12
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

        z: 1000
    }

    Rectangle {
        id: dropDown
        width: root.width
        height: 0
        clip: true
        anchors.top: chosenItem.top
        anchors.topMargin:chosenItem.height/2
        color: root.backgroundColor

        Rectangle {
            id: flattenRect
            width: parent.width
            height:chosenItem.height/2
            anchors.top: parent.top
            color:parent.color
        }

        ListView {
            id: listView
            anchors.top: flattenRect.bottom
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
                    font.family: "Open Sans, sans-serif"
                    font.weight: Font.Light
                    font.pixelSize: 12
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
            height: root.height * root.model.length + chosenItem.height/2
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


