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
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.0
import base 1.0
import editor 1.0
import editor.private 1.0
import live 1.0

ApplicationWindow{
    id : root

    visible: true
    width: 1240
    minimumWidth: 640
    height: 700
    minimumHeight: 400
    color : "#293039"
    objectName: "window"
    title: qsTr("Live CV")

    property int prevVisibility: 2

    property QtObject controls: QtObject{

        property QtObject workspace : QtObject{

            property QtObject panes : QtObject{
                property var open: []
                property Item activePane : null
                property Item activeItem : null

                function add(pane){
                    open.push(pane)
                    if ( !activeItem )
                        activateItem(pane, pane)
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
            }

            property QtObject project : Project{
                windowControls: controls
                runSpace: runSpace
            }
        }

        property QtObject editor : QtObject{
            property EditorEnvironment environment : EditorEnvironment{
                content : contentWrap
            }
        }

        property QtObject dialogs : QtObject{

            function saveFile(options, callback){
                var title = options.title ? options.title : "Pleace choose a file";
                var filters = options.filters ? options.filters : ["All files (*)"]
                var selectExisting = false

                fileSaveDialog.title = title
                fileSaveDialog.nameFilters = filters
                fileSaveDialog.callback = callback

                fileSaveDialog.open()
            }

            function openFile(options, callback){
                var title = options.title ? options.title : "Pleace choose a file";
                var filters = options.filters ? options.filters : ["All files (*)"]
                var selectExisting = false

                fileOpenDialog.title = title
                fileOpenDialog.nameFilters = filters
                fileOpenDialog.callback = callback

                fileOpenDialog.open()
            }

            function openDir(options, callback){
                var title = options.title ? options.title : "Pleace choose a directory";

                dirOpenDialog.title = title
                dirOpenDialog.callback = callback

                dirOpenDialog.open()
            }

            function message(message, options){
                var ob = overlayBoxFactory.createObject(root)
                ob.box = messageDialogConfirmFactory.createObject()
                ob.box.show(message, options)
                return ob
            }

            function overlayBox(object){
                var ob = overlayBoxFactory.createObject(root)
                ob.box = object
                return ob
            }
        }
    }

    property bool documentsReloaded : false
    onActiveChanged: {
        if ( active ){
            project.navigationModel.requiresReindex()
            project.fileModel.rescanEntries()
            project.documentModel.rescanDocuments()
            if ( documentsReloaded && project.active ){
                root.controls.workspace.project.compileTimer.restart()
                documentsReloaded = false
            }
            editor.forceActiveFocus()
        }
    }

    function toggleFullScreen(){
        if (root.visibility !== Window.FullScreen){
            root.prevVisibility = root.visibility
            root.visibility = Window.FullScreen
        } else {
            root.visibility = root.prevVisibility
        }
    }

    function setLiveCodingMode()
    {
        livecv.windowControls().editor.environment.codingMode = 0
        modeContainer.visible = false
        header.modeImage.source = modeContainer.liveImage.source
        header.modeImage.anchors.rightMargin = modeContainer.liveImage.anchors.rightMargin
        controls.workspace.project.compile()
    }

    function setOnSaveCodingMode()
    {
        livecv.windowControls().editor.environment.codingMode = 1
        modeContainer.visible = false
        header.modeImage.source = modeContainer.onSaveImage.source
        header.modeImage.anchors.rightMargin = modeContainer.onSaveImage.anchors.rightMargin
    }

    function setDisabledCodingMode()
    {
        livecv.windowControls().editor.environment.codingMode = 2
        modeContainer.visible = false
        header.modeImage.source = modeContainer.disabledImage.source
        header.modeImage.anchors.rightMargin = modeContainer.disabledImage.anchors.rightMargin
    }

    function compileActive()
    {
        if (project.active){
            controls.workspace.project.compile()
        }
    }

    Component.onCompleted: {
        livecv.commands.add(root, {
            'minimize' : [root.showMinimized, "Minimize"],
            'toggleMaximizedRuntime' : [contentWrap.toggleMaximizedRuntime, "Toggle Maximized Runtime"],
            'toggleNavigation' : [contentWrap.toggleNavigation, "Toggle Navigation"],
            'openLogInWindow' : [mainVerticalSplit.openLogInWindow, "Open Log In Window"],
            'openLogInEditor' : [mainVerticalSplit.openLogInEditor, "Open Log In Editor"],
            'toggleLog' : [mainVerticalSplit.toggleLog, "Toggle Log"],
            'toggleLogPrefix' : [logView.toggleLogPrefix, "Toggle Log Prefix"],
            'addHorizontalEditorView' : [mainVerticalSplit.addHorizontalEditor, "Add Horizontal Editor"],
            'addHorizontalFragmentEditorView': [mainVerticalSplit.addHorizontalFragmentEditor, "Add Horizontal Fragment Editor"],
            'removeHorizontalEditorView' : [mainVerticalSplit.removeHorizontalEditor, "Remove Horizontal Editor"],
            'toggleFullScreen': [root.toggleFullScreen, "Toggle Fullscreen"],
            'setLiveCodingMode': [root.setLiveCodingMode, "Set 'Live' Coding Mode"],
            'setOnSaveCodingMode': [root.setOnSaveCodingMode, "Set 'On Save' Coding Mode"],
            'setDisabledCodingMode': [root.setDisabledCodingMode, "Set 'Disabled' Coding Mode"],
            'compileActiveFile': [root.compileActive, "Compile Active File"]
        })
    }

    FontLoader{ id: ubuntuMonoBold;       source: "qrc:/fonts/UbuntuMono-Bold.ttf"; }
    FontLoader{ id: ubuntuMonoRegular;    source: "qrc:/fonts/UbuntuMono-Regular.ttf"; }
    FontLoader{ id: ubuntuMonoItalic;     source: "qrc:/fonts/UbuntuMono-Italic.ttf"; }
    FontLoader{ id: sourceCodeProLight;   source: "qrc:/fonts/SourceCodePro-Light.ttf"; }
    FontLoader{ id: sourceCodeProRegular; source: "qrc:/fonts/SourceCodePro-Regular.ttf"; }
    FontLoader{ id: sourceCodeProBold;    source: "qrc:/fonts/SourceCodePro-Bold.ttf"; }
    FontLoader{ id: openSansLight;        source: "qrc:/fonts/OpenSans-Light.ttf"; }
    FontLoader{ id: openSansBold;         source: "qrc:/fonts/OpenSans-LightItalic.ttf"; }
    FontLoader{ id: openSansRegular;      source: "qrc:/fonts/OpenSans-Regular.ttf"; }
    FontLoader{ id: openSansLightItalic;  source: "qrc:/fonts/OpenSans-Bold.ttf"; }

    LogWindow{
        id : logWindow
        visible : false
        onVisibleChanged: if ( !visible ) logView.visible = false
        Component.onCompleted: width = root.width
    }

    Top{
        id : header
        visible: !livecv.settings.launchMode
        modeContainer: modeContainer
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: livecv.settings.launchMode ? 0 : 35
        color: "#08141d"

        property var callback : function(){}

        isTextDirty: editor.isDirty

        property string action : ""

        onNewProject: root.controls.workspace.project.newProject()
        onOpenFile : root.controls.workspace.project.openFile()
        onOpenProject: root.controls.workspace.project.openProject()
        onSaveFile : {
            var fe = root.controls.workspace.project.findFocusEditor()
            if ( fe )
                fe.saveAs()
        }
        onToggleLogWindow : mainVerticalSplit.toggleLog()
        onOpenCommandsMenu: {
            livecv.commands.model.setFilter('')
            commandsMenu.visible = !commandsMenu.visible
        }

        onOpenSettings: {
            var fe = root.controls.workspace.project.findFocusEditor()
            if ( fe ){
                fe.document = project.openFile(livecv.settings.file('editor').path, ProjectDocument.Edit);
                livecv.settings.file('editor').documentOpened(editor.document)
            }
        }

        property License license : License{}
        onOpenLicense: {root.controls.dialogs.overlayBox(license)}
    }

    CommandsMenu {
        id: commandsMenu
        anchors.top: header.bottom
        x: 395
    }

    ModeContainer {
        id: modeContainer
        modeWrapper: header
        setDisabledCodingMode: root.setDisabledCodingMode
        setLiveCodingMode: root.setLiveCodingMode
        setOnSaveCodingMode: root.setOnSaveCodingMode
    }

    FileDialog {
        id: fileOpenDialog
        title: "Please choose a file"
        nameFilters: ["All files (*)" ]
        selectExisting : true
        visible : script.environment.os.platform === 'linux' ? true : false // fixes a display bug in some linux distributions

        property var callback: null

        onAccepted: {
            fileOpenDialog.callback(fileOpenDialog.fileUrl)
            fileOpenDialog.callback = null
        }
        onRejected:{
            fileOpenDialog.callback = null
        }

        Component.onCompleted: {
            if ( script.environment.os.platform === 'darwin' )
                folder = '~' // fixes a warning message that the path was constructed with an empty filename
            visible = false
            close()
        }
    }

    FileDialog {
        id: dirOpenDialog
        title: "Please choose a directory"
        selectExisting : true
        selectMultiple : false
        selectFolder : true

        visible : script.environment.os.platform === 'linux' ? true : false /// fixes a display bug in some linux distributions

        property var callback: null

        onAccepted: {
            dirOpenDialog.callback(dirOpenDialog.fileUrl)
            dirOpenDialog.callback = null
        }
        onRejected:{
            dirOpenDialog.callback = null
        }

        Component.onCompleted: {
            if ( script.environment.os.platform === 'darwin' )
                folder = '~' // fixes a warning message that the path was constructed with an empty filename
            visible = false
            close()
        }
    }

    FileDialog{
        id : fileSaveDialog
        title: "Please choose a file"
        nameFilters: ["All files (*)"]
        selectExisting : false
        visible : script.environment.os.platform === 'linux' ? true : false /// fixes a display bug in some linux distributions

        property var callback: null

        onAccepted: {
            fileSaveDialog.callback(fileSaveDialog.fileUrl)
            fileSaveDialog.callback = null
        }
        onRejected:{
            fileSaveDialog.callback = null
        }
        Component.onCompleted: {
            if ( script.environment.os.platform === 'darwin' )
                folder = '~' // fixes a warning message that the path was constructed with an empty filename
            visible: false
            close()
        }
    }

    Component{
        id: editorFactory

        Editor{
            id: editorComponent
            height: parent ? parent.height : 0
            width: 400
            windowControls: controls
            onInternalActiveFocusChanged: if ( internalActiveFocus ) {
                root.controls.workspace.panes.setActiveItem(editorComponent.textEdit, editorComponent)
            }

            Component.onCompleted: {
                root.controls.workspace.panes.add(editorComponent)
                root.controls.workspace.panes.setActiveItem(editorComponent.textEdit, editorComponent)
                if ( project.active ){
                    editorComponent.document = project.active
                }
                forceFocus()
            }
        }
    }

    SplitView{
        id: mainVerticalSplit
        anchors.top : header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height : parent.height - header.height
        orientation: Qt.Vertical

        handleDelegate: Rectangle{
            implicitWidth: 1
            implicitHeight: 1
            color: "#191e23"
        }

        function addHorizontalEditor(){
            var editorObject = editorFactory.createObject(0)
            editorObject.height = mainHorizontalSplit.height

            var projectViewWidth = projectView.width
            var viewerWidth = viewer.width

            var editorWidths = []
            for ( var i = 0; i < mainHorizontalSplit.editors.length; ++i ){
                editorWidths.push(mainHorizontalSplit.editors[i].width)
            }

            mainHorizontalSplit.removeItem(viewer)
            mainHorizontalSplit.addItem(editorObject)
            mainHorizontalSplit.addItem(viewer)

            projectView.width = projectViewWidth
            editorObject.width = 400
            viewer.width = viewer.width - editorObject.width

            for ( var i = 0; i < editorWidths.length; ++i ){
                mainHorizontalSplit.editors[i].width = editorWidths[i]
            }
        }

        function addHorizontalFragmentEditor(){
            var fe = projectView.findFocusEditor()
            if ( !fe || !fe.document ){
                addHorizontalEditor();
                return
            }

            var cursorBlock = fe.getCursorFragment()
            if ( cursorBlock.start === cursorBlock.end ){
                addHorizontalEditor()
                return
            }

            var editorObject = editorFactory.createObject(0)
            editorObject.fragmentStart = cursorBlock.start + 1
            editorObject.fragmentEnd = cursorBlock.end + 1
            editorObject.height = mainHorizontalSplit.height

            var projectViewWidth = projectView.width
            var viewerWidth = viewer.width

            var editorWidths = []
            for ( var i = 0; i < mainHorizontalSplit.editors.length; ++i ){
                editorWidths.push(mainHorizontalSplit.editors[i].width)
            }

            mainHorizontalSplit.removeItem(viewer)
            mainHorizontalSplit.addItem(editorObject)
            mainHorizontalSplit.addItem(viewer)

            projectView.width = projectViewWidth
            editorObject.width = 400
            viewer.width = viewer.width - editorObject.width

            for ( var i = 0; i < editorWidths.length; ++i ){
                mainHorizontalSplit.editors[i].width = editorWidths[i]
            }
        }

        function removeHorizontalEditor(){
            var fe = projectView.findFocusEditor()
            if ( fe )
                mainHorizontalSplit.removeItem(fe)
        }

        function openLogInWindow(){
            var logItem = logView
            mainVerticalSplit.removeItem(logView)

            logItem.x = 0
            logItem.y = 0
            logItem.width = Qt.binding(function(){ return logWindow.contentWidth; })
            logItem.height = Qt.binding(function(){ return logWindow.contentHeight; })
            logItem.isInWindow = true

            logWindow.content = logItem
            logWindow.show()
        }

        function openLogInEditor(){
            var logItem = logWindow.content
            logWindow.content = null
            logWindow.close()

            logItem.isInWindow = false
            logItem.visible = true
            mainVerticalSplit.addItem(logItem)
            contentWrap.height = contentWrap.height - 200
        }

        function toggleLog(){
            if ( logView.isInWindow ){
                if ( logView.visible ){
                    logWindow.close()
                    logView.visible = false
                } else {
                    header.isLogWindowDirty = false
                    logView.visible = true
                    logWindow.show()
                }
            } else {
                if ( logView.visible ){
                    logView.visible = false
                    mainVerticalSplit.removeItem(logView)
                } else {
                    header.isLogWindowDirty = false
                    logView.visible = true
                    mainVerticalSplit.addItem(logView)
                    contentWrap.height = contentWrap.height - 200
                }
            }
        }

        Rectangle{
            id : contentWrap
            width: parent.width
            height: parent.height - 200

            property var palettes: []

            property var toggledItems: []
            function toggleMaximizedRuntime(){
                if ( toggledItems.length === 0 ){
                    toggledItems = [projectView.width, editor.width]
                    projectView.width = 0
                    editor.width = 0
                } else {
                    projectView.width = toggledItems[0]
                    editor.width = toggledItems[1]
                    toggledItems = []
                }
            }

            function toggleNavigation(){
                var ap = livecv.windowControls().workspace.panes.activePane
                if ( ap.objectName === 'editor' ){
                    var navMenu = livecv.windowControls().workspace.project.navigation
                    navMenu.parent = ap
                    navMenu.visible = !navMenu.visible
                }
            }

            SplitView{
                id: mainHorizontalSplit
                anchors.fill: parent
                orientation: Qt.Horizontal
                handleDelegate: Rectangle{
                    implicitWidth: 1
                    implicitHeight: 1
                    color: "#081019"
                }

                property var editors: [editor]

                ProjectFileSystem{
                    id: projectView
                    height: parent.height
                    width: 240
                    visible : !livecv.settings.launchMode
                    windowControls: controls
                    Component.onCompleted: {
                        root.controls.workspace.panes.add(projectView)
                    }
                }

                Editor{
                    id: editor
                    height: parent.height
                    width: 400
                    visible : !livecv.settings.launchMode
                    windowControls: controls
                    onInternalActiveFocusChanged: if ( internalFocus ) {
                        root.controls.workspace.panes.setActiveItem(editor.textEdit, editor)
                    }

                    Component.onCompleted: {
                        root.controls.workspace.panes.add(editor)
                        root.controls.workspace.panes.setActiveItem(editor.textEdit, editor)
                        if ( project.active ){
                            editor.document = project.active
                        }
                        forceFocus()
                    }
                }

                Rectangle{
                    id : viewer
                    height : parent.height
                    objectName: "viewer"

                    color : "#000509"

                    Item{
                        id: runSpace
                        anchors.fill: parent
                        property variant item

                        Connections{
                            target: livecv.engine
                            onAboutToCreateObject : {
                                if (staticContainer)
                                    staticContainer.beforeCompile()
                            }
                            onObjectCreated : {
                                error.text = ''
                                if (runSpace.item) {
                                    runSpace.item.destroy();
                                }
                                runSpace.item = object;
                                if ( staticContainer )
                                    staticContainer.afterCompile()
                            }
                            onObjectCreationError : {
                                var errorMessage = error.wrapMessage(errors)
                                error.text = errorMessage.rich
                                console.error(errorMessage.log)
                            }
                        }
                    }

                    ErrorContainer{
                        id: error
                        anchors.bottom: parent.bottom
                        width : parent.width
                        color : editor.color
                        font.pointSize: editor.font.pointSize
                    }

                }
            }
        }
    }

    property LogContainer logView : LogContainer{
        id: logView
        visible: false
        isInWindow: false
        width: parent ? parent.width : 0
        height: 200

        onItemAdded: {
            if ( !visible  )
                header.isLogWindowDirty = true
        }
    }

    Component{
        id: messageDialogConfirmFactory

        MessageDialogConfirm{
            anchors.fill: parent
            color: "#050e16"
        }
    }

    Component{
        id: overlayBoxFactory

        OverlayBox{
            anchors.fill: parent
        }
    }

    // Logo

    Image{
        id: logo

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 20
        visible: true

        opacity: livecv.settings.launchMode ? 0.3 : 1.0
        source : "qrc:/images/logo.png"
    }

}
