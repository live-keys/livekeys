/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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
import QtQuick.Controls 2.4
import base 1.0 as Base
import editor 1.0
import editor.private 1.0
import fs 1.0 as Fs
import visual.input 1.0 as Input
import workspace 1.0

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
                var f = lk.layers.workspace.project.fileModel.addFile(entry, name)
                if ( callbackFunction ){
                    callbackFunction(f)
                }
            } else {
                var e = lk.layers.workspace.project.fileModel.addDirectory(entry, name)
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
            root.openFile(path, ProjectDocument.EditIfNotOpen)
        }
        onCloseFile: {
            root.wizards.closeFile(path)
        }
        onCancel: {
            if ( lk.layers.workspace.panes.activeItem )
                lk.layers.workspace.panes.activeItem.forceActiveFocus()
        }
    }

    property QtObject __private : QtObject{

        property var refreshActive : function(){
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
                var path = lk.layers.workspace.project.active.path
                var format = lk.layers.workspace.fileFormats.find(path)
                fe.document = lk.layers.workspace.project.openFile(
                    Fs.UrlInfo.urlFromLocalFile(path), {type: 'text', format: format}
                )
            }
        }
    }


    Connections{
        target: lk.layers.workspace.project
        function onActiveChanged(){
            if (lk.layers.workspace.project.active){
                if ( pathChange ){
                    root.__private.refreshActive()
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
        target: lk.layers.workspace.project.documentModel
        function onDocumentChangedOutside(document){
            lk.layers.window.dialogs.message(
                'File \'' + document.path + '\' changed outside Livekeys. Would you like to reload it?',
                {
                    button1Name : 'Yes',
                    button1Function : function(mbox){
                        mbox.close()
                        document.readContent()
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

    function toggleVisibility(){
        var pfs = root.panes.focusPane('projectFileSystem')

        if (pfs){
            root.panes.removePane(pfs)
        } else {
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
                    'toggleProjectVisibility' : [toggleVisibility, "Project File View: Toggle Visibility"]
                })
            }
        }
    }

    property QtObject wizards : QtObject{

        function openProjectDirViaDialog(callback){
            root.wizards.checkUnsavedFiles(function(){
                lk.layers.window.dialogs.openDir({}, function(url){
                    root.wizards.openProject(url, callback)
                    // lk.layers.workspace.openProjectInstance(url)
                })
            })
        }

        function openProjectFileViaDialog(callback){
            root.wizards.checkUnsavedFiles(function(){
                lk.layers.window.dialogs.openFile({}, function(url){
                    lk.layers.workspace.project.closeProject()
                    lk.layers.workspace.project.openProject(url)
                    var path = Fs.UrlInfo.toLocalFile(url)
                    if ( callback )
                        callback(path)
                })
            })
        }

        function openProject(url, callback){
            root.wizards.checkUnsavedFiles(function(){
                lk.layers.workspace.project.closeProject()
                var path = Fs.UrlInfo.toLocalFile(url)

                Base.Time.delay(0, function(){
                    lk.layers.workspace.project.openProject(url)

                    if ( !lk.layers.workspace.project.active ){
                        var message = 'Folder doesn\'t have a main project file. Would you like to create one?'
                        var createFile = function(mbox){
                            root.wizards.addFile(
                                lk.layers.workspace.project.dir(),
                                {
                                    'extension': 'qml',
                                    'heading' : 'Add main file in ' + lk.layers.workspace.project.dir()
                                },
                                function(file){
                                    lk.layers.workspace.project.setActive(file.path)
                                    if ( callback )
                                        callback(path)
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

                    } else {
                        if ( callback )
                            callback(path)
                    }
                    // lk.layers.workspace.openProjectInstance(url)
                })
            })
        }

        function newProject(callback){
            root.wizards.closeProject(function(){
                // delay so that previous panes get deleted before new ones are added
                Base.Time.delay(0, function(){
                    lk.layers.workspace.project.newProject()
                    if ( callback )
                        callback()
                    // lk.layers.workspace.newProjectInstance()
                })
            })
        }

        function closeProject(callback){
            root.wizards.checkUnsavedFiles(function(){
                lk.layers.workspace.project.closeProject()
                if ( callback )
                    callback()
            })
        }


        function openFileViaDialog(opt, callback){
            var openCallback = function(url){
                var localPath = Fs.UrlInfo.toLocalFile(url)
                if ( lk.layers.workspace.project.rootPath === '' ){
                    lk.layers.workspace.project.openProject(url)
                    var path = Fs.UrlInfo.toLocalFile(url)
                    if ( callback )
                        callback(path)
                } else if ( lk.layers.workspace.project.isFileInProject(url) ){
                    root.openFile(url, ProjectDocument.EditIfNotOpen, callback)
                } else if ( !lk.layers.workspace.project.canRunFile(localPath) ){
                    root.openFile(url, ProjectDocument.EditIfNotOpen, callback)
                } else {
                    var fileUrl = url
                    lk.layers.window.dialogs.message(
                        'File is outside project scope. Would you like to open it as a new project?',
                    {
                        button1Name : 'Open as project',
                        button1Function : function(mbox){
                            var projectUrl = fileUrl
                            root.wizards.closeProject(function(){
                                lk.layers.workspace.project.openProject(projectUrl)
                                var path = Fs.UrlInfo.toLocalFile(projectUrl)
                                if ( callback )
                                    callback(path)
                            })
                            mbox.close()
                        },
                        button2Name : 'Open file',
                        button2Function : function(mbox){
                            root.openFile(url, ProjectDocument.EditIfNotOpen, callback)
                        },
                        button3Name : 'Cancel',
                        button3Function : function(mbox){
                            mbox.close()
                        },
                        returnPressed : function(mbox){
                            var projectUrl = fileUrl
                            root.wizards.closeProject(function(){
                                Base.Time.delay(0, function(){
                                    lk.layers.workspace.project.openProject(projectUrl)
                                    var path = Fs.UrlInfo.toLocalFile(projectUrl)
                                    if ( callback )
                                        callback(path)
                                })
                            })
                            mbox.close()
                        }
                    })
                }
            }

            if ( !lk.layers.workspace.project.isDirProject() ){
                root.wizards.checkUnsavedFiles(function(){
                    lk.layers.window.dialogs.openFile(
                        { filters: opt && opt.filters ? opt.filters : ["Qml files (*.qml)", "All files (*)"] },
                        function(url){
                            lk.layers.workspace.project.closeProject()
                            lk.layers.workspace.project.openProject(url)
                            var path = Fs.UrlInfo.toLocalFile(url)
                            if ( callback )
                                callback(path)
                        }
                    )
                })
            } else {
                lk.layers.window.dialogs.openFile(
                    { filters: opt && opt.filters ? opt.filters : ["All files (*)"] },
                    openCallback
                )
            }
        }

        function saveDocument(document, callback){
            if ( !document )
                return
            if ( document.isOnDisk() ){
                document.save()
                if (callback)
                    callback(Fs.UrlInfo.urlFromLocalFile(document.path))
            } else {
                saveDocumentAs(document, callback)
            }
        }

        function saveDocumentAs(document, callback){
            if ( !document )
                return
            lk.layers.window.dialogs.saveFile(
                { filters: Fs.UrlInfo.urlFromLocalFile(document.path) },
                function(url){
                    if ( !document.saveAs(url) ){
                        lk.layers.window.dialogs.message(
                            'Failed to save file to: ' + url,
                            {
                                button3Name : 'Ok',
                                button3Function : function(mbox){
                                    mbox.close()
                                }
                            }
                        )
                        return
                    }

                    if ( !lk.layers.workspace.project.isDirProject() ){
                        lk.layers.workspace.project.openProject(url)
                        var path = Fs.UrlInfo.toLocalFile(url)
                        if (callback)
                            callback(path)
                    } else if ( lk.layers.workspace.project.isFileInProject(url) ){
                        lk.layers.workspace.environment.fileSystem.openFile(url, ProjectDocument.Edit)
                        var path = Fs.UrlInfo.toLocalFile(url)
                        if (callback)
                            callback(path)
                    } else {
                        var fileUrl = url
                        lk.layers.window.dialogs.message(
                            'File is outside project scope. Would you like to open it as a new project?',
                        {
                            button1Name : 'Open as project',
                            button1Function : function(mbox){
                                var projectUrl = fileUrl
                                projectView.closeProject(
                                    function(){
                                        lk.layers.workspace.project.openProject(projectUrl)
                                        var path = Fs.UrlInfo.toLocalFile(projectUrl)
                                        if (callback)
                                            callback(path)
                                    }
                                )
                                mbox.close()
                            },
                            button3Name : 'Cancel',
                            button3Function : function(mbox){
                                mbox.close()
                                if (callback)
                                    callback(url)
                            },
                            returnPressed : function(mbox){
                                var projectUrl = fileUrl
                                projectView.closeProject(
                                    function(){
                                        lk.layers.workspace.project.openProject(url)
                                        var path = Fs.UrlInfo.toLocalFile(url)
                                        if (callback)
                                            callback(path)
                                    }
                                )
                                mbox.close()
                            }
                        })
                    }
                }
            )
        }

        // workspace.formatToFileFilters

        function closeDocument(document, callback){
            if ( !document )
                return
            if ( document.isDirty ){
                var saveFunction = function(mbox, callback){
                    if ( document.isOnDisk() ){
                        document.save()
                        root.fileSystem.closeDocument(document, callbacK)
                    } else {
                        var fileFilter = document.formatType
                                ? lk.layers.workspace.fileFormats.fileFilterFromFormat(document.formatType)
                                : ''
                        var filters = fileFilter.length === '' ? ["All files (*)"] :  [fileFilter,"All files (*)"]

                        lk.layers.window.dialogs.saveFile(
                            { filters: filters },
                            function(url){
                                if ( !document.saveAs(url) ){
                                    lk.layers.window.dialogs.message(
                                        'Failed to save file to: ' + url,
                                        {
                                            button3Name : 'Ok',
                                            button3Function : function(){
                                                lk.layers.window.dialogs.messageClose()
                                            }
                                        }
                                    )
                                    return;
                                }
                                root.fileSystem.closeDocument(document, callback)
                            }
                        )
                    }
                    mbox.close()
                }


                lk.layers.window.dialogs.message('File contains unsaved changes. Would you like to save them before closing?',
                {
                    button1Name : 'Yes',
                    button1Function : function(mbox){
                        saveFunction(mbox, callback)
                    },
                    button2Name : 'No',
                    button2Function : function(mbox){
                        root.fileSystem.closeDocument(path, callback)
                    },
                    button3Name : 'Cancel',
                    button3Function : function(mbox){
                        mbox.close()
                    },
                    returnPressed : function(mbox){
                        saveFunction(mbox, callback)
                    },
                })
            } else {
                root.fileSystem.closeDocument(document, callback)
            }
        }

        function switchActive(url){
            var path = Fs.UrlInfo.toLocalFile(url)
            if ( lk.layers.workspace.project.active && lk.layers.workspace.project.active.path === path )
                return

            lk.layers.workspace.project.setActive(path)
            root.fileSystem.updateProjectFile()
        }

        function addFile(path, opt, callback){

            var entry = lk.layers.workspace.project.fileModel.findPath(path)
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
            var entry = lk.layers.workspace.project.fileModel.findPath(path)
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

        function removePath(path, callback){
            var message = ''
            if ( !Fs.Path.exists(path) )
                return
            if ( Fs.Path.isDir(path) ){
                var documentList = lk.layers.workspace.project.documentModel.listUnsavedDocumentsInPath(entry.path)
                if ( documentList.length === 0 ){
                    message =
                        "Are you sure you want to remove directory\'" + path + "\' " +
                        "and all its contents?"
                } else {
                    var unsavedFiles = '';
                    for ( var i = 0; i < documentList.length; ++i ){
                        unsavedFiles += documentList[i] + "\n";
                    }

                    message = "The following files have unsaved changes:\n";
                    message += unsavedFiles
                    message +=
                        "Are you sure you want to remove directory \'" + path +
                        "\' and all its contents?\n"
                }
            } else {
                var doc = lk.layers.workspace.project.isOpened(Fs.UrlInfo.urlFromLocalFile(path))
                if  ( doc && doc.isDirty ){
                    message = "Are you sure you want to remove unsaved file \'" + path + "\'?"
                } else {
                    message = "Are you sure you want to remove file \'" + path + "\'?"
                }
            }

            lk.layers.window.dialogs.message(message, {
                button1Name : 'Yes',
                button1Function : function(mbox){
                    root.fileSystem.removePath(path)
                    mbox.close()
                    if ( callback )
                        callback()
                },
                button3Name : 'No',
                button3Function : function(mbox){
                    mbox.close()
                },
                returnPressed : function(mbox){
                    root.fileSystem.removePath(path)
                    mbox.close()
                    if ( callback )
                        callback()
                }
            })
        }

        function movePath(path, newParent, callback){
            var message = ''
            if ( !Fs.Path.exists(path) )
                return

            if ( !Fs.Path.isDir(path) ){
                var doc = lk.layers.workspace.project.isOpened(Fs.UrlInfo.urlFromLocalFile(path))
                if  ( doc && doc.isDirty ){
                    message = lang([
                        "Are you sure you want to move unsaved file '%0'?\n",
                        "All your changes will be lost."
                    ]).format(entry.path)
                } else {
                    root.fileSystem.movePath(path, newParent)
                    if ( callback )
                        callback()
                    return;
                }
            } else {
                var documentList = lk.layers.workspace.project.documentModel.listUnsavedDocumentsInPath(entry.path)
                if ( documentList.length === 0 ){
                    root.fileSystem.movePath(path, newParent)
                    if ( callback )
                        callbac()
                    return;
                } else {
                    var unsavedFiles = '';
                    for ( var i = 0; i < documentList.length; ++i ){
                        unsavedFiles += documentList[i] + "\n";
                    }
                    message = lang([
                        "The following files have unsaved changes:\n %0",
                        "Are you sure you want to move directory \'%1\'",
                        "and all its contents? Unsaved changes will be lost."
                    ]).format(unsavedFiles, entry.path)
                }
            }

            lk.layers.window.dialogs.message(message, {
                button1Name : 'Yes',
                button1Function : function(mbox){
                    root.fileSystem.movePath(path, newParent)
                    mbox.close()
                    if ( callback )
                        callback()
                },
                button3Name : 'No',
                button3Function : function(mbox){
                    mbox.close()
                },
                returnPressed : function(mbox){
                    root.fileSystem.movePath(path, newParent)
                    mbox.close()
                    if ( callback )
                        callback()
                }
            })
        }

        function checkUnsavedFiles(callback){
            var documentList = lk.layers.workspace.project.documentModel.listUnsavedDocuments()
            callback = callback ? callback : function(){}
            var message = ''
            if ( documentList.length === 0 ){
                callback()
                return;
            } else if ( !lk.layers.workspace.project.isDirProject() ){
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

                    if ( !lk.layers.workspace.project.isDirProject() && documentList.length === 1 && documentList[0] === ':/0'){
                        var closeCallback = callback;
                        var untitledDocument = lk.layers.workspace.project.documentModel.isOpened(documentList[0])

                        var fileFilter = untitledDocument.formatType
                                ? lk.layers.workspace.fileFormats.fileFilterFromFormat(untitledDocument.formatType)
                                : ''
                        var filters = fileFilter.length === '' ? ["All files (*)"] :  [fileFilter,"All files (*)"]


                        lk.layers.window.dialogs.saveFile(
                            { filters: filters },
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
                                lk.layers.workspace.project.closeProject()
                                closeCallback()
                            }
                        )
                    } else if ( !lk.layers.workspace.project.documentModel.saveDocuments() ){
                        var unsavedList = lk.layers.workspace.project.documentModel.listUnsavedDocuments()
                        unsavedFiles = '';
                        for ( var i = 0; i < unsavedList.length; ++i ){
                            unsavedFiles += lk.layers.workspace.project.fileModel.printableName(unsavedList[i]) + "\n";
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

    property QtObject fileSystem: QtObject{

        function openFile(path, mode, callback){
            var format = lk.layers.workspace.fileFormats.find(path)
            var pane = lk.layers.workspace.interceptFile(path, format, mode)
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

            var doc = lk.layers.workspace.project.openFile(path, {type: 'text', format: format, mode: mode})
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
            fe.document = doc
            if ( callback )
                callback(path, doc, fe)
            return fe
        }

        function openDocumentInEditor(doc, callback){
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
            fe.document = doc
            if ( callback )
                callback(path, doc, fe)
        }

        function hasProjectFile(){
            return Fs.Path.exists(lk.layers.workspace.project.path('live.project.json'))
        }

        function updateProjectFile(){
            var activePath = lk.layers.workspace.project.active.path
            var projectPath = lk.layers.workspace.project.dir()
            var relativeActivePath = activePath.replace(projectPath + '/', '')

            if ( hasProjectFile() ){
                var projectFileUrl = Fs.UrlInfo.urlFromLocalFile(lk.layers.workspace.project.path('live.project.json'))
                var doc = lk.layers.workspace.project.openFile(projectFileUrl, { type: 'binary', fileSystem: true })
                var docContent = doc.content.toString()

                var contentObject = JSON.parse(docContent)
                contentObject.active = relativeActivePath
                doc.setContent(JSON.stringify(contentObject))
                doc.save()
                lk.layers.workspace.project.closeFile(lk.layers.workspace.project.path('live.project.json'))
            } else {
                var doc = lk.layers.workspace.project.createDocument({ type: 'binary', fileSystem: true })
                doc.setContent(JSON.stringify({active: relativeActivePath}))
                doc.saveAs(lk.layers.workspace.project.path('live.project.json'))
                lk.layers.workspace.project.closeFile(lk.layers.workspace.project.path('live.project.json'))
            }
        }

        function removePath(path){
            lk.layers.workspace.project.fileModel.removePath(path)
        }

        function renamePath(path, name){
            lk.layers.workspace.project.fileModel.renamePath(path, name)
        }

        function movePath(path, newParent){
            lk.layers.workspace.project.fileModel.movePath(path, newParent)
        }

        function closeDocument(document, callback){
            if ( !lk.layers.workspace.project.isDirProject() && document.path === lk.layers.workspace.project.active.path ){
                lk.layers.window.dialogs.message(
                    'Closing this file will also close this project. Would you like to close the project?',
                {
                    button1Name : 'Yes',
                    button1Function : function(mbox){
                        lk.layers.workspace.project.closeProject()
                        mbox.close()
                        if ( callback )
                            callback()
                    },
                    button3Name : 'No',
                    button3Function : function(mbox){
                        mbox.close()
                    },
                    returnPressed : function(mbox){
                        lk.layers.workspace.project.closeProject()
                        mbox.close()
                        if ( callback )
                            callback()
                    }
                })
            } else {
                lk.layers.workspace.project.closeFile(document.path)
                if ( callback )
                    callback()
            }
        }
    }

}

