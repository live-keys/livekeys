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
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.0
import base 1.0
import workspace 1.0
import editor 1.0
import editor.private 1.0
import workspace.icons 1.0 as Icons
import live 1.0
import visual.input 1.0 as Input

Item{
    id: root
    anchors.fill: parent
    objectName: "workspace"

    property Item runSpace : root.panes.__factories.viewer.runSpace

    property QtObject panes : PaneEnvironment{
        container: mainSplit
        contextMenu: contextMenu
        paneDropArea: paneDropArea
    }

    property QtObject projectEnvironment : ProjectEnvironment{
        panes: root.panes
        runSpace: root.runSpace
    }

    property QtObject startup: QtObject{
        property Component __factory: Component{
            StartupBox{}
        }

        property Item instance: null

        property var show: function(){
            if ( !root.startup.instance ){
                var startup = root.startup.__factory.createObject()
                root.startup.instance = lk.layers.window.dialogs.overlayBox(startup)
            }
        }

        property var close: function(){
            if ( root.startup.instance ){
                root.startup.instance.box.destroy()
                root.startup.instance.destroy()
                root.startup.instance = null
            }
        }
    }

    property Component messageDialogButton: Input.TextButton{
        height: 28
        width: 100
        style: lk.layers.workspace.themes.current.formButtonStyle
        onClicked: parent.clicked()
    }

    property bool documentsReloaded : false

    Connections{
        target: lk.layers.window
        function onIsActiveChanged(isActive){
            if ( isActive ){
                project.navigationModel.requiresReindex()
                project.fileModel.rescanEntries()
                project.documentModel.rescanDocuments()
                if ( documentsReloaded && project.active ){
                    project.scheduleRun()
                    documentsReloaded = false
                }

                if ( root.panes.activePane )
                    root.panes.activePane.forceActiveFocus()
            }
        }
    }

    Connections{
        target: lk

        function onLayerReady(layer){
            if ( layer.name === 'workspace' ){
                layer.commands.add(root, {
                    'toggleNavigation' : [root.toggleNavigation, "Toggle Navigation"],
//                    'openLogInWindow' : [mainVerticalSplit.openLogInWindow, "Open Log In Window"],
//                    'openLogInEditor' : [mainVerticalSplit.openLogInEditor, "Open Log In Editor"],
                    'toggleLog' : [function(){}, "Toggle Log"],
                    'toggleLogPrefix' : [root.panes.__factories.mainLog.toggleLogPrefix, "Toggle Log Prefix"],
//                    'addHorizontalEditorView' : [mainVerticalSplit.addHorizontalEditor, "Add Horizontal Editor"],
//                    'addHorizontalFragmentEditorView': [mainVerticalSplit.addHorizontalFragmentEditor, "Add Horizontal Fragment Editor"],
//                    'removeHorizontalEditorView' : [mainVerticalSplit.removeHorizontalEditor, "Remove Horizontal Editor"],
                    'setLiveCodingMode': [root.panes.__factories.viewer.modeContainer.setLiveCodingMode, "Set 'Live' Coding Mode"],
                    'setOnSaveCodingMode': [root.panes.__factories.viewer.modeContainer.setOnSaveCodingMode, "Set 'On Save' Coding Mode"],
                    'setDisabledCodingMode': [root.panes.__factories.viewer.modeContainer.setDisabledCodingMode, "Set 'Disabled' Coding Mode"],
                    'runProject': [project.run, "Run Project"],
                    'addRunView' : [root.addRunView, "Add Run View"],
                    "help" : [root.help, "Help"]
                })

                removePaneBox.style.backgroundColor = layer.themes.current.colorScheme.backgroundOverlay
                removePaneBox.style.borderColor = layer.themes.current.colorScheme.backgroundBorder
                removePaneBox.style.borderHighlightColor = layer.themes.current.colorScheme.error
                removePaneBox.style.iconColor = layer.themes.current.colorScheme.topIconColor
                removePaneBox.style.iconColorAlternate = layer.themes.current.colorScheme.topIconColorAlternate
                root.panes.style.splitterColor = layer.themes.current.paneSplitterColor
                contextMenu.style = layer.themes.current.popupMenuStyle

                var messageDialogStyle = lk.layers.window.dialogs.messageStyle
                messageDialogStyle.box.background = layer.themes.current.colorScheme.middleground
                messageDialogStyle.box.borderColor = layer.themes.current.colorScheme.middlegroundBorder
                messageDialogStyle.box.borderWidth = 1
                messageDialogStyle.box.textColor = layer.themes.current.colorScheme.foreground
                messageDialogStyle.box.font = layer.themes.current.inputLabelStyle.textStyle.font
                messageDialogStyle.button = root.messageDialogButton
            }
        }
    }

    Top{
        id : header
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        property var callback : function(){}

        property string action : ""

        property int errorContainerState: messagesContainer.messageState

        onToggleLogWindow : {
            var fe = root.panes.focusPane('log')
            if ( !fe ){
                fe = root.panes.createPane('log', {}, [200, 200])
                var containerUsed = root.panes.container
                root.panes.splitPaneVerticallyWith(containerUsed, containerUsed.panes.length - 1, fe)
            } else {
                root.panes.removePane(fe)
            }
        }

        onOpenCommandsMenu: {
            lk.layers.workspace.commands.model.setFilter('')
            commandsMenu.visible = !commandsMenu.visible
        }

        onOpenMessages: {
            messagesContainer.visible = !messagesContainer.visible
        }

        onOpenSettings: {
            var fe = lk.layers.workspace.project.openFile(
                lk.settings.file('editor').path, ProjectDocument.Edit
            )
            lk.settings.file('editor').documentOpened(fe.document)
        }

        property License license : License{}
        onOpenLicense: {lk.layers.window.dialogs.overlayBox(license)}
    }

    CommandsMenu{
        id: commandsMenu
        anchors.top: header.bottom
        x: 355
    }

    MessagesContainer{
        id: messagesContainer
        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.right: parent.right
        z: 3000
    }

    function addRunView(){
        var pane = root.panes.createPane('runView', {}, [400, 0])

        var containerUsed = root.panes.container
        if ( containerUsed.orientation === Qt.Vertical ){
            root.panes.splitPaneVerticallyWith(containerUsed, containerUsed.panes.length - 1, pane)
        } else {
            root.panes.splitPaneHorizontallyWith(containerUsed, containerUsed.panes.length - 1, pane)
        }
    }

    function help(){
        var activePane = root.panes.activePane
        if ( activePane )
            activePane.paneHelp()
    }

    Component{
        id: paneWindowFactory

        PaneWindow{
            id: paneWindow
            panes: root.panes
            createNewSplitter : function(orientation){
                var ob = paneSplitViewFactory.createObject(null)
                ob.orientation = orientation
                ob.createNewSplitter = mainSplit.createNewSplitter
                ob.currentWindow = paneWindow
                return ob
            }
        }
    }


    Component{
        id: paneSplitViewFactory

        PaneSplitView{}
    }

    function toggleNavigation(){
        var ap = root.panes.activePane
        if ( ap.objectName === 'editor' ){
            var navMenu = root.projectEnvironment.navigation
            navMenu.parent = ap
            navMenu.visible = !navMenu.visible
        }
    }

    PaneSplitView{
        id: mainSplit

        anchors.top : header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height : parent.height - header.height
        orientation: Qt.Vertical
        currentWindow : lk.layers.window.window()

        createNewSplitter: function(orientation){
            var ob = paneSplitViewFactory.createObject(null)
            ob.handleColor = root.panes.style.splitterColor
            ob.orientation = orientation
            ob.createNewSplitter = mainSplit.createNewSplitter
            ob.currentWindow = mainSplit.currentWindow
            return ob
        }
    }

    PaneDropArea{
        id: paneDropArea
        anchors.top : header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height : parent.height - header.height

        onDropped : {
            if ( data.pane === currentPane )
                return

            var clone = null
            if ( currentPane ){
                clone = currentPane
                root.panes.removePane(currentPane)
            } else {
                clone = paneFactory()
            }


            var parentSplitView = data.pane.parentSplitView
            var paneIndex = data.pane.parentSplitViewIndex()

            if ( location === paneDropArea.topPosition ){
                root.panes.splitPaneVerticallyBeforeWith(parentSplitView, paneIndex, clone)
            } else if ( location === paneDropArea.rightPosition ){
                root.panes.splitPaneHorizontallyWith(parentSplitView, paneIndex, clone)
            } else if ( location === paneDropArea.bottomPosition ){
                root.panes.splitPaneVerticallyWith(parentSplitView, paneIndex, clone)
            } else if ( location === paneDropArea.leftPosition ){
                root.panes.splitPaneHorizontallyBeforeWith(parentSplitView, paneIndex, clone)
            }
        }
    }


    Rectangle{
        id: removePaneBox
        width: 30
        height: 30
        anchors.top: parent.top
        anchors.topMargin: 3
        anchors.right: parent.right
        anchors.rightMargin: 5

        property QtObject style: QtObject{
            property color backgroundColor: '#0a1014'
            property color borderColor: '#232f37'
            property color borderHighlightColor: '#ba2020'
            property color iconColor: '#b3bdcc'
            property color iconColorAlternate: '#3f4449'
        }

        color: style.backgroundColor
        border.color: removePaneDropArea.containsDrag ? style.borderHighlightColor : style.borderColor
        border.width: 1
        radius: 2
        visible: paneDropArea.visible

        Icons.TrashIcon{
            anchors.centerIn: parent
            color: parent.style.iconColor
            alternateColor: parent.style.iconColorAlternate
            width: 15
            height: 15
        }

        DropArea{
            id: removePaneDropArea
            anchors.fill: parent
            keys: ["text/plain"]
            onDropped: {
                if ( paneDropArea.currentPane ){
                    root.panes.clearPane(paneDropArea.currentPane)
                    paneDropArea.currentPane = null
                }
                drag.accepted = true
            }
        }
    }

    ContextMenu {
        id: contextMenu
    }

}
