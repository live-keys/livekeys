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
import QtQuick.Controls 1.4
import editor.private 1.0
import base 1.0
import live 1.0

Rectangle {
    id : container
    width: 100
    height: 35

    property color topColor: "#020a11"
    property color bottomColor: "#000509"

    gradient: Gradient{
        GradientStop{ position: 0.0; color: topColor}
        GradientStop{ position: 1.0; color: bottomColor}
    }

    property bool isLogWindowDirty     : false
    property bool isTextDirty          : false

    property var licenseSettings: livecv.settings.file('license')

    signal messageYes()
    signal messageNo()

    signal newProject()
    signal openFile()
    signal openProject()
    signal saveFile()
    signal openCommandsMenu()

    signal toggleLogWindow()

    signal openSettings()
    signal openLicense()

    function questionSave(){
        messageBox.visible = true
    }

    // New

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 130
        height : newMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        color : "transparent"
//        border.width: 1
//        border.color: "#031626"
        Image{
            id : newImage
            anchors.centerIn: parent
            source : "qrc:/images/new.png"
        }
        Rectangle{
            color : "#031626"
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
        anchors.leftMargin: 170
        color : "transparent"
//        border.width: 1
//        border.color: "#031626"
        height : saveMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : saveImage
            anchors.centerIn: parent
            source : "qrc:/images/save.png"
        }
        Rectangle{
            color : "#031626"
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
        anchors.leftMargin: 210
        color : "transparent"
//        border.width: 1
//        border.color: "#031626"
        height : openMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : openImage
            anchors.centerIn: parent
            source : "qrc:/images/open.png"
        }
        Rectangle{
            color : "#031626"
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
        anchors.leftMargin: 250
        color : "transparent"
//        border.width: 1
//        border.color: "#031626"
        height : openProjectMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : openProjectImage
            anchors.centerIn: parent
            source : "qrc:/images/open-directory.png"
        }
        Rectangle{
            color : "#031626"
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
        anchors.leftMargin: 330
        color : "transparent"
//        border.width: 1
//        border.color: "#031626"
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
            color : "#031626"
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

    // Commands

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 363
        color : "transparent"
        height : commandMArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : commandImage
            anchors.centerIn: parent
            source : "qrc:/images/command.png"
        }
        Rectangle{
            color : "#031626"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : commandMArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }
        MouseArea{
            id : commandMArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.openCommandsMenu()
        }
    }

    // Configuration

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 395
        color : "transparent"
//        border.width: 1
//        border.color: "#031626"
        height : openSettingsArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : openSettings
            anchors.centerIn: parent
            source : "qrc:/images/settings.png"
        }
        Rectangle{
            color : "#031626"
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
        anchors.leftMargin: 435
        color : livecv.settings.file('license').highlights > 0 ? "#44130b" : "transparent"
//        border.width: 1
//        border.color: "#031626"
        height : openLicenseArea.containsMouse ? parent.height : parent.height - 5
        width : 35
        Image{
            id : openLicense
            anchors.centerIn: parent
            source : "qrc:/images/license.png"
        }
        Rectangle{
            color : "#031626"
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

    // Compile button

    Button {
        id: compileButton
        anchors.left: parent.left
        anchors.leftMargin: 500
        // color : "transparent"
        text: "Compile"
        onClicked: {
            console.log(project.active.content)
            if (project.active)
                livecv.engine.createObjectAsync(
                    project.active.content,
                    livecv.windowControls().runSpace,
                    project.active.file.pathUrl(),
                    project.active,
                    true
                );
        }
    }

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 650
        height : parent.height
        color : "transparent"
        Text{
            color :  "#bec7ce"
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 5
            font.pixelSize: 12
            text : project.active && project.active.file ? project.active.file.name : ""
            font.family: 'Open Sans, Arial, sans-serif'
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
