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
import live 1.0

Rectangle {
    id : container
    width: 100
    height: 35
    gradient: Gradient{
        GradientStop{ position: 0.0; color: "#141923"}
        GradientStop{ position: 1.0; color: "#061119"}
    }

    property bool isLogWindowDirty     : false
    property bool isTextDirty          : false

    property var licenseSettings: settings.custom('license')

    signal messageYes()
    signal messageNo()

    signal newProject()
    signal openFile()
    signal openProject()
    signal saveFile()

    signal toggleLogWindow()

    signal openSettings()
    signal openLicense()

    function questionSave(){
        messageBox.visible = true
    }

    // New

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 160
        height : newMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        color : "#0c1924"
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
            onClicked: container.newProject()
        }
    }

    // Save

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 200
        color : "#0c1924"
        border.width: 1
        border.color: "#0f2636"
        height : saveMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : saveImage
            anchors.centerIn: parent
            source : "qrc:/images/save.png"
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

    // Open File

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 240
        color : "#0c1924"
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


    // Open Project

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 280
        color : "#0c1924"
        border.width: 1
        border.color: "#0f2636"
        height : openProjectMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : openProjectImage
            anchors.centerIn: parent
            source : "qrc:/images/open-directory.png"
        }
        Rectangle{
            color : "#0f2636"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : openProjectMArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }
        MouseArea{
            id : openProjectMArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.openProject()
        }
    }

    // Log Window

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 360
        color : "#0c1924"
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


    // Configuration

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 425
        color : "#0c1924"
        border.width: 1
        border.color: "#0f2636"
        height : openSettingsArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : openSettings
            anchors.centerIn: parent
            source : "qrc:/images/settings.png"
        }
        Rectangle{
            color : "#0f2636"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : openSettingsArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }
        MouseArea{
            id : openSettingsArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.openSettings()
        }
    }
    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 465
        color : settings.custom('license').highlights > 0 ? "#44130b" : "#0c1924"
        border.width: 1
        border.color: "#0f2636"
        height : openLicenseArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : openLicense
            anchors.centerIn: parent
            source : "qrc:/images/license.png"
        }
        Rectangle{
            color : "#0f2636"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : openLicenseArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }
        MouseArea{
            id : openLicenseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.openLicense()
        }
    }

    Rectangle{
        id : messageBox
        anchors.left: parent.left
        anchors.leftMargin: 130
        height : visible ? parent.height : 0
        color : "#102235"
        width : 400
        visible : false
        Behavior on height {
            SpringAnimation { spring: 3; damping: 0.1 }
        }
        Text{
            color : "#bec7ce"
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 12
            text : "Would you like to save your current code?"
            font.family: "Ubuntu Mono, Courier New, Courier"
        }
        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 300
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
                font.family: "Ubuntu Mono, Courier New, Courier"
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
            anchors.leftMargin: 350
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
                font.family: "Ubuntu Mono, Courier New, Courier"
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
