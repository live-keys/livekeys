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

import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import base 1.0
import editor 1.0
import editor.private 1.0

Item{
    id: root
    objectName: "project"

    property QtObject windowControls : null
    property QtObject runSpace: null

    property Timer compileTimer : Timer{
        interval: 1000
        running: true
        repeat : false
        onTriggered: root.compile()
    }

    property Item addEntryOverlay : ProjectAddEntry{
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

    property Item navigation : ProjectNavigation{
        id: projectNavigation
        anchors.fill: parent
        visible: false
        onOpen: {
            projectNavigation.parent.document = project.openFile(path, ProjectDocument.EditIfNotOpen)
            root.windowControls.workspace.panes.setActiveItem(projectNavigation.parent.textEdit, projectNavigation.parent)
        }
        onCloseFile: {
            var doc = project.documentModel.isOpened(path)
            if ( doc ){
                if ( doc.isDirty ){
                    root.windowControls.dialogs.message('File contains unsaved changes. Would you like to save them before closing?',
                    {
                        button1Name : 'Yes',
                        button1Function : function(mbox){
                            doc.save()
                            project.closeFile(path)
                            mbox.close()
                            root.compile()
                        },
                        button2Name : 'No',
                        button2Function : function(mbox){
                            project.closeFile(path)
                            mbox.close()
                            root.compile()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(mbox){
                            mbox.close()
                        },
                        returnPressed : function(mbox){
                            doc.save()
                            project.closeFile(path)
                            mbox.close()
                            root.compile()
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

    Connections{
        target: project
        onActiveChanged : {
            if (root.runSpace.item) {
                root.runSpace.item.destroy();
                root.runSpace.item = 0
                if ( staticContainer )
                    staticContainer.clearStates()
            }
            if (project.active){
                if ( pathChange ){
                    var fe = root.findFocusEditor()
                    if ( fe )
                        fe.document = project.active
                }

                root.compileTimer.restart()
            }
            pathChange = false
        }

        property bool pathChange: false

        onPathChanged: pathChange = true
    }

    Connections{
        target: project.documentModel
        onMonitoredDocumentChanged : {
            if (controls.codingMode === 0){  // live coding
                controls.workspace.project.compile()
            }
        }
        onDocumentChangedOutside : {
            root.windowControls.dialogs.message(
                'File \'' + document.file.path + '\' changed outside Live CV. Would you like to reload it?',
                {
                    button1Name : 'Yes',
                    button1Function : function(mbox){
                        mbox.close()
                        document.readContent()
                        root.documentsReloaded = true
                    },
                    button2Name : 'Save',
                    button2Function : function(mbox){
                        mbox.close()
                        document.save()
                    },
                    button3Name : 'No',
                    button3Function : function(mbox){
                        mbox.close()
                    }
                }
            )
        }
    }

    Connections{
        target: project.fileModel
        onError : root.windowControls.dialogs.message(message,{
            button2Name : 'Ok',
            button2Function : function(mbox){ mbox.close(); }
        })
    }

    function compile(){
        var documentList = project.documentModel.listUnsavedDocuments()
        livecv.engine.createObjectAsync(
            project.active.content,
            livecv.layerPlaceholder(),
            project.active.file.pathUrl(),
            project.active,
            !(documentList.size === 1 && documentList[0] === project.active)
        );
    }

    function findFocusEditor(){
        var ap = livecv.windowControls().workspace.panes.activePane
        if ( ap.objectName === 'editor' ){
            return ap;
        }

        var openPanes = livecv.windowControls().workspace.panes.open
        for ( var i = 0; i < openPanes.length; ++i ){
            if ( openPanes[i].objectName === 'editor' )
                return openPanes[i]
        }

        return null;
    }

    function toggleVisibility(){
        var pfs = null

        var openPanes = livecv.windowControls().workspace.panes.open
        for ( var i = 0; i < openPanes.length; ++i ){
            if ( openPanes[i].objectName === 'projectFileSystem' ){
                pfs = openPanes[i]
                break
            }
        }

        if ( pfs )
            pfs.width = pfs.width === 0 ? 240 : 0
    }

    Component.onCompleted: {
        livecv.commands.add(root, {
            'close' : [closeProject, "Close Project"],
            'open' : [openProject, "Open Project"],
            'new' : [newProject, "New Project"],
            'openFile' : [openFile, "Open File"],
            'toggleVisibility' : [toggleVisibility, "Project: Toggle Visibility"]
        })
    }

    function closeProject(callback){
        var documentList = project.documentModel.listUnsavedDocuments()
        var message = ''
        if ( documentList.length === 0 ){
            project.closeProject()
            callback()
            return;
        } else if ( !project.isDirProject() ){
            message = "Your project file has unsaved changes. Would you like to save them before closing it?";
        } else {
            var unsavedFiles = '';
            for ( var i = 0; i < documentList.length; ++i ){
                unsavedFiles += documentList[i] + "\n";
            }

            message = "The following files have unsaved changes:\n";
            message += unsavedFiles
            message += "Would you like to save them before closing the project?\n"
        }
        windowControls.dialogs.message(message, {
            button1Name : 'Yes',
            button1Function : function(box){
                box.close()

                if ( !project.isDirProject() && documentList.length === 1 && documentList[0] === ''){
                    var closeCallback = callback;
                    var untitledDocument = documentModel.isOpened(documentList[0])

                    windowControls.dialogs.saveFile(
                        { filters: [ "Qml files (*.qml)", "All files (*)" ] },
                        function(url){
                            if ( !untitledDocument.saveAs(url) ){
                                windowControls.dialogs.message(
                                    'Failed to save file to: ' + url,
                                    {
                                        button3Name : 'Ok',
                                        button3Function : function(mbox){ mbox.close() }
                                    }
                                )
                                return;
                            }
                            project.closeProject()
                            closeCallback()
                        }
                    )
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
                    box.close()
                    windowControls.dialogs.message(message,{
                        button1Name : 'Close',
                        button1Function : function(mbox){
                            project.closeProject()
                            mbox.close()
                            callback()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(mbox){
                            mbox.close()
                        },
                        returnPressed : function(mbox){
                            project.closeProject()
                            mbox.close()
                            callback()
                        }
                    })
                } else {
                    project.closeProject()
                    box.close()
                    callback()
                }
            },
            button2Name : 'No',
            button2Function : function(mbox){
                project.closeProject()
                mbox.close()
                callback()
            },
            button3Name : 'Cancel',
            button3Function : function(mbox){
                mbox.close()
            },
            returnPressed : function(mbox){
                project.closeProject()
                mbox.close()
            }
        })
    }
    function openProject(){
        root.closeProject(function(){
            root.windowControls.dialogs.openDir({}, function(url){
                project.openProject(url)
            })
        })
    }
    function newProject(){
        closeProject(function(){
            project.newProject()
        })
    }
    function openFile(){
        var openCallback = function(url){
            if ( project.rootPath === '' ){
                project.openProject(url)
            } else if ( project.isFileInProject(url) ) {
                var doc = project.openFile(url, ProjectDocument.Edit)
                if ( doc ) {
                    var fe = root.findFocusEditor()
                    if ( fe )
                        fe.document = doc
                }
            } else {
                var fileUrl = url
                root.windowControls.dialogs.message(
                    'File is outside project scope. Would you like to open it as a new project?',
                {
                    button1Name : 'Open as project',
                    button1Function : function(mbox){
                        var projectUrl = fileUrl
                        projectView.closeProject(function(){
                            project.openProject(projectUrl)
                        })
                        mbox.close()
                    },
                    button3Name : 'Cancel',
                    button3Function : function(mbox){
                        mbox.close()
                    },
                    returnPressed : function(mbox){
                        var projectUrl = fileUrl
                        header.closeProject(
                            function(){
                                project.openProject(projectUrl)
                            }
                        )
                        mbox.close()
                    }
                })
            }
        }

        if ( !project.isDirProject() ){
            closeProject(function(){
                root.windowControls.dialogs.openFile(
                    { filters: ["Qml files (*.qml)", "All files (*)"] },
                    openCallback
                )
            })
        } else {
            root.windowControls.dialogs.openFile(
                { filters: ["Qml files (*.qml)", "All files (*)"] },
                openCallback
            )
        }
    }
}

