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
import editor 1.0
import editor.private 1.0
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons
import live 1.0

Item{
    id: root
    anchors.fill: parent
    objectName: "workspace"

    property Item runSpace : viewer.runSpace

    property QtObject style : QtObject{
        property font errorFont : Qt.font({
            family: "Source Code Pro, Ubuntu Mono, Courier New, Courier",
            pixelSize: 12,
            weight: Font.Normal
        })
        property color errorFontColor : "#fff"
        property color errorBackgroundColor: "#000"
    }

    property QtObject panes : QtObject{
        property Item activePane : null
        property Item activeItem : null
        property Item container : mainSplit
        property ContextMenu contextMenu: contextMenu

        property var openWindows : []

        property var factories : {
            return {
                "editor" : function(p, s){
                    var pane = editorFactory.createObject(p)
                    if ( s )
                        pane.paneInitialize(s)
                    return pane
                },
                "projectFileSystem" : function(p, s){
                    if ( s )
                        root.projectFileSystemSingle.paneInitialize(s)
                    return root.projectFileSystemSingle
                },
                "viewer" : function(p, s){
                    return root.viewer
                },
                "runView" : function(p, s){
                    var pane = runViewFactory.createObject(p)
                    if ( s )
                        pane.paneInitialize(s)
                    return pane
                },
                "documentation" : function(p, s){
                    var pane = documentationFactory.createObject(p)
                    if ( s )
                        pane.paneInitialize(s)
                    return pane
                },
                "objectPalette" : function(p, s){
                    var pane = objectPaletteFactory.createObject(p)
                    if ( s )
                        pane.paneInitialize(s)
                    return pane
                },
                "log" : function(p, s){
                    if ( !root.logView.parent ){
                        root.logView.visible = true
                        root.logView.parent = p
                        header.isLogWindowDirty = false
                        return root.logView
                    }

                    var pane = logFactory.createObject(p)
                    return pane
                }
            }
        }

        function __dragStarted(pane){
            for ( var i = 0; i < openWindows.length; ++i ){
                var w = openWindows[i]
                w.paneDropArea.currentPane = pane
                w.paneDropArea.model = w.mainSplit.createPositioningModel()
                w.paneDropArea.visible = true
            }

            paneDropArea.currentPane = pane
            paneDropArea.model = mainSplit.createPositioningModel()
            paneDropArea.visible = true
        }

        function __dragFinished(pane){
            paneDropArea.currentPane = null
            paneDropArea.model = []
            for ( var i = 0; i < openWindows.length; ++i ){
                var w = openWindows[i]
                w.paneDropArea.currentPane = null
                w.paneDropArea.model = []
                w.paneDropArea.visible = false
            }
            paneDropArea.visible = false
        }

        function initializeSplitterPanes(splitter, paneConfiguration){
            var orientation = paneConfiguration[0] === 'h' ? Qt.Horizontal : Qt.Vertical
            splitter.orientation = orientation

            if ( splitter !== mainSplit ){
                if ( orientation === Qt.Vertical && paneConfiguration.length > 1 ){
                    splitter.width = paneConfiguration[1].width
                } else {
                    splitter.height = paneConfiguration[1].width
                }
            }

            var panesToOpen = []
            var recurseSplitters = []

            for ( var i = 1; i < paneConfiguration.length; ++i ){

                if ( Array.isArray(paneConfiguration[i]) ){
                    var split = splitter.createNewSplitter(Qt.Vertical)
                    panesToOpen.push(split)
                    recurseSplitters.push(i)
                } else {
                    var state = 'state' in paneConfiguration[i] ? paneConfiguration[i].state : null
                    var pane = createPane(paneConfiguration[i].type, state, paneConfiguration[i].size)
                    panesToOpen.push(pane)
                }
            }

            splitter.initialize(panesToOpen)

            for ( var i = 0; i < recurseSplitters.length; ++i ){
                var configIndex = recurseSplitters[i]
                initializeSplitterPanes(panesToOpen[configIndex - 1], paneConfiguration[configIndex])
            }

            return panesToOpen
        }

        property var initializePanes : function(windowConfiguration, paneConfiguration){
            removeStartupBox()

            var currentWindowPanes = paneConfiguration[0]

            var initializeStructure = [
                [lk.layers.window.window()],
                [initializeSplitterPanes(container, currentWindowPanes)]
            ]

            for ( var i = 1; i < paneConfiguration.length; ++i ){
                var window = paneWindowFactory.createObject(root)
                openWindows.push(window)
                initializeStructure[0].push(window)
                initializeStructure[1].push(initializeSplitterPanes(window.mainSplit, paneConfiguration[i]))

                window.closing.connect(function(){
                    var index = openWindows.indexOf(window);
                    if (index > -1) {
                        openWindows.splice(index, 1);
                    }
                })
            }

            return initializeStructure
        }

        property var createPane : function(paneType, paneState, paneSize){

            if ( paneType in root.panes.factories ){
                var paneFactory = root.panes.factories[paneType]
                var paneObject = paneFactory(null, paneState)
                if ( paneSize ){
                    paneObject.width = paneSize[0]
                    paneObject.height = paneSize[1]
                }

                return paneObject

            } else {
                throw new Error('Key not found: ' + paneType)
            }
        }

        property var movePaneToNewWindow : function(pane){
            removePane(pane)

            var window = paneWindowFactory.createObject(root)
            openWindows.push(window)

            window.closing.connect(function(){
                var index = openWindows.indexOf(window);
                if (index > -1) {
                    openWindows.splice(index, 1);
                }
            })

            lk.layers.workspace.addWindow(window)

            splitPaneVerticallyWith(window.mainSplit, 0, pane)
        }

        property var splitPaneHorizontallyWith : function(splitter, index, pane){
            splitter.splitHorizontally(index, pane)

            lk.layers.workspace.addPane(pane, pane.paneWindow(), pane.splitterHierarchyPositioning())
        }

        property var splitPaneVerticallyWith : function(splitter, index, pane){
            splitter.splitVertically(index, pane)

            lk.layers.workspace.addPane(pane, pane.paneWindow(), pane.splitterHierarchyPositioning())
        }

        property var splitPaneHorizontallyBeforeWith : function(splitter, index, pane){
            splitter.splitHorizontallyBefore(index, pane)

            lk.layers.workspace.addPane(pane, pane.paneWindow(), pane.splitterHierarchyPositioning())
        }

        property var splitPaneVerticallyBeforeWith : function(splitter, index, pane){
            splitter.splitVerticallyBefore(index, pane)

            lk.layers.workspace.addPane(pane, pane.paneWindow(), pane.splitterHierarchyPositioning())
        }

        property var clearPane : function(pane){
            removePane(pane)
            pane.paneCleared()
        }

        property var removePane : function(pane){
            if ( pane.parentSplitter ){
                var split = pane.parentSplitter
                var paneIndex = split.paneIndex(pane)

                if ( pane.paneType !== 'splitview' )
                    lk.layers.workspace.removePane(pane)

                split.removeAt(paneIndex)

                if ( panes.activePane === pane ){
                    panes.activePane = null
                    panes.activeItem = null
                }

                if ( split.panes.length === 0 && split !== mainSplit ){
                    removePane(split)
                }
            } else if ( pane.paneType === 'splitview' ){
                if ( pane.currentWindow !== lk.layers.window.window() ){
                    pane.currentWindow.close()
                }
            }
        }

        property var __clearPanes : function(){
            mainSplit.clearPanes()
            activePane = null
            activeItem = null
        }

        property var removeStartupBox : function(){
            if ( root.startupBox ){
                root.startupBox.box.destroy()
                root.startupBox.destroy()
                root.startupBox = null
            }
        }

        property var initializeStartupBox : function(){
            if ( !root.startupBox ){
                var startup = startupBoxFactory.createObject()
                root.startupBox = lk.layers.window.dialogs.overlayBox(startup)
            }
        }

        function setActiveItem(item, pane){
            activeItem = item
            var p = pane ? pane : item
            while ( p !== null ){
                if ( p.objectName === 'editor' || p.objectName === 'project' || p.objectName === 'viewer' ){
                    activePane = p
                    return
                }
                p = p.parent
            }
        }

        function activateItem(item, pane){
            if ( activeItem && activeItem !== item ){
                activeItem.focus = false
            }

            activeItem = item
            activeItem.forceActiveFocus()
            var p = pane ? pane : item
            while ( p !== null ){
                if ( p.objectName === 'editor' || p.objectName === 'project' || p.objectName === 'viewer' ){
                    activePane = p
                    return
                }
                p = p.parent
            }
        }

        function focusPane(paneType){
            var ap = root.panes.activePane
            if ( ap && ap.paneType === paneType ){
                return ap
            }

            var pane = mainSplit.findPaneByType(paneType)
            if ( pane )
                return pane

            for ( var i = 0; i < root.panes.openWindows.length; ++i ){
                var w = root.panes.openWindows[i]
                var p = w.mainSplit.findPaneByType(paneType)
                if ( p )
                    return p
            }
            return null
        }

        function openContextMenu(item, pane){
            if ( !item )
                item = activeItem
            if ( !pane )
                pane = activePane

            var res = lk.layers.workspace.interceptMenu(pane, item)
            contextMenu.show(res)
        }
    }

    property QtObject projectEnvironment : ProjectEnvironment{
        panes: root.panes
        runSpace: root.runSpace
    }

    property color paneSplitterColor: "transparent"

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
//                    'toggleLog' : [mainVerticalSplit.toggleLog, "Toggle Log"],
                    'toggleLogPrefix' : [logView.toggleLogPrefix, "Toggle Log Prefix"],
//                    'addHorizontalEditorView' : [mainVerticalSplit.addHorizontalEditor, "Add Horizontal Editor"],
//                    'addHorizontalFragmentEditorView': [mainVerticalSplit.addHorizontalFragmentEditor, "Add Horizontal Fragment Editor"],
//                    'removeHorizontalEditorView' : [mainVerticalSplit.removeHorizontalEditor, "Remove Horizontal Editor"],
                    'setLiveCodingMode': [viewer.modeContainer.setLiveCodingMode, "Set 'Live' Coding Mode"],
                    'setOnSaveCodingMode': [viewer.modeContainer.setOnSaveCodingMode, "Set 'On Save' Coding Mode"],
                    'setDisabledCodingMode': [viewer.modeContainer.setDisabledCodingMode, "Set 'Disabled' Coding Mode"],
                    'runProject': [project.run, "Run Project"],
                    'addRunView' : [root.addRunView, "Add Run View"],
                    "help" : [root.help, "Help"]
                })

                removePaneBox.style.backgroundColor = layer.themes.current.colorScheme.backgroundOverlay
                removePaneBox.style.borderColor = layer.themes.current.colorScheme.backgroundBorder
                removePaneBox.style.borderHighlightColor = layer.themes.current.colorScheme.error
                removePaneBox.style.iconColor = layer.themes.current.colorScheme.topIconColor
                removePaneBox.style.iconColorAlternate = layer.themes.current.colorScheme.topIconColorAlternate
                paneSplitterColor = layer.themes.current.paneSplitterColor
                contextMenu.style = layer.themes.current.popupMenuStyle
            }
        }
    }

    LogWindow{
        id : logWindow
        visible : false
        onVisibleChanged: if (!visible){
            logView.visible = false
        }
        Component.onCompleted: width = root.width
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

    Component{
        id: editorFactory

        EditorPane{
            id: editorComponent
            height: parent ? parent.height : 0
            width: 400
            panes: root.panes
            onInternalActiveFocusChanged: if ( internalActiveFocus ) {
                root.panes.setActiveItem(editorComponent.textEdit, editorComponent)
            }
            Component.onCompleted: {
                editorComponent.editor.forceFocus()
            }
        }
    }

    Component{
        id: runViewFactory

        RunView{
            id: runViewComponent
            panes: root.panes
        }
    }

    Component{
        id: documentationFactory

        DocumentationViewPane{
            id: documentationViewComponent
            panes: root.panes
        }
    }

    Component{
        id: objectPaletteFactory

        ObjectPalettePane{
            id: objacetPaletteComponent
            panes: root.panes
        }
    }

    Component{
        id: startupBoxFactory

        StartupBox{
        }
    }

    property Item startupBox : null

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
        id: logFactory

        LogContainer{
            id: logView
            isInWindow: false
            width: 300
            height: 200
        }
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

    property ProjectFileSystem projectFileSystemSingle : ProjectFileSystem{
        id: projectView
        width: 300
        panes: root.panes
    }

    property Item viewer : Viewer {
        id: viewer
        panes: root.panes
    }

    Component{
        id: projectFileSystemFactory

        ProjectFileSystem{
            id: projectView
            width: 240
            panes: root.panes
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
            ob.handleColor = root.paneSplitterColor
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

            var clone = currentPane
            root.panes.removePane(currentPane)

            var parentSplitter = data.pane.parentSplitter
            var paneIndex = data.pane.parentSplitterIndex()

            if ( location === paneDropArea.topPosition ){
                root.panes.splitPaneVerticallyBeforeWith(parentSplitter, paneIndex, clone)
            } else if ( location === paneDropArea.rightPosition ){
                root.panes.splitPaneHorizontallyWith(parentSplitter, paneIndex, clone)
            } else if ( location === paneDropArea.bottomPosition ){
                root.panes.splitPaneVerticallyWith(parentSplitter, paneIndex, clone)
            } else if ( location === paneDropArea.leftPosition ){
                root.panes.splitPaneHorizontallyBeforeWith(parentSplitter, paneIndex, clone)
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

    property LogContainer logView : LogContainer{
        visible: false
        isInWindow: false
        width: 300
        height: 200

        onItemAdded: {
            if ( !parent  )
                header.isLogWindowDirty = true
        }
    }

    ContextMenu {
        id: contextMenu
    }

}
