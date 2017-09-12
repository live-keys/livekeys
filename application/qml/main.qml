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
import Cv 1.0
import live 1.0

ApplicationWindow{
    id : root

    visible: true
    width: 1240
    height: 700
    color : "#293039"

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

    title: qsTr("Live CV")

    signal projectActiveChanged()

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
        Component.onCompleted: width = root.width
        text : lcvlog.data
        onTextChanged: {
            if ( !visible && text !== "" )
                header.isLogWindowDirty = true
        }
    }

    Top{
        id : header
        visible: !settings.previewMode
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: settings.previewMode ? 0 : 35
        color: "#08141d"

        function closeProject(callback){
            var documentList = project.documentModel.listUnsavedDocuments()
            var message = ''
            if ( documentList.length === 0 ){
                project.closeProject()
                callback()
                return;
            } else if ( !project.isDirProject() && project.inFocus ){
                if ( !project.inFocus.isDirty ){
                    project.closeProject()
                    callback()
                    return;
                } else {
                    message = "Your project file has unsaved changes. Would you like to save them before closing it?";
                }
            } else {
                var unsavedFiles = '';
                for ( var i = 0; i < documentList.length; ++i ){
                    unsavedFiles += documentList[i] + "\n";
                }

                message = "The following files have unsaved changes:\n";
                message += unsavedFiles
                message += "Would you like to save them before closing the project?\n"
            }
            messageBox.show(message, {
                button1Name : 'Yes',
                button1Function : function(){
                    messageBox.close()
                    if ( !project.isDirProject() && project.inFocus && project.inFocus.file.name === ''){
                        var closeCallback = callback;
                        fileSaveDialog.callback = function(){
                            project.inFocus.dumpContent(editor.text)
                            if ( !project.inFocus.saveAs(fileSaveDialog.fileUrl) ){
                                messageBox.show(
                                    'Failed to save file to: ' + fileSaveDialog.fileUrl,
                                    {
                                        button3Name : 'Ok',
                                        button3Function : function(){ messageBox.close(); }
                                    }
                                )
                                return;
                            }
                            project.closeProject()
                            closeCallback()
                        }
                        fileSaveDialog.open()
                    } else if ( !project.documentModel.saveDocuments() ){
                        var unsavedList = project.documentModel.listUnsavedDocuments()
                        unsavedFiles = '';
                        for ( var i = 0; i < unsavedList.length; ++i ){
                            if ( unsavedList[i] === '' )
                                unsavedList[i] = 'untitled'
                            unsavedFiles += unsavedList[i] + "\n";
                        }

                        message = 'Failed to save the following files:\n'
                        message += unsavedFiles
                        messageBox.show(message,{
                            button1Name : 'Close',
                            button1Function : function(){
                                project.closeProject()
                                messageBox.close()
                                callback()
                            },
                            button3Name : 'Cancel',
                            button3Function : function(){
                                messageBox.close()
                            },
                            returnPressed : function(){
                                project.closeProject()
                                messageBox.close()
                                callback()
                            }
                        })
                    } else {
                        project.closeProject()
                        messageBox.close()
                        callback()
                    }
                },
                button2Name : 'No',
                button2Function : function(){
                    project.closeProject()
                    messageBox.close()
                    callback()
                },
                button3Name : 'Cancel',
                button3Function : function(){
                    messageBox.close()
                },
                returnPressed : function(){
                    project.closeProject()
                    messageBox.close()
                }
            })
        }

        property var callback : function(){}

        isTextDirty: editor.isDirty

        property string action : ""

        onNewProject: {
            closeProject(function(){
                project.newProject()
            })
        }
        onOpenFile : {
            if ( !project.isDirProject() ){
                closeProject(function(){
                    fileOpenDialog.open()
                })
            } else {
                fileOpenDialog.open()
            }
        }
        onOpenProject: {
            closeProject(function(){
                dirOpenDialog.open()
            })
        }
        onSaveFile : {
            fileSaveDialog.open()
        }

        onToggleLogWindow : {
            if ( !logWindow.visible ){
                logWindow.show()
                isLogWindowDirty = false
            }
        }

        onOpenSettings: {
            project.openFile(settings.editor.path, ProjectDocument.Edit);
            settings.editor.documentOpened(project.inFocus)
        }

        onOpenLicense: licenseBox.visible = true
    }

    FileDialog {
        id: fileOpenDialog
        title: "Please choose a file"
        nameFilters: [ "Qml files (*.qml)", "All files (*)" ]
        selectExisting : true
        visible : isLinux ? true : false // fixes a display bug in some linux distributions
        onAccepted: {
            if ( project.path === '' )
                project.openProject(fileOpenDialog.fileUrl)
            else if ( project.isFileInProject(fileOpenDialog.fileUrl ) )
                project.openFile(fileOpenDialog.fileUrl, ProjectDocument.Edit)
            else {
                var fileUrl = fileOpenDialog.fileUrl
                messageBox.show(
                    'File is outside project scope. Would you like to open it as a new project?',
                {
                    button1Name : 'Open as project',
                    button1Function : function(){
                        var projectUrl = fileUrl
                        header.closeProject(
                            function(){ project.openProject(projectUrl) }
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
                            function(){ project.openProject(projectUrl) }
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

        visible : isLinux ? true : false /// fixes a display bug in some linux distributions
        onAccepted: {
            project.openProject(dirOpenDialog.fileUrl)
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
        visible : isLinux ? true : false /// fixes a display bug in some linux distributions

        property var callback: null

        onAccepted: {
            if ( callback ){
                callback()
                callback = null
            } else if ( project.inFocus ){
                project.inFocus.dumpContent(editor.text)
                if ( !project.inFocus.saveAs(fileSaveDialog.fileUrl) ){
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
                } else if ( project.isFileInProject(fileSaveDialog.fileUrl ) ){
                    project.openFile(fileSaveDialog.fileUrl, ProjectDocument.Edit)
                    if ( project.active && project.active !== project.inFocus ){
                        engine.createObjectAsync(
                            project.active.content,
                            tester,
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
                            header.closeProject(
                                function(){ project.openProject(projectUrl) }
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
                                function(){ project.openProject(projectUrl) }
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


    Rectangle{
        id : contentWrap
        anchors.top : header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height : parent.height - header.height

        SplitView{
            anchors.fill: parent
            orientation: Qt.Horizontal
            handleDelegate: Rectangle {
                implicitWidth: 1
                implicitHeight: 1
                color: "#060d13"
            }

            Project{
                id: projectView
                height: parent.height
                width: 240
                visible : !settings.previewMode
                onOpenEntry: {
                    if ( project.inFocus )
                        project.inFocus.dumpContent(editor.text)
                    project.openFile(entry, monitor ? ProjectDocument.Monitor : ProjectDocument.EditIfNotOpen)
                }
                onEditEntry : {
                    if ( project.inFocus )
                        project.inFocus.dumpContent(editor.text)
                    project.openFile(entry, ProjectDocument.Edit)
                }
                onAddEntry: {
                    projectAddEntry.show(parentEntry, isFile)
                }
                onRemoveEntry: {
                    var message = ''
                    if ( entry.isFile ){
                        if ( !entry.isDirty ){
                            message = "Are you sure you want to remove file \'" + entry.path + "\'?"
                        } else
                            message = "Are you sure you want to remove unsaved file \'" + entry.path + "\'?"
                    } else {
                        var documentList = project.documentModel.listUnsavedDocumentsInPath(entry.path)
                        if ( documentList.length === 0 ){
                            message =
                                "Are you sure you want to remove directory\'" + entry.path + "\' " +
                                "and all its contents?"
                        } else {
                            var unsavedFiles = '';
                            for ( var i = 0; i < documentList.length; ++i ){
                                unsavedFiles += documentList[i] + "\n";
                            }

                            message = "The following files have unsaved changes:\n";
                            message += unsavedFiles
                            message +=
                                "Are you sure you want to remove directory \'" + entry.path +
                                "\' and all its contents?\n"
                        }
                    }

                    messageBox.show(message, {
                        button1Name : 'Yes',
                        button1Function : function(){
                            project.fileModel.removeEntry(entry)
                            messageBox.close()
                        },
                        button3Name : 'No',
                        button3Function : function(){
                            messageBox.close()
                        },
                        returnPressed : function(){
                            project.fileModel.removeEntry(entry)
                            messageBox.close()
                        }
                    })
                }
                onMoveEntry: {
                    var message = ''
                    if ( entry.isFile ){
                        if ( !entry.isDirty ){
                            project.fileModel.moveEntry(entry, newParent)
                            return;
                        }
                        message =
                            "Are you sure you want to move unsaved file \'" + entry.path + "\'?\n" +
                            "All your changes will be lost."
                    } else {
                        var documentList = project.documentModel.listUnsavedDocumentsInPath(entry.path)
                        if ( documentList.length === 0 ){
                            project.fileModel.moveEntry(entry, newParent)
                            return;
                        } else {
                            var unsavedFiles = '';
                            for ( var i = 0; i < documentList.length; ++i ){
                                unsavedFiles += documentList[i] + "\n";
                            }

                            message = "The following files have unsaved changes:\n";
                            message += unsavedFiles
                            message +=
                                "Are you sure you want to move directory \'" + entry.path +
                                "\' and all its contents? Unsaved changes will be lost.\n"
                        }
                    }

                    messageBox.show(message, {
                        button1Name : 'Yes',
                        button1Function : function(){
                            project.fileModel.moveEntry(entry, newParent)
                            messageBox.close()
                        },
                        button3Name : 'No',
                        button3Function : function(){
                            messageBox.close()
                        },
                        returnPressed : function(){
                            project.fileModel.moveEntry(entry, newParent)
                            messageBox.close()
                        }
                    })
                }
                onRenameEntry: {
                    project.fileModel.renameEntry(entry, newName)
                }
                onCloseProject: {
                    header.closeProject(function(){})
                }
            }

            Editor{
                id: editor
                height: parent.height
                width: 400
                visible : !settings.previewMode

                font.pixelSize: settings.editor.fontSize

                onSave: {
                    if ( !project.inFocus )
                        return;
                    if ( project.inFocus.file.name !== '' ){
                        project.inFocus.dumpContent(editor.text)
                        project.inFocus.save()
                        if ( project.active && project.active !== project.inFocus ){
                            engine.createObjectAsync(
                                project.active.content,
                                tester,
                                project.active.file.pathUrl(),
                                project.active,
                                true
                            );
                        }
                    } else {
                        project.inFocus.dumpContent(editor.text)
                        fileSaveDialog.open()
                    }
                }
                onOpen: {
                    header.openFile()
                }
                onCloseFocusedFile: {
                    if ( project.inFocus.isDirty ){
                        messageBox.show('File contains unsaved changes. Would you like to save them before closing?',
                        {
                            button1Name : 'Yes',
                            button1Function : function(){
                                project.inFocus.dumpContent(editor.text)
                                if ( project.inFocus.file.name !== '' ){
                                    project.inFocus.save()
                                } else {
                                    fileSaveDialog.open()
                                    fileSaveDialog.callback = function(){
                                        if ( !project.inFocus.saveAs(fileSaveDialog.fileUrl) ){
                                            messageBox.show(
                                                'Failed to save file to: ' + fileSaveDialog.fileUrl,
                                                {
                                                    button3Name : 'Ok',
                                                    button3Function : function(){ messageBox.close(); }
                                                }
                                            )
                                            return;
                                        }
                                        project.closeFocusedFile()
                                    }
                                }
                                messageBox.close()
                            },
                            button2Name : 'No',
                            button2Function : function(){
                                project.closeFocusedFile()
                                messageBox.close()
                            },
                            button3Name : 'Cancel',
                            button3Function : function(){
                                messageBox.close()
                            },
                            returnPressed : function(){
                                project.inFocus.dumpContent(editor.text)
                                if ( project.inFocus.file.name !== '' ){
                                    project.inFocus.save()
                                } else {
                                    fileSaveDialog.callback = function(){
                                        if ( !project.inFocus.saveAs(fileSaveDialog.fileUrl) ){
                                            messageBox.show(
                                                'Failed to save file to: ' + fileSaveDialog.fileUrl,
                                                {
                                                    button3Name : 'Ok',
                                                    button3Function : function(){ messageBox.close(); }
                                                }
                                            )
                                            return;
                                        }
                                        project.closeFocusedFile()
                                    }
                                }
                                project.closeFocusedFile()
                                messageBox.close()
                            }
                        })
                    } else
                        project.closeFocusedFile()
                }

                onToggleSize: {
                    if ( editor.width < contentWrap.width / 2)
                        editor.width = contentWrap.width - contentWrap.width / 4
                    else if ( editor.width === contentWrap.width / 2 )
                        editor.width = contentWrap.width / 4
                    else
                        editor.width = contentWrap.width / 2
                }
                onToggleVisibility: {
                    if ( editor.width === 0 && projectView.width === 0 ){
                        editor.width = 400
                        projectView.width = 240
                    } else {
                        editor.width = 0
                        projectView.width = 0
                    }
                }
                onToggleProject: {
                    projectView.width = projectView.width === 0 ? 240 : 0
                }
                onToggleNavigation: {
                    projectNavigation.visible = !projectNavigation.visible
                }
                onEditFragment: codeHandler.edit(position, tester.item)
                onAdjustFragment: codeHandler.adjust(position, tester.item)

                onBindProperties: codeHandler.bind(position, length, tester.item)

                Component.onCompleted: forceFocus()
            }

            Rectangle{
                id : viewer
                height : parent.height

                color : "#081017"

                Item {
                    id: tester
                    anchors.fill: parent
                    property string program: editor.text
                    property variant item
                    Connections{
                        target: codeHandler
                        onContentsChangedManually: {
                            if ( project.active === project.inFocus )
                                createTimer.restart()
                            scopeTimer.restart()
                        }
                    }
                    Timer {
                        id: scopeTimer
                        interval: 1000
                        running: true
                        repeat : false
                        onTriggered: codeHandler.updateScope(editor.text)
                    }
                    Timer {
                        id: createTimer
                        interval: 1000
                        running: true
                        repeat : false
                        onTriggered: {
                            if (project.active === project.inFocus && project.active){
                                engine.createObjectAsync(
                                    tester.program,
                                    tester,
                                    project.active.file.pathUrl(),
                                    project.active
                                );
                            } else if ( project.active ){
                                engine.createObjectAsync(
                                    project.active.content,
                                    tester,
                                    project.active.file.pathUrl(),
                                    project.active,
                                    true
                                );
                            }
                        }
                    }
                    Connections{
                        target: engine
                        onAboutToCreateObject : {
                            if (staticContainer)
                                staticContainer.beforeCompile()
                            if ( engineMonitor )
                                engineMonitor.emitBeforeCompile()
                        }
                        onObjectCreated : {
                            error.text = ''
                            if (tester.item) {
                                tester.item.destroy();
                            }
                            tester.item = object;
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
            width: codeHandler.palette ? codeHandler.palette.item.width + 10: 0
            height: codeHandler.palette ? codeHandler.palette.item.height + 10 : 0
            color: "#02070b"
            border.width: 1
            border.color: "#061b24"
            visible: children.length > 0
            opacity: visible ? 1 : 0
            clip: true
            Behavior on opacity{
                NumberAnimation{ duration : 200; easing.type: Easing.OutCubic; }
            }
            Behavior on y{
                id : moveYBehavior
                NumberAnimation{ duration : 200; easing.type: Easing.OutCubic; }
            }

            Connections{
                target: codeHandler
                onPaletteChanged : {
                    if ( codeHandler.palette ){
                        var rect = editor.getCursorRectangle()
                        moveYBehavior.enabled = false

                        var startY = editor.y + rect.y + 38

                        paletteBox.x = editor.x + rect.x + 7
                        paletteBox.y = startY - rect.y / 2

                        codeHandler.palette.item.x = 5
                        codeHandler.palette.item.y = 7
                        paletteBox.children = [codeHandler.palette.item]

                        moveYBehavior.enabled = true

                        var newX = paletteBox.x - paletteBox.width / 2
                        paletteBox.x = newX < 0 ? 0 : newX

                        var upY = startY - paletteBox.height
                        var downY = startY + rect.height + 5
                        paletteBox.y = upY > 0 ? upY : downY
                    } else {
                        paletteBox.children = []
                    }
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
                if ( project.inFocus )
                    project.inFocus.dumpContent(editor.text)
                project.openFile(path, ProjectDocument.EditIfNotOpen)
                editor.forceFocus()
            }
            onCloseFile: {
                var doc = project.documentModel.isOpened(path)
                if ( doc ){
                    if ( doc.isDirty ){
                        messageBox.show('File contains unsaved changes. Would you like to save them before closing?',
                        {
                            button1Name : 'Yes',
                            button1Function : function(){
                                doc.dumpContent(editor.text)
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
                                doc.dumpContent(editor.text)
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


    MessageDialogInternal{
        id: messageBox
        anchors.fill: parent
        visible: false
        backgroudColor: "#050e16"

        function show(
            message,
            options
        ){
            var defaults = {
                button1Name : '',
                button1Function : null,
                button2Name : '',
                button2Function : null,
                button3Name : '',
                button3Function : null,
                returnPressed : function(){ messageBox.close(); },
                escapePressed : function(){ messageBox.close(); }
            }
            for ( var i in defaults )
                if ( typeof options[i] == 'undefined' )
                    options[i] = defaults[i]

            if ( options.button1Name !== '' ){
                messageBoxButton1.text     = options.button1Name
                messageBoxButton1.callback = options.button1Function
            }
            if ( options.button2Name !== '' ){
                messageBoxButton2.text     = options.button2Name
                messageBoxButton2.callback = options.button2Function
            }
            if ( options.button3Name !== '' ){
                messageBoxButton3.text     = options.button3Name
                messageBoxButton3.callback = options.button3Function
            }

            messageBox.message = message
            messageBox.returnPressed = options.returnPressed
            messageBox.escapePressed = options.escapePressed

            messageBox.visible = true
            messageBox.forceActiveFocus()
        }
        function close(){
            messageBox.message = ''
            messageBox.visible = false
            messageBoxButton1.text = ''
            messageBoxButton2.text = ''
            messageBoxButton3.text = ''
            editor.forceFocus()
        }

        MessageDialogButton{
            id: messageBoxButton1
            visible : text !== ''
        }

        MessageDialogButton{
            id: messageBoxButton2
            anchors.centerIn: parent
            visible : text !== ''
        }

        MessageDialogButton{
            id: messageBoxButton3
            anchors.right: parent.right
            visible : text !== ''
        }
    }

    License{
        id: licenseBox
        anchors.fill: parent
        visible: false
    }

    Connections{
        target: project
        onActiveChanged : {
            if (tester.item) {
                tester.item.destroy();
                tester.item = 0
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
                    tester.program,
                    tester,
                    project.active.file.pathUrl(),
                    project.active
                );
            } else if ( project.active ){
                engine.createObjectAsync(
                    project.active.content,
                    tester,
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
                        if ( project.inFocus === document )
                            document.dumpContent(editor.text)
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
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 14

        opacity: settings.previewMode ? 0.3 : 1.0
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
