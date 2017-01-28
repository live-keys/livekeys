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

ApplicationWindow {
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
        }
    }

    title: qsTr("Live CV")

    signal afterCompile()
    signal aboutToRecompile()
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
        visible: true
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        color: "#08141d"

        function closeProject(callback){
            var documentList = project.documentModel.listUnsavedDocuments()
            var message = ''
            if ( documentList.length === 0 ){
                project.closeProject()
                callback()
                return;
            } else if ( !project.isDirProject() && project.inFocus ){
                if ( !project.inFocus.file.isDirty ){
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
                    if ( !project.documentModel.saveDocuments() ){
                        var unsavedList = project.documentModel.listUnsavedDocuments()
                        unsavedFiles = '';
                        for ( var i = 0; i < unsavedList.length; ++i ){
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
            fileOpenDialog.open()
        }
        onOpenProject: {
            dirOpenDialog.open()
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

        onFontPlus: if ( editor.font.pixelSize < 24 ) editor.font.pixelSize += 2
        onFontMinus: if ( editor.font.pixelSize > 10 ) editor.font.pixelSize -= 2
    }

    FileDialog {
        id: fileOpenDialog
        title: "Please choose a file"
        nameFilters: [ "Qml files (*.qml)", "All files (*)" ]
        selectExisting : true
        visible : isLinux ? true : false // fixes a display bug in some linux distributions
        onAccepted: {
            if ( project.isFileInProject(fileOpenDialog.fileUrl ) )
                project.openFile(fileOpenDialog.fileUrl, ProjectDocument.Edit)
            else if ( !project.isDirProject() ){
                header.closeProject(function(){ project.openProject(fileOpenDialog.fileUrl) } )
            } else {
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
            header.closeProject(function(){
                project.openProject(dirOpenDialog.fileUrl)
            })
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
        onAccepted: {
            if ( project.inFocus ){
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
            header.callback()
            header.callback = function(){}
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
                color: "#071723"
            }

            Project{
                id: projectView
                height: parent.height
                width: 240
                visible : !args.previewFlag
                onOpenEntry: {
                    if ( project.inFocus )
                        project.inFocus.dumpContent(editor.text)
                    project.openFile(entry, monitor ? ProjectDocument.Monitor : ProjectDocument.EditIfNotOpen)
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
                visible : !args.previewFlag

                onSave: {
                    if ( project.inFocus.name !== '' ){
                        project.inFocus.dumpContent(editor.text)
                        project.inFocus.save()
                        if ( project.active && project.active !== project.inFocus ){
                            engine.createObjectAsync(
                                project.active.content,
                                tester,
                                project.active.file.pathUrl(),
                                true
                            );
                        }
                    } else {
                        fileSaveDialog.open()
                    }
                }
                onOpen: {
                    header.openFile()
                }
                onCloseFocusedFile: {
                    if ( project.inFocus.file.isDirty ){
                        messageBox.show('File contains unsaved changes. Would you like to save them before closing?',
                        {
                            button1Name : 'Yes',
                            button1Function : function(){
                                project.inFocus.dumpContent(editor.text)
                                project.inFocus.save()
                                project.closeFocusedFile()
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
                                project.inFocus.save()
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
                onToggleNavigation: {
                    projectNavigation.visible = !projectNavigation.visible
                }

                Component.onCompleted: forceFocus()
            }


            Rectangle{
                id : viewer
                height : parent.height

                color : "#05111b"

                Item {
                    id: tester
                    anchors.fill: parent
                    property string program: editor.text
                    property variant item
                    onProgramChanged: {
                        if ( project.active === project.inFocus )
                            createTimer.restart()
                        scopeTimer.restart()
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
                                    project.active.file.pathUrl()
                                );
                            } else if ( project.active ){
                                engine.createObjectAsync(
                                    project.active.content,
                                    tester,
                                    project.active.file.pathUrl(),
                                    true
                                );
                            }
                        }
                    }
                    Connections{
                        target: engine
                        onAboutToCreateObject : {
                            root.aboutToRecompile()
                        }
                        onObjectCreated : {
                            error.text = ''
                            if (tester.item) {
                                tester.item.destroy();
                            }
                            tester.item = object;
                            root.afterCompile()
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
                    color : "#141a1a"
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
                    if ( doc.file.isDirty ){
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
        backgroudColor: "#08141d"

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

    Connections{
        target: project
        onActiveChanged : {
            if (tester.item) {
                tester.item.destroy();
                tester.item = 0
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
                    project.active.file.pathUrl()
                );
            } else if ( project.active ){
                engine.createObjectAsync(
                    project.active.content,
                    tester,
                    project.active.file.pathUrl(),
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

    Connections{
        target: project.fileModel
        onError : messageBox.show(message,{
            button2Name : 'Ok',
            button2Function : function(){ messageBox.close(); }
        })
    }

}
