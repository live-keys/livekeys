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
import QtQuick.Controls 1.4
import editor.private 1.0
import base 1.0
import live 1.0
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons

Rectangle {
    id : container
    width: 100
    height: 35
    color: 'transparent'

    property bool isLogWindowDirty: false

    property var licenseSettings: lk.settings.file('license')

    signal openCommandsMenu()
    signal toggleLogWindow()
    signal openSettings()
    signal openLicense()
    signal openMessages()

    property string newIcon: ""
    property string saveIcon: ""
    property string openFileIcon: ""
    property string openProjectIcon: ""

    property string viewLogIcon: ""
    property string runCommandIcon: ""
    property string openSettingsIcon: ""
    property string openLicenseIcon: ""

    property int errorContainerState: 0

    // New

    property QtObject theme : null

    Connections{
        target: lk
        onLayerReady: {
            if (layer && layer.name === 'workspace' ){
                var theme = layer.themes.current
                container.theme = theme
                container.newIcon = theme.topNewIcon
                container.saveIcon = theme.topSaveIcon
                container.openFileIcon = theme.topOpenFileIcon
                container.openProjectIcon = theme.topOpenProjectIcon
                container.viewLogIcon = theme.topViewLogIcon
                container.runCommandIcon = theme.topRunCommandIcon
                container.openSettingsIcon = theme.topOpenSettingsIcon
                container.openLicenseIcon = theme.topOpenLicenseIcon
            }
        }
    }

    MouseArea{
        id: logoFunction
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        width: 30
        height: parent.height
        hoverEnabled: true
        onClicked: {
            lk.layers.workspace.panes.initializeStartupBox()
        }
        Workspace.Tooltip{
            mouseOver: logoFunction.containsMouse
            text: "Livekeys: Startup screen"
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
            onClicked: lk.layers.workspace.project.newProject()
        }
        Workspace.Tooltip{
            mouseOver: newMArea.containsMouse
            text: "New Project"
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
                var fe = lk.layers.workspace.panes.focusPane('editor')
                if ( fe )
                    fe.saveAs()
            }
        }
        Workspace.Tooltip{
            mouseOver: saveMArea.containsMouse
            text: "Save file as"
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
            onClicked: lk.layers.workspace.project.openFileDialog()
        }
        Workspace.Tooltip{
            mouseOver: openMArea.containsMouse
            text: "Open file"
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
            onClicked: lk.layers.workspace.project.openProject()
        }
        Workspace.Tooltip{
            mouseOver: openProjectMArea.containsMouse
            text: "Open project"
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
        Workspace.Tooltip{
            mouseOver: openLogMArea.containsMouse
            text: "Toggle log pane"
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
        Workspace.Tooltip{
            mouseOver: commandMArea.containsMouse
            text: "View commands"
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
        Workspace.Tooltip{
            mouseOver: openSettingsArea.containsMouse
            text: "Configuration"
        }
    }

    // Open License

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 220 + 35 * 6
        color : lk.settings.file('license').highlights > 0 ? "#44130b" : "transparent"
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
        Workspace.Tooltip{
            mouseOver: openLicenseArea.containsMouse
            text: "Package licenses"
        }
    }

    // Messages

    Rectangle{
        anchors.right: parent.right
        anchors.rightMargin: 10
        color: "transparent"
        height : messagesArea.containsMouse ? parent.height : parent.height - 5
        width : 25
        Icons.WarningIcon{
            id: warningIcon
            anchors.centerIn: parent
            width: 24
            height: 22

            color: {
                if ( !container.theme )
                    return color
                if ( container.errorContainerState === 1 )
                    return container.theme.colorScheme.warning
                if ( container.errorContainerState === 2 )
                    return container.theme.colorScheme.error
                return container.theme.colorScheme.topIconColor
            }

            colorAlternate: {
                if ( !container.theme )
                    return colorAlternate
                if ( container.errorContainerState === 1 )
                    return container.theme.colorScheme.warning
                if ( container.errorContainerState === 2 )
                    return container.theme.colorScheme.error
                return container.theme.colorScheme.topIconColorAlternate
            }
        }
        Rectangle{
            color : "#031626"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : messagesArea.containsMouse
        }
        Behavior on height{ NumberAnimation{  duration: 100 } }
        MouseArea{
            id : messagesArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: container.openMessages()
        }
        Workspace.Tooltip{
            x: -70
            mouseOver: messagesArea.containsMouse
            text: "Show messages"
        }
    }
}
