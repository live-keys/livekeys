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
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
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

    property WindowControls controls: WindowControls{
        saveFileDialog: fileSaveDialog
        openFileDialog: fileOpenDialog
        openDirDialog: dirOpenDialog
        messageDialog: messageBox
        runSpace: runSpace
        createTimer: createTimer
        paletteBox: paletteBox
    }

    property bool documentsReloaded : false
    onActiveChanged: {
        if ( active ){
            project.navigationModel.requiresReindex()
            project.fileModel.rescanEntries()
            project.documentModel.rescanDocuments()
            if ( documentsReloaded && project.active ){
                createTimer.restart()
                documentsReloaded = false
            }
            editor.forceActiveFocus()
        }
    }

    Component.onCompleted: {
        livecv.commands.add(root, {
            'minimize' : root.showMinimized,
            'toggleMaximizedRuntime' : contentWrap.toggleMaximizedRuntime,
            'toggleNavigation' : contentWrap.toggleNavigation,
            'openLogInWindow' : mainVerticalSplit.openLogInWindow,
            'openLogInEditor' : mainVerticalSplit.openLogInEditor,
            'toggleLog' : mainVerticalSplit.toggleLog,
            'toggleLogPrefix' : logView.toggleLogPrefix,
            'addHorizontalEditorView' : mainVerticalSplit.addHorizontalEditor,
            'addHorizontalFragmentEditorView': mainVerticalSplit.addHorizontalFragmentEditor,
            'removeHorizontalEditorView' : mainVerticalSplit.removeHorizontalEditor
        })
    }

    title: qsTr("Live CV")

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
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: livecv.settings.launchMode ? 0 : 35
        color: "#08141d"

        property var callback : function(){}

        isTextDirty: editor.isDirty

        property string action : ""

        onNewProject: projectView.newProject()
        onOpenFile : projectView.openFile()
        onOpenProject: projectView.openProject()
        onSaveFile : projectView.focusEditor.saveAs()
        onToggleLogWindow : mainVerticalSplit.toggleLog()

        onOpenSettings: {
            editor.document = project.openFile(livecv.settings.file('editor').path, ProjectDocument.Edit);
            livecv.settings.file('editor').documentOpened(editor.document)
        }

        onOpenLicense: licenseBox.visible = true
    }

    FileDialog {
        id: fileOpenDialog
        title: "Please choose a file"
        nameFilters: [ "Qml files (*.qml)", "All files (*)" ]
        selectExisting : true
        visible : script.environment.os.platform === 'linux' ? true : false // fixes a display bug in some linux distributions
        onAccepted: {
            if ( project.rootPath === '' ){
                project.openProject(fileOpenDialog.fileUrl)
                if ( project.active )
                    projectView.focusEditor.document = project.active
            } else if ( project.isFileInProject(fileOpenDialog.fileUrl ) ) {
                var doc = project.openFile(fileOpenDialog.fileUrl, ProjectDocument.Edit)
                if ( doc )
                    projectView.focusEditor.document = doc
            } else {
                var fileUrl = fileOpenDialog.fileUrl
                messageBox.show(
                    'File is outside project scope. Would you like to open it as a new project?',
                {
                    button1Name : 'Open as project',
                    button1Function : function(){
                        var projectUrl = fileUrl
                        projectView.closeProject(
                            function(){
                                project.openProject(projectUrl)
                                projectView.focusEditor.document = project.active
                            }
                        )
                        messageBox.close()
                    },
                    button3Name : 'Cancel',
                    button3Function : function(){
                        messageBox.close()
                    },
                    returnPressed : function(){
                        var projectUrl = fileUrl
                        header.closeProject(
                            function(){
                                project.openProject(projectUrl)
                                projectView.focusEditor.document = project.active
                            }
                        )
                        messageBox.close()
                    }
                })
            }
        }
        Component.onCompleted: {
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
        onAccepted: {
            project.openProject(dirOpenDialog.fileUrl)
            if ( project.active )
                projectView.focusEditor.document = project.active
        }
        Component.onCompleted: {
            visible = false
            close()
        }
    }

    FileDialog{
        id : fileSaveDialog
        title: "Please choose a file"
        nameFilters: ["Qml files (*.qml)", "All files (*)"]
        selectExisting : false
        visible : script.environment.os.platform === 'linux' ? true : false /// fixes a display bug in some linux distributions

        property var callback: null

        onAccepted: {
            if ( callback ){
                callback()
                callback = null
            } else if ( projectView.focusEditor.document ){
                var editordoc = projectView.focusEditor.document
                if ( !editordoc.saveAs(fileSaveDialog.fileUrl) ){
                    messageBox.show(
                        'Failed to save file to: ' + fileSaveDialog.fileUrl,
                        {
                            button3Name : 'Ok',
                            button3Function : function(){ messageBox.close(); }
                        }
                    )
                    return;
                }

                if ( !project.isDirProject() ){
                    project.openProject(fileSaveDialog.fileUrl)
                    projectView.focusEditor.document = project.active
                } else if ( project.isFileInProject(fileSaveDialog.fileUrl ) ){
                    projectView.focusEditor.document = project.openFile(fileSaveDialog.fileUrl, ProjectDocument.Edit)
                    if ( project.active && project.active !== projectView.focusEditor.document ){
                        engine.createObjectAsync(
                            project.active.content,
                            runSpace,
                            project.active.file.pathUrl(),
                            project.active,
                            true
                        );
                    }
                } else {
                    var fileUrl = fileSaveDialog.fileUrl
                    messageBox.show(
                        'File is outside project scope. Would you like to open it as a new project?',
                    {
                        button1Name : 'Open as project',
                        button1Function : function(){
                            var projectUrl = fileUrl
                            projectView.closeProject(
                                function(){
                                    project.openProject(projectUrl);
                                    projectView.focusEditor.document = project.active
                                }
                            )
                            messageBox.close()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(){
                            messageBox.close()
                        },
                        returnPressed : function(){
                            var projectUrl = fileUrl
                            projectView.closeProject(
                                function(){
                                    project.openProject(projectUrl)
                                    projectView.focusEditor.document = project.active
                                }
                            )
                            messageBox.close()
                        }
                    })
                }
            }
        }
        onRejected:{
            fileSaveDialog.callback = null
        }
        Component.onCompleted: {
            visible: false
            close()
        }
    }

    Component{
        id: editorFactory

        Editor{
            id: editorComponent
            height: parent.height
            width: 400
            windowControls: controls
            onInternalActiveFocusChanged: if ( internalActiveFocus ) projectView.setFocusEditor(editorComponent)

            Component.onCompleted: {
                projectView.setFocusEditor(editorComponent)
                if ( project.active ){
                    editorComponent.document = project.active
                }
                forceFocus()
            }
        }
    }

    Component{
        id: fragmentEditorFactory

        FragmentEditor{
            id: fragmentEditorComponent
            height: parent.height
            width: 400
            windowControls: controls
            onInternalActiveFocusChanged: if ( internalActiveFocus ) projectView.setFocusEditor(fragmentEditorComponent)

            Component.onCompleted: {
                projectView.setFocusEditor(fragmentEditorComponent)
                forceFocus()
            }
        }
    }

    Component{
        id: fragmentDocumentFactory
        DocumentFragment{}
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
            color: "#0e1924"
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
            if ( !projectView.focusEditor || !projectView.focusEditor.document ){
                addHorizontalEditor();
                return
            }

            var cursorBlock = projectView.focusEditor.getCursorFragment()
            var editorObject = null
            if ( cursorBlock.start !== cursorBlock.end ){
                var editorDocumentFragment = fragmentDocumentFactory.createObject(0)
                editorDocumentFragment.document = projectView.focusEditor.document
                editorDocumentFragment.lineStartIndex = cursorBlock.start
                editorDocumentFragment.lineEndIndex = cursorBlock.end + 1

                editorObject = fragmentEditorFactory.createObject(0)
                editorObject.height = mainHorizontalSplit.height
                editorObject.document = editorDocumentFragment

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

            } else {
                addHorizontalEditor()
            }
        }

        function removeHorizontalEditor(){
            mainHorizontalSplit.removeItem(projectView.focusEditor)
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
                projectNavigation.visible = !projectNavigation.visible
            }

            SplitView{
                id: mainHorizontalSplit
                anchors.fill: parent
                orientation: Qt.Horizontal
                handleDelegate: Rectangle {
                    implicitWidth: 1
                    implicitHeight: 1
                    color: "#060d13"
                }

                property var editors: [editor]

                Project{
                    id: projectView
                    height: parent.height
                    width: 240
                    visible : !livecv.settings.launchMode
                    windowControls: controls
                }

                Editor{
                    id: editor
                    height: parent.height
                    width: 400
                    visible : !livecv.settings.launchMode
                    windowControls: controls
                    onInternalActiveFocusChanged: if ( internalFocus ) projectView.focusEditor = editor

                    Component.onCompleted: {
                        projectView.focusEditor = editor
                        if ( project.active ){
                            editor.document = project.active
                        }
                        forceFocus()
                    }
                }

                Rectangle{
                    id : viewer
                    height : parent.height

                    color : "#000509"

                    Item{
                        id: runSpace
                        anchors.fill: parent
                        property string program: editor.text
                        property variant item

                        Timer{
                            id: createTimer
                            interval: 1000
                            running: true
                            repeat : false
                            onTriggered: {
                                if (project.active === project.inFocus && project.active){
                                    livecv.engine.createObjectAsync(
                                        runSpace.program,
                                        runSpace,
                                        project.active.file.pathUrl(),
                                        project.active
                                    );
                                } else if ( project.active ){
                                    livecv.engine.createObjectAsync(
                                        project.active.content,
                                        runSpace,
                                        project.active.file.pathUrl(),
                                        project.active,
                                        true
                                    );
                                }
                            }
                        }

                        Connections{
                            target: livecv.engine
                            onAboutToCreateObject : {
                                if (staticContainer)
                                    staticContainer.beforeCompile()
                                if ( engineMonitor )
                                    engineMonitor.emitBeforeCompile()
                            }
                            onObjectCreated : {
                                error.text = ''
                                if (runSpace.item) {
                                    runSpace.item.destroy();
                                }
                                runSpace.item = object;
                                if ( staticContainer )
                                    staticContainer.afterCompile()
                                if ( engineMonitor )
                                    engineMonitor.emitAfterCompile()
                            }
                            onObjectCreationError : {
                                var lastErrorsText = ''
                                var lastErrorsLog  = ''
                                for ( var i = 0; i < errors.length; ++i ){
                                    var lerror = errors[i]
                                    var errorFile = lerror.fileName
                                    var index = errorFile.lastIndexOf('/')
                                    if ( index !== -1 && index < errorFile.length - 1)
                                        errorFile = errorFile.substring(index + 1)

                                    lastErrorsText +=
                                        (i !== 0 ? '<br>' : '') +
                                        '<a href=\"' + lerror.fileName + ':' + lerror.lineNumber + '\">' +
                                            errorFile + ':' + lerror.lineNumber +
                                        '</a>' +
                                        ' ' + lerror.message
                                    lastErrorsLog +=
                                        (lastErrorsLog === '' ? '' : '\n') +
                                        'Error: ' + lerror.fileName + ':' + lerror.lineNumber + ' ' + lerror.message
                                }
                                error.text = lastErrorsText
                                console.error(lastErrorsLog)
                            }
                        }
                    }

                    Rectangle{
                        id : errorWrap
                        anchors.bottom: parent.bottom
                        height : error.text !== '' ? error.height + 20 : 0
                        width : parent.width
                        color : editor.color
                        Behavior on height {
                            SpringAnimation { spring: 3; damping: 0.1 }
                        }

                        Rectangle{
                            width : 14
                            height : parent.height
                            color : "#601818"
                            visible: error.visible
                        }
                        Text {
                            id: error
                            anchors.left : parent.left
                            anchors.leftMargin: 25
                            anchors.verticalCenter: parent.verticalCenter

                            width: parent.width
                            wrapMode: Text.Wrap
                            textFormat: Text.StyledText

                            linkColor: "#c5d0d7"
                            font.family: "Ubuntu Mono, Courier New, Courier"
                            font.pointSize: editor.font.pointSize
                            text: ''
                            color: "#c5d0d7"
                            visible : text === "" ? false : true

                            onLinkActivated: {
                                project.openFile(link.substring(0, link.lastIndexOf(':')), ProjectDocument.EditIfNotOpen)
                            }
                        }
                    }

                }
            }

            Rectangle{
                id: paletteBox
                x: 0
                y: visible ? 0 : 20
                width: paletteItem ? paletteItem.width + 10: 0
                height: paletteItem ? paletteItem.height + 10 : 0
                color: "#02070b"
                radius: 5
                border.width: 1
                border.color: "#061b24"
                visible: children.length > 0
                opacity: visible ? 1 : 0
                clip: true

                property Item paletteItem : null
                children: paletteItem ? [paletteItem] : []

                Behavior on opacity{
                    NumberAnimation{ duration : 200; easing.type: Easing.OutCubic; }
                }
                Behavior on y{
                    id : moveYBehavior
                    NumberAnimation{ duration : 200; easing.type: Easing.OutCubic; }
                }

                function setPalette(palette, cursorRectangle, editorPosition){
                    if ( palette ){
                        moveYBehavior.enabled = false

                        var startY = editorPosition.y + cursorRectangle.y + 38

                        paletteBox.x = editorPosition.x + cursorRectangle.x + 7
                        paletteBox.y = startY - cursorRectangle.y / 2

                        palette.item.x = 5
                        palette.item.y = 7

                        paletteBox.paletteItem = palette.item

                        moveYBehavior.enabled = true

                        var newX = paletteBox.x - paletteBox.width / 2
                        paletteBox.x = newX < 0 ? 0 : newX

                        var upY = startY - paletteBox.height
                        var downY = startY + cursorRectangle.height + 5
                        paletteBox.y = upY > 0 ? upY : downY
                    } else {
                        paletteBox.paletteItem = null
                    }
                }
            }

            ProjectNavigation{
                id: projectNavigation
                anchors.left: parent.left

                width: projectView.width + editor.width
                height: parent.height
                visible: false
                onOpen: {
                    projectView.focusEditor.document = project.openFile(path, ProjectDocument.EditIfNotOpen)
                    projectView.focusEditor.forceFocus()
                }
                onCloseFile: {
                    var doc = project.documentModel.isOpened(path)
                    if ( doc ){
                        if ( doc.isDirty ){
                            messageBox.show('File contains unsaved changes. Would you like to save them before closing?',
                            {
                                button1Name : 'Yes',
                                button1Function : function(){
                                    doc.save()
                                    project.closeFile(path)
                                    messageBox.close()
                                },
                                button2Name : 'No',
                                button2Function : function(){
                                    project.closeFile(path)
                                    messageBox.close()
                                },
                                button3Name : 'Cancel',
                                button3Function : function(){
                                    messageBox.close()
                                },
                                returnPressed : function(){
                                    doc.save()
                                    project.closeFile(path)
                                    messageBox.close()
                                }
                            })
                        } else
                            project.closeFile(path)
                    }
                }
                onCancel: {
                    editor.forceFocus()
                }
            }

            ProjectAddEntry{
                id: projectAddEntry
                anchors.left: parent.left
                width: projectView.width + editor.width
                height: parent.height
                visible: false
                onAccepted: {
                    if ( isFile ){
                        var f = project.fileModel.addFile(entry, name)
                        if ( f !== null )
                            project.openFile(f, ProjectDocument.Edit)
                    } else {
                        project.fileModel.addDirectory(entry, name)
                    }
                }
            }
        }
    }

    property LogContainer logView : LogContainer{
        id: logView
        visible: false
        isInWindow: false
        width: parent.width
        height: 200

        onItemAdded: {
            if ( !visible  )
                header.isLogWindowDirty = true
        }
    }


    MessageDialogMain{
        id: messageBox
        anchors.fill: parent
        visible: false
        backgroudColor: "#050e16"
    }

    License{
        id: licenseBox
        anchors.fill: parent
        visible: false
    }

    Connections{
        target: project
        onActiveChanged : {
            if (runSpace.item) {
                runSpace.item.destroy();
                runSpace.item = 0
                if ( staticContainer )
                    staticContainer.clearStates()
                if ( engineMonitor )
                    engineMonitor.emitTargetChanged()
            }
            if (active)
                createTimer.restart()
        }
    }

    Connections{
        target: project.documentModel
        onMonitoredDocumentChanged : {
            if (project.active === project.inFocus){
                engine.createObjectAsync(
                    runSpace.program,
                    runSpace,
                    project.active.file.pathUrl(),
                    project.active
                );
            } else if ( project.active ){
                engine.createObjectAsync(
                    project.active.content,
                    runSpace,
                    project.active.file.pathUrl(),
                    project.active,
                    true
                );
            }
        }
        onDocumentChangedOutside : {
            messageBox.show(
                'File \'' + document.file.path + '\' changed outside Live CV. Would you like to reload it?',
                {
                    button1Name : 'Yes',
                    button1Function : function(){
                        messageBox.close()
                        document.readContent()
                        root.documentsReloaded = true
                    },
                    button2Name : 'Save',
                    button2Function : function(){
                        messageBox.close()
                        document.save()
                    },
                    button3Name : 'No',
                    button3Function : function(){
                        messageBox.close()
                    }
                }
            )
        }
    }


    // Logo

    Image{
        id: logo

        anchors.top: parent.top
        anchors.topMargin: 9
        anchors.left: parent.left
        anchors.leftMargin: 14

        opacity: settings.launchMode ? 0.3 : 1.0
        source : "qrc:/images/logo.png"
    }

    Connections{
        target: project.fileModel
        onError : messageBox.show(message,{
            button2Name : 'Ok',
            button2Function : function(){ messageBox.close(); }
        })
    }

}
