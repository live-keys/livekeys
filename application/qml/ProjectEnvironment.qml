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

    property QtObject panes: null
    property QtObject runSpace: null

    property Item addEntryOverlay : ProjectAddEntry{
        onAccepted: {
            if ( isFile ){
                var f = project.fileModel.addFile(entry, name)
                livecv.layers.workspace.project.openFile(f.path, ProjectDocument.Edit)
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
            root.panes.setActiveItem(projectNavigation.parent.textEdit, projectNavigation.parent)
            projectEnvironment.openFile(path, ProjectDocument.EditIfNotOpen)
        }
        onCloseFile: {
            var doc = project.documentModel.isOpened(path)
            if ( doc ){
                if ( doc.isDirty ){
                    livecv.layers.window.dialogs.message('File contains unsaved changes. Would you like to save them before closing?',
                    {
                        button1Name : 'Yes',
                        button1Function : function(mbox){
                            doc.save()
                            project.closeFile(path)
                            mbox.close()
                        },
                        button2Name : 'No',
                        button2Function : function(mbox){
                            project.closeFile(path)
                            mbox.close()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(mbox){
                            mbox.close()
                        },
                        returnPressed : function(mbox){
                            doc.save()
                            project.closeFile(path)
                            mbox.close()
                        }
                    })
                } else
                    project.closeFile(path)
            }
        }
        onCancel: {
            var activePane = livecv.layers.workspace.panes.activePane
            if ( activePane )
                activePane.forceActiveFocus()
        }
    }

    Connections{
        target: project
        onActiveChanged : {
            if (project.active){
                if ( pathChange ){
                    var fe = root.panes.focusPane('editor')
                    if ( !fe )
                        fe = root.panes.createPane('editor', {}, [1], [400, 0])
                    fe.document = project.active
                }
            }
            pathChange = false
        }

        property bool pathChange: false

        onPathChanged: pathChange = true
    }

    Connections{
        target: project.documentModel
        onDocumentChangedOutside : {
            livecv.layers.window.dialogs.message(
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
        onError : livecv.layers.window.dialogs.message(message,{
            button2Name : 'Ok',
            button2Function : function(mbox){ mbox.close(); }
        })
    }

    function toggleVisibility(){
        var pfs = null

        var openPanes = root.panes.open
        for ( var i = 0; i < openPanes.length; ++i ){
            if ( openPanes[i].objectName === 'projectFileSystem' ){
                pfs = openPanes[i]
                break
            }
        }

        if ( pfs )
            pfs.width = pfs.width === 0 ? 240 : 0
    }


    Connections{
        target: livecv
        onLayerReady: {
            if ( layer.name === 'workspace' ){
                layer.commands.add(root, {
                    'close' : [closeProject, "Close Project"],
                    'open' : [openProject, "Open Project"],
                    'new' : [newProject, "New Project"],
                    'openFile' : [openFileDialog, "Open File"],
                    'toggleVisibility' : [toggleVisibility, "Project: Toggle Visibility"]
                })
            }
        }
    }

    function closeProject(callback){
        var documentList = project.documentModel.listUnsavedDocuments()
        callback = callback ? callback : function(){}
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
        livecv.layers.window.dialogs.message(message, {
            button1Name : 'Yes',
            button1Function : function(box){
                box.close()

                if ( !project.isDirProject() && documentList.length === 1 && documentList[0] === ''){
                    var closeCallback = callback;
                    var untitledDocument = project.documentModel.isOpened(documentList[0])

                    livecv.layers.window.dialogs.saveFile(
                        { filters: [ "Qml files (*.qml)", "All files (*)" ] },
                        function(url){
                            if ( !untitledDocument.saveAs(url) ){
                                livecv.layers.window.dialogs.message(
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
                    livecv.layers.window.dialogs.message(message,{
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
            livecv.layers.window.dialogs.openDir({}, function(url){
                project.openProject(url)
            })
        })
    }
    function newProject(){
        closeProject(function(){
            project.newProject()
        })
    }
    function openFileDialog(){
        var openCallback = function(url){
            if ( project.rootPath === '' ){
                project.openProject(url)
            } else if ( project.isFileInProject(url) ) {
                openFile(url, ProjectDocument.EditIfNotOpen)
            } else {
                var fileUrl = url
                livecv.layers.window.dialogs.message(
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
                livecv.layers.window.dialogs.openFile(
                    { filters: ["Qml files (*.qml)", "All files (*)"] },
                    openCallback
                )
            })
        } else {
            livecv.layers.window.dialogs.openFile(
                { filters: ["Qml files (*.qml)", "All files (*)"] },
                openCallback
            )
        }
    }

    function openFile(path, mode){
        var pane = livecv.layers.workspace.interceptFile(path, mode)
        if ( pane )
            return pane

        var doc = project.openFile(path, mode)
        if ( !doc )
            return;
        var fe = root.panes.focusPane('editor')
        if ( !fe ){
            fe = root.panes.createPane('editor', {}, [1], [400, 400])
        }
        fe.document = doc
        return fe
    }
}

