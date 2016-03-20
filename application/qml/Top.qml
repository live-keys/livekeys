/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

import QtQuick 2.0

Rectangle {
    id : container
    width: 100
    height: 35
    gradient: Gradient{
        GradientStop{ position: 0.0; color: "#141923"}
        GradientStop{ position: 1.0; color: "#05111b"}
    }

    property bool isLogWindowDirty : false
    property bool isTextDirty      : false

    signal messageYes()
    signal messageNo()

    signal newFile()
    signal openFile()
    signal saveFile()

    signal toggleLogWindow()

    signal fontPlus()
    signal fontMinus()

    function questionSave(){
        messageBox.visible = true
    }

    // Logo

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 14
        height : parent.height
        Image{
            anchors.verticalCenter: parent.verticalCenter
            source : "qrc:/images/logo.png"
        }
    }

    // New

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 160
        height : newMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        color : "#091f2e"
        border.width: 1
        border.color: "#0f2636"
        Image{
            id : newImage
            anchors.centerIn: parent
            source : "qrc:/images/new.png"
        }
        Rectangle{
            color : "#0f2636"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : newMArea.containsMouse
        }
        Behavior on height{ NumberAnimation{ duration: 100 } }
        MouseArea{
            id : newMArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.newFile()
        }
    }

    // Save

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 200
        color : "#091f2e"
        border.width: 1
        border.color: "#0f2636"
        height : saveMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : saveImage
            anchors.centerIn: parent
            source : "qrc:/images/save.png"

            Rectangle{
                visible : container.isTextDirty
                anchors.bottom : parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: 4
                anchors.bottomMargin: 2
                width : 5
                height : 5
                color : "#aabbcc"
                radius : width * 0.5
            }
        }
        Rectangle{
            color : "#0f2636"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : saveMArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }
        MouseArea{
            id : saveMArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.saveFile();
        }
    }

    // Open

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 240
        color : "#091f2e"
        border.width: 1
        border.color: "#0f2636"
        height : openMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : openImage
            anchors.centerIn: parent
            source : "qrc:/images/open.png"
        }
        Rectangle{
            color : "#0f2636"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : openMArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }
        MouseArea{
            id : openMArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.openFile()
        }
    }

    // Log Window

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 320
        color : "#091f2e"
        border.width: 1
        border.color: "#0f2636"
        height : openLogMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : openLog
            anchors.centerIn: parent
            source : "qrc:/images/log.png"

            Rectangle{
                visible : container.isLogWindowDirty
                anchors.bottom : parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: 1
                anchors.bottomMargin: 4
                width : 5
                height : 5
                color : "#667799"
                radius : width * 0.5
            }
        }
        Rectangle{
            color : "#0f2636"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : openLogMArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }

        MouseArea{
            id : openLogMArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.toggleLogWindow()
        }
    }


    // Font Size

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 385
        color : "#091f2e"
        border.width: 1
        border.color: "#0f2636"
        height : minusMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Text{
            anchors.centerIn: parent
            text : "-"
            color : "#eee"
            font.pixelSize: 24
            font.family: "Arial"
        }
        Rectangle{
            color : "#0f2636"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : minusMArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }
        MouseArea{
            id : minusMArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.fontMinus()
        }
    }
    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 425
        color : "#091f2e"
        border.width: 1
        border.color: "#0f2636"
        height : plusMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Text{
            anchors.centerIn: parent
            text : "+"
            color : "#eee"
            font.pixelSize: 24
            font.family: "Arial"
        }
        Rectangle{
            color : "#0f2636"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : plusMArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }
        MouseArea{
            id : plusMArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.fontPlus()
        }
    }

    Rectangle{
        id : messageBox
        anchors.left: parent.left
        anchors.leftMargin: 165
        height : parent.height
        color : "#242a34"
        width : 350
        visible : false
        Text{
            color : "#bec7ce"
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 12
            text : "Would you like to save your current code?"
        }
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 250
            height : parent.height
            width : 50
            color : "transparent"
            Text{
                color : yesMArea.containsMouse ? "#fff" : "#bec7ce"
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
                text : "Yes"
            }
            MouseArea{
                id : yesMArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked : {
                    messageBox.visible = false
                    container.messageYes()
                }
            }
        }
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 300
            height : parent.height
            width : 50
            color : "transparent"
            Text{
                color : noMArea.containsMouse ? "#fff" : "#bec7ce"
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
                text : "No"
            }
            MouseArea{
                id : noMArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked : {
                    container.messageNo()
                    messageBox.visible = false
                }
            }
        }
    }

}
