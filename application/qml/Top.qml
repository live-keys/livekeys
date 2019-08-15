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
    color: 'transparent'

    property alias modeImage: modeImage
    property var modeContainer: null
    property var runnablesMenu : null

    property bool isLogWindowDirty: false
    property var licenseSettings: livecv.settings.file('license')

    signal openCommandsMenu()
    signal toggleLogWindow()
    signal openSettings()
    signal openLicense()

    property string newIcon: ""
    property string saveIcon: ""
    property string openFileIcon: ""
    property string openProjectIcon: ""

    property string viewLogIcon: ""
    property string runCommandIcon: ""
    property string openSettingsIcon: ""
    property string openLicenseIcon: ""

    // New

    Connections{
        target: livecv
        onLayerReady: {
            if ( layer.name === 'workspace' ){
                var theme = layer.themes.current
                container.newIcon = theme.topNewIcon
                container.saveIcon = theme.topSaveIcon
                container.openFileIcon = theme.topOpenFileIcon
                container.openProjectIcon = theme.topOpenProjectIcon
                container.viewLogIcon = theme.topViewLogIcon
                container.runCommandIcon = theme.topRunCommandIcon
                container.openSettingsIcon = theme.topOpenSettingsIcon
                container.openLicenseIcon = theme.topOpenLicenseIcon

                container.modeContainer.liveImage.source = theme.topLiveModeIcon
                container.modeContainer.onSaveImage.source = theme.topOnSaveModeIcon
                container.modeContainer.disabledImage.source = theme.topDisabledModeIcon
            }
        }
    }

    Item{
        anchors.left: parent.left
        anchors.leftMargin: 130
        height : newMArea.containsMouse ? parent.height : parent.height - 5
        width : 25
        Image{
            id : newImage
            anchors.centerIn: parent
            source: container.newIcon
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
            onClicked: livecv.layers.workspace.project.newProject()
        }
    }

    // Save

    Item{
        anchors.left: parent.left
        anchors.leftMargin: 130 + 35
        height : saveMArea.containsMouse ? parent.height : parent.height - 5
        width : 25
        Image{
            id : saveImage
            anchors.centerIn: parent
            source : container.saveIcon
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
            onClicked: {
                var fe = livecv.layers.workspace.panes.focusPane('editor')
                if ( fe )
                    fe.saveAs()
            }
        }
    }

    // Open File

    Item{
        anchors.left: parent.left
        anchors.leftMargin: 130 + 35 * 2
        height : openMArea.containsMouse ? parent.height : parent.height - 5
        width : 25
        Image{
            id : openImage
            anchors.centerIn: parent
            source : container.openFileIcon
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
            onClicked: livecv.layers.workspace.project.openFileDialog()
        }
    }


    // Open Project

    Item{
        anchors.left: parent.left
        anchors.leftMargin: 130 + 35 * 3
        height : openProjectMArea.containsMouse ? parent.height : parent.height - 5
        width : 25
        Image{
            id : openProjectImage
            anchors.centerIn: parent
            source : container.openProjectIcon
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
            onClicked: livecv.layers.workspace.project.openProject()
        }
    }

    // Log Window

    Item{
        anchors.left: parent.left
        anchors.leftMargin: 220 + 35 * 3
        height : openLogMArea.containsMouse ? parent.height : parent.height - 5
        width : 25
        Image{
            id : openLogImage
            anchors.centerIn: parent
            source : container.viewLogIcon

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

    Item{
        anchors.left: parent.left
        anchors.leftMargin: 220 + 35 * 4
        height : commandMArea.containsMouse ? parent.height : parent.height - 5
        width : 25
        Image{
            id : commandImage
            anchors.centerIn: parent
            source : container.runCommandIcon
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

    Item{
        anchors.left: parent.left
        anchors.leftMargin: 220 + 35 * 5
        height : openSettingsArea.containsMouse ? parent.height : parent.height - 5
        width : 25
        Image{
            id : openSettings
            anchors.centerIn: parent
            source : container.openSettingsIcon
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

    // Open License

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 220 + 35 * 6
        color : livecv.settings.file('license').highlights > 0 ? "#44130b" : "transparent"
        height : openLicenseArea.containsMouse ? parent.height : parent.height - 5
        width : 25
        Image{
            id : openLicense
            anchors.centerIn: parent
            source : container.openLicenseIcon
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


    Rectangle {
        id: modeWrapper
        anchors.left: parent.left
        anchors.leftMargin: 520
        width: 220
        height: 30
        color: 'transparent'

        Item{
            anchors.left: parent.left
            anchors.leftMargin: 35
            height : parent.height
            Text{
                color :  "#969aa1"
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
                font.family: 'Open Sans, Arial, sans-serif'
                elide: Text.ElideRight
                width: 130
                text : {
                    if (!project.active)
                        return "";

                    return project.active.name
                }
            }
        }

        Triangle{
            id: compileButtonShape
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            width: compileButton.containsMouse ? 8 : 7
            height: compileButton.containsMouse ? 13 : 12
            state : "Released"
            rotation: Triangle.Right

            Behavior on height{ NumberAnimation{ duration: 100 } }
            Behavior on width{ NumberAnimation{ duration: 100 } }

            states: [
                State {
                    name: "Pressed"
                    PropertyChanges { target: compileButtonShape; color: "#487db9"}
                },
                State {
                    name: "Released"
                    PropertyChanges { target: compileButtonShape; color: compileButton.containsMouse ? "#768aca" : "#bcbdc1"}
                }
            ]
            transitions: [
                Transition {
                    from: "Pressed"
                    to: "Released"
                    ColorAnimation { target: compileButtonShape; duration: 100}
                },
                Transition {
                    from: "Released"
                    to: "Pressed"
                    ColorAnimation { target: compileButtonShape; duration: 100}
                }
            ]
        }

        MouseArea{
            id : compileButton
            anchors.left: parent.left
            anchors.leftMargin: 0
            width: 50
            height: 30
            hoverEnabled: true
            onPressed: compileButtonShape.state = "Pressed"
            onReleased: compileButtonShape.state = "Released"
            onClicked: { project.run() }
        }

        Item{
            anchors.right: parent.right
            anchors.rightMargin: 50
            width: 30
            height: parent.height

            Image{
                id: switchRunnableImage
                anchors.centerIn: parent
                source : "qrc:/images/switch-file.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: container.runnablesMenu.visible = !container.runnablesMenu.visible
            }
        }

        Item{
            width: modeImage.width + modeImage.anchors.rightMargin + 10
            height: parent.height
            anchors.right: parent.right

            Item{
                width: 25
                height: 25
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 20

                Image{
                    id : modeImage
                    anchors.centerIn: parent
                    source: modeContainer.liveImage.source
                }
            }


            Triangle{
                anchors.right: parent.right
                anchors.rightMargin: 7
                anchors.verticalCenter: parent.verticalCenter
                width: 9
                height: 5
                color: openStatesDropdown.containsMouse ? "#9b6804" : "#bcbdc1"
                rotation: Triangle.Bottom
            }

            MouseArea{
                id : openStatesDropdown
                anchors.fill: parent
                hoverEnabled: true
                onClicked: modeContainer.visible = !modeContainer.visible
            }
        }


        Rectangle{
            width: parent.width
            height: 1
            color: "#1a1f25"
            anchors.bottom: parent.bottom
        }

    }
}
