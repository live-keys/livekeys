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

import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import base 1.0
import editor 1.0
import editor.private 1.0
import fs 1.0 as Fs
import visual.input 1.0 as Input

Item{
    id: root
    objectName: "project"

    property QtObject panes: null
    property QtObject runSpace: null

    property QtObject theme: lk.layers.workspace.themes.current

    property var documentationViewFactory : Component{
        DocumentationView{}
    }

    property Item addEntryOverlay : ProjectAddEntry{

        property var callbackFunction: null

        style: QtObject{
            property color background: root.theme.colorScheme.middlegroundOverlay
            property double borderWidth: 1
            property color borderColor: root.theme.colorScheme.middlegroundBorder
            property double radius: 5

            property QtObject headerStyle: root.theme.inputLabelStyle.textStyle
            property QtObject inputStyle: root.theme.inputStyle

            property QtObject applyButton: root.theme.buttons.apply
        }

        onAccepted: {
            if ( isFile ){
                var f = project.fileModel.addFile(entry, name)
                if ( callbackFunction ){
                    callbackFunction(f)
                }
            } else {
                var e = project.fileModel.addDirectory(entry, name)
                if ( callbackFunction ){
                    callbackFunction(e)
                }
            }
        }
    }

    property Item navigation : ProjectNavigation{
        id: projectNavigation
        anchors.fill: parent
        visible: false
        onOpen: {
            root.panes.setActiveItem(projectNavigation.parent.textEdit, projectNavigation.parent)
            root.wizards.openFile(path, ProjectDocument.EditIfNotOpen)
        }
        onCloseFile: {
            root.wizards.closeFile(path)
        }
        onCancel: {
            if ( lk.layers.workspace.panes.activeItem )
                lk.layers.workspace.panes.activeItem.forceActiveFocus()
        }
    }


    Connections{
        target: project
        function onActiveChanged(){
            if (project.active){
                if ( pathChange ){
                    var fe = root.panes.focusPane('editor')
                    if ( !fe ){
                        fe = root.panes.createPane('editor', {}, [400, 0])

                        var containerUsed = root.panes.container
                        if ( containerUsed.orientation === Qt.Vertical ){
                            for ( var i = 0; i < containerUsed.panes.length; ++i ){
                                if ( containerUsed.panes[i].paneType === 'splitview' &&
                                     containerUsed.panes[i].orientation === Qt.Horizontal )
                                {
                                    containerUsed = containerUsed.panes[i]
                                    break
                                }
                            }
                        }

                        root.panes.splitPaneHorizontallyWith(containerUsed, 0, fe)

                        var containerPanes = containerUsed.panes
                        if ( (containerPanes.length > 2 && containerPanes[2].width > 500 + containerPanes[0].width) || containerUsed.width === 0 ){
                            containerUsed.resizePane(containerPanes[0], containerPanes[0].width * 2)
                            containerUsed.resizePane(fe, 400)
                        }
                    }
                    if ( !fe.document ){
                        fe.document = project.openTextFile(project.active.path)
                    }
                }
            }
            pathChange = false
        }

        property bool pathChange: false

        function onPathChanged(){
            pathChange = true
        }
    }

    Connections{
        target: project.documentModel
        function onDocumentChangedOutside(document){
            lk.layers.window.dialogs.message(
                'File \'' + document.file.path + '\' changed outside Livekeys. Would you like to reload it?',
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
        function onError(message){
            lk.layers.window.dialogs.message(message,{
                button2Name : 'Ok',
                button2Function : function(mbox){ mbox.close(); }
            })
        }
    }

    function toggleVisibility(){
        var pfs = root.panes.focusPane('projectFileSystem')

        if (pfs)
        {
            root.panes.removePane(pfs)
        }
        else
        {
            pfs = root.panes.createPane('projectFileSystem')

            var containerUsed = root.panes.container
            if ( containerUsed.orientation === Qt.Vertical ){
                for ( var i = 0; i < containerUsed.panes.length; ++i ){
                    if ( containerUsed.panes[i].paneType === 'splitview' &&
                         containerUsed.panes[i].orientation === Qt.Horizontal )
                    {
                        containerUsed = containerUsed.panes[i]
                        break
                    }
                }
            }

            root.panes.splitPaneHorizontallyBeforeWith(containerUsed, 0, pfs)

            var containerPanes = containerUsed.panes
            if ( (containerPanes.length > 2 && containerPanes[2].width > 300 + containerPanes[0].width) || containerUsed.width === 0 ){
                containerPanes[0].width = containerPanes[0].width * 2
                pfs.width = 300
            }
        }

    }


    Connections{
        target: lk
        function onLayerReady(layer){
            if ( layer.name === 'workspace' ){
                layer.commands.add(root, {
                    'close' : [wizards.closeProject, "Close Project"],
                    'open' : [wizards.openProjectViaDialog, "Open Project"],
                    'new' : [wizards.newProject, "New Project"],
                    'openFile' : [wizards.openFileViaDialog, "Open File"],
                    'toggleProjectVisibility' : [toggleVisibility, "Project: Toggle Visibility"]
                })
            }
        }
    }

    property QtObject wizards : QtObject{

        function openProjectDirViaDialog(callback){
            root.wizards.checkUnsavedFiles(function(){
                lk.layers.window.dialogs.openDir({}, function(url){
                    project.closeProject()
                    project.openProject(url)
                    var path = Fs.UrlInfo.toLocalFile(url)
                    if ( callback )
                        callback(path)
    //                lk.openProjectInstance(url)
                })
            })
        }

        function openProjectFileViaDialog(callback){
            root.wizards.checkUnsavedFiles(function(){
                lk.layers.window.dialogs.openFile({}, function(url){
                    project.closeProject()
                    project.openProject(url)
                    var path = Fs.UrlInfo.toLocalFile(url)
                    if ( callback )
                        callback(path)
                })
            })
        }

        function openProject(url, callback){
            root.wizards.checkUnsavedFiles(function(){
                project.closeProject()

                var path = Fs.UrlInfo.toLocalFile(url)

                project.openProject(url)

                if ( !project.active ){
                    var message = 'Project has no file to run. Would you like to create one?'
                    var createFile = function(mbox){
                        root.wizards.addFile(
                            project.dir(),
                            {
                                'extension': 'qml',
                                'heading' : 'Add main file in ' + project.dir()
                            },
                            function(file){
                                project.setActive(file.path)
                            }
                        )
                        mbox.close()
                    }

                    lk.layers.window.dialogs.message(message, {
                        button1Name : 'Yes',
                        button1Function : createFile,
                        button3Name : 'No',
                        button3Function : function(mbox){ mbox.close() },
                        returnPressed : createFile
                    })

                }



                if ( callback )
                    callback(path)
    //                lk.openProjectInstance(url)
            })
        }

        function newProject(callback){
            root.wizards.closeProject(function(){
                project.newProject()
                if ( callback )
                    callback()
    //            lk.newProjectInstance()
            })
        }

        function closeProject(callback){
            root.wizards.checkUnsavedFiles(function(){
                project.closeProject()
                if ( callback )
                    callback()
            })
        }


        function openFileViaDialog(callback){
            var openCallback = function(url){
                if ( project.rootPath === '' ){
                    project.openProject(url)
                    var path = Fs.UrlInfo.toLocalFile(url)
                    if ( callback )
                        callback(path)
                } else if ( project.isFileInProject(url) ){
                    root.wizards.openFile(url, ProjectDocument.EditIfNotOpen, callback)
                } else {
                    var fileUrl = url
                    lk.layers.window.dialogs.message(
                        'File is outside project scope. Would you like to open it as a new project?',
                    {
                        button1Name : 'Open as project',
                        button1Function : function(mbox){
                            var projectUrl = fileUrl
                            root.wizards.closeProject(function(){
                                project.openProject(projectUrl)
                                var path = Fs.UrlInfo.toLocalFile(projectUrl)
                                if ( callback )
                                    callback(path)
                            })
                            mbox.close()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(mbox){
                            mbox.close()
                        },
                        returnPressed : function(mbox){
                            var projectUrl = fileUrl
                            root.wizards.closeProject(function(){
                                project.openProject(projectUrl)
                                var path = Fs.UrlInfo.toLocalFile(projectUrl)
                                if ( callback )
                                    callback(path)
                            })
                            mbox.close()
                        }
                    })
                }
            }

            if ( !project.isDirProject() ){
                root.wizards.checkUnsavedFiles(function(){
                    lk.layers.window.dialogs.openFile(
                        { filters: ["Qml files (*.qml)", "All files (*)"] },
                        function(url){
                            project.closeProject()
                            project.openProject(url)
                            var path = Fs.UrlInfo.toLocalFile(url)
                            if ( callback )
                                callback(path)
                        }
                    )
                })
            } else {
                lk.layers.window.dialogs.openFile(
                    { filters: ["Qml files (*.qml)", "All files (*)"] },
                    openCallback
                )
            }
        }

        /// callback will receive: path, document, pane
        function openFile(path, mode, callback){
            var pane = lk.layers.workspace.interceptFile(path, mode)
            if ( pane )
                return pane

            if ( Fs.Path.hasExtensions(path, 'html')){
                if ( Fs.Path.hasExtensions(path, 'doc.html')){
                    var docItem = documentationViewFactory.createObject()
                    if ( docItem ){
                        docItem.loadDocumentationHtml(path)
                        var docPane = mainSplit.findPaneByType('documentation')
                        if ( !docPane ){
                            var storeWidth = root.width
                            docPane = root.panes.createPane('documentation', {}, [400, 400])
                            root.panes.container.splitPane(0, docPane)
                        }
                        docPane.pageTitle = Fs.Path.baseName(path)
                        docPane.page = docItem

                        if ( callback )
                            callback(path, null, docPane)

                        return docPane
                    }
                }
            }

            var doc = project.openTextFile(path, mode)
            if ( !doc )
                return;
            var fe = root.panes.focusPane('editor')
            if ( !fe ){
                fe = root.panes.createPane('editor', {}, [400, 0])
                root.panes.container.splitPane(0, fe)

                var containerPanes = root.panes.container.panes
                if ( containerPanes.length > 2 && containerPanes[2].width > 500 + containerPanes[0].width){
                    containerPanes[0].width = containerPanes[0].width * 2
                    fe.width = 400
                }
            }
            if ( callback )
                callback(path, doc, fe)
            fe.document = doc
            return fe
        }

        function closeFile(path, callback){
            var doc = project.documentModel.isOpened(path)
            if ( doc ){
                if ( doc.isDirty ){
                    lk.layers.window.dialogs.message('File contains unsaved changes. Would you like to save them before closing?',
                    {
                        button1Name : 'Yes',
                        button1Function : function(mbox){
                            doc.save()
                            project.closeFile(path)
                            callback()
                            mbox.close()
                        },
                        button2Name : 'No',
                        button2Function : function(mbox){
                            project.closeFile(path)
                            callback()
                            mbox.close()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(mbox){
                            mbox.close()
                        },
                        returnPressed : function(mbox){
                            doc.save()
                            project.closeFile(path)
                            callback()
                            mbox.close()
                        }
                    })
                } else {
                    project.closeFile(path)
                    callback()
                }
            }
        }

        function addFile(path, opt, callback){

            var entry = project.fileModel.findPath(path)
            if ( !entry ){
                //TODO: Enable if directory is outside the project
                lk.layers.workspace.messages.pushError("Path not found in project: " + path, 100)
                return
            }

            root.addEntryOverlay.isFile = true
            root.addEntryOverlay.entry = entry
            if ( opt.hasOwnProperty('heading') ){
                root.addEntryOverlay.heading = opt['heading']
            } else {
                root.addEntryOverlay.heading = 'Add file in ' + entry.path
            }
            if ( opt.hasOwnProperty('suggestion') ){
                root.addEntryOverlay.setInitialValue(opt['suggestion'])
            } else {
                root.addEntryOverlay.setInitialValue('')
            }
            if ( opt.hasOwnProperty('extension') ){
                root.addEntryOverlay.extension = opt['extension']
            } else {
                root.addEntryOverlay.extension = ''
            }

            root.addEntryOverlay.callbackFunction = function(file){
                if ( callback )
                    callback(file)
            }

            lk.layers.window.dialogs.overlayBox(root.addEntryOverlay)
        }

        function addDirectory(path, opt, callback){
            var entry = project.fileModel.findPath(path)
            if ( !entry ){
                //TODO: Enable if directory is outside the project
                lk.layers.workspace.messages.pushError("Path not found in project: " + path, 100)
            }

            root.addEntryOverlay.isFile = false
            root.addEntryOverlay.entry = entry
            if ( opt.hasOwnProperty('heading') ){
                root.addEntryOverlay.heading = opt['heading']
            } else {
                root.addEntryOverlay.heading = 'Add file in ' + entry.path
            }
            if ( opt.hasOwnProperty('suggestion') ){
                root.addEntryOverlay.setInitialValue(opt['suggestion'])
            } else {
                root.addEntryOverlay.setInitialValue('')
            }
            if ( opt.hasOwnProperty('extension') ){
                root.addEntryOverlay.extension = opt['extension']
            } else {
                root.addEntryOverlay.extension = ''
            }

            root.addEntryOverlay.callbackFunction = function(dir){
                if ( callback )
                    callback(dir)
            }

            lk.layers.window.dialogs.overlayBox(root.addEntryOverlay)
        }

        function checkUnsavedFiles(callback){
            var documentList = project.documentModel.listUnsavedDocuments()
            callback = callback ? callback : function(){}
            var message = ''
            if ( documentList.length === 0 ){
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
            lk.layers.window.dialogs.message(message, {
                button1Name : 'Yes',
                button1Function : function(box){
                    box.close()

                    if ( !project.isDirProject() && documentList.length === 1 && documentList[0] === ':/0'){
                        var closeCallback = callback;
                        var untitledDocument = project.documentModel.isOpened(documentList[0])

                        lk.layers.window.dialogs.saveFile(
                            { filters: [ "Qml files (*.qml)", "All files (*)" ] },
                            function(url){
                                if ( !untitledDocument.saveAs(url) ){
                                    lk.layers.window.dialogs.message(
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
                            unsavedFiles += project.fileModel.printableName(unsavedList[i]) + "\n";
                        }

                        message = 'Failed to save the following files:\n'
                        message += unsavedFiles
                        box.close()
                        lk.layers.window.dialogs.message(message,{
                            button1Name : 'Close',
                            button1Function : function(mbox){
                                mbox.close()
                                callback()
                            },
                            button3Name : 'Cancel',
                            button3Function : function(mbox){
                                mbox.close()
                            },
                            returnPressed : function(mbox){
                                mbox.close()
                                callback()
                            }
                        })
                    } else {
                        box.close()
                        callback()
                    }
                },
                button2Name : 'No',
                button2Function : function(mbox){
                    mbox.close()
                    callback()
                },
                button3Name : 'Cancel',
                button3Function : function(mbox){
                    mbox.close()
                },
                returnPressed : function(mbox){
                    mbox.close()
                    callback()
                }
            })
        }
    }

}

