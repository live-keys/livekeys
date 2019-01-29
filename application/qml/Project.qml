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

Rectangle{
    id: root
    color : "#060e17"
    gradient: Gradient{
        GradientStop { position: 0.0;  color: "#060e17" }
        GradientStop { position: 0.01; color: "#060e17" }
    }
    objectName: "project"

    property WindowControls windowControls : null
    property Item focusEditor : null
    property Item navEditor: null

    function maintainCodingMode()
    {
        if (focusEditor && focusEditor.document && focusEditor.document !== project.active && windowControls.codingMode === 0)
        {
            windowControls.codingMode = 1
            windowControls.wasLiveCoding = true
            modeImage.source = onSaveImage.source
            modeImage.anchors.rightMargin = onSaveImage.anchors.rightMargin
        }
        if (focusEditor && focusEditor.document && focusEditor.document === project.active && windowControls.wasLiveCoding)
        {
            windowControls.wasLiveCoding = false
            windowControls.codingMode = 0
            modeImage.source = liveImage.source
            modeImage.anchors.rightMargin = liveImage.anchors.rightMargin
        }

        modeContainer.visible = false
    }

    function setFocusEditor(e){
        if (focusEditor !== e ){
            if (focusEditor) focusEditor.internalFocus = false
            focusEditor = e
            focusEditor.internalFocus = true
        }

        maintainCodingMode();
    }

    function setNavEditor(e){
        if (navEditor && navEditor !== e)
        {
            navEditor = e;
        }
    }

    Component.onCompleted: {
        livecv.commands.add(root, {
            'close' : [closeProject, "Close Project"],
            'open' : [openProject, "Open Project"],
            'new' : [newProject, "New Project"],
            'openFile' : [openFile, "Open File"],
            'toggleVisibility' : [toggleVisibility, "Toggle Visibility"]
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
        windowControls.messageDialog.show(message, {
            button1Name : 'Yes',
            button1Function : function(){
                windowControls.messageDialog.close()
                if ( !project.isDirProject() && documentList.length === 1 && documentList[0] === ''){
                    var closeCallback = callback;
                    var untitledDocument = documentModel.isOpened(documentList[0])
                    fileSaveDialog.callback = function(){
                        if ( !untitledDocument.saveAs(windowControls.saveFileDialog.fileUrl) ){
                            windowControls.messageDialog.show(
                                'Failed to save file to: ' + windowControls.saveFileDialog.fileUrl,
                                {
                                    button3Name : 'Ok',
                                    button3Function : function(){ windowControls.messageDialog.close(); }
                                }
                            )
                            return;
                        }
                        project.closeProject()
                        closeCallback()
                    }
                    windowControls.saveFileDialog.open()
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
                    windowControls.messageDialog.show(message,{
                        button1Name : 'Close',
                        button1Function : function(){
                            project.closeProject()
                            windowControls.messageDialog.close()
                            callback()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(){
                            windowControls.messageDialog.close()
                        },
                        returnPressed : function(){
                            project.closeProject()
                            windowControls.messageDialog.close()
                            callback()
                        }
                    })
                } else {
                    project.closeProject()
                    windowControls.messageDialog.close()
                    callback()
                }
            },
            button2Name : 'No',
            button2Function : function(){
                project.closeProject()
                windowControls.messageDialog.close()
                callback()
            },
            button3Name : 'Cancel',
            button3Function : function(){
                windowControls.messageDialog.close()
            },
            returnPressed : function(){
                project.closeProject()
                windowControls.messageDialog.close()
            }
        })
    }
    function openProject(){
        root.closeProject(function(){
            root.windowControls.openDirDialog.open()
        })
    }
    function newProject(){
        closeProject(function(){
            project.newProject()
            if ( project.active )
            {
                focusEditor.document = project.active
                maintainCodingMode();
            }
        })
    }
    function openFile(){
        if ( !project.isDirProject() ){
            closeProject(function(){
                root.windowControls.openFileDialog.open()
            })
        } else {
            root.windowControls.openFileDialog.open()
        }
    }

    function openCommandsMenu(){
        livecv.commands.model.updateAvailableCommands()
    }

    function addEntry(parentEntry, isFile){
        projectAddEntry.show(parentEntry, isFile)
    }
    function openEntry(entry, monitor){
        root.focusEditor.document = project.openFile(
            entry, monitor ? ProjectDocument.Monitor : ProjectDocument.EditIfNotOpen
        )

        maintainCodingMode();
    }
    function editEntry(entry){
        root.focusEditor.document = project.openFile(entry, ProjectDocument.Edit)
        maintainCodingMode();
    }
    function removeEntry(entry, isFile){
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

        windowControls.messageDialog.show(message, {
            button1Name : 'Yes',
            button1Function : function(){
                project.fileModel.removeEntry(entry)
                windowControls.messageDialog.close()
            },
            button3Name : 'No',
            button3Function : function(){
                windowControls.messageDialog.close()
            },
            returnPressed : function(){
                project.fileModel.removeEntry(entry)
                windowControls.messageDialog.close()
            }
        })
    }
    function moveEntry(entry, newParent){
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

        windowControls.messageDialog.show(message, {
            button1Name : 'Yes',
            button1Function : function(){
                project.fileModel.moveEntry(entry, newParent)
                windowControls.messageDialog.close()
            },
            button3Name : 'No',
            button3Function : function(){
                windowControls.messageDialog.close()
            },
            returnPressed : function(){
                project.fileModel.moveEntry(entry, newParent)
                windowControls.messageDialog.close()
            }
        })
    }

    function renameEntry(entry, newName){
        project.fileModel.renameEntry(entry, newName)
    }

    function toggleVisibility(){
        root.width = root.width === 0 ? 240 : 0
    }


    TreeView {
        id: view
        model: project.fileModel
        anchors.topMargin: 10
        anchors.leftMargin: 10
        anchors.fill: parent

        style: TreeViewStyle{
            backgroundColor: "transparent"
            transientScrollBars: false
            handle: Item {
                implicitWidth: 10
                implicitHeight: 10
                Rectangle {
                    color: "#0b1f2e"
                    anchors.fill: parent
                }
            }
            scrollBarBackground: Item{
                implicitWidth: 10
                implicitHeight: 10
                Rectangle{
                    anchors.fill: parent
                    color: "#091823"
                }
            }
            textColor: '#9babb8'
            decrementControl: null
            incrementControl: null
            frame: Rectangle{color: "transparent"}
            corner: Rectangle{color: "#091823"}
        }

        property var dropEntry: null
        property var dragEntry: null

        property var contextDelegate : null

        function setContextDelegate(delegate){
            if ( contextDelegate !== null )
                contextDelegate.editMode = false
            contextDelegate = delegate
        }

        TableViewColumn {
            title: "Title"
            role: "fileName"
            resizable: true
        }

        selectionMode: SelectionMode.NoSelection

        headerVisible: false

        itemDelegate: Item{
            id: entryDelegate

            property bool editMode: false

            function path(){
                return styleData.value.path
            }
            function entry(){
                return styleData.value
            }
            function focusText(){
                entryData.selectAll()
                entryData.forceActiveFocus()
            }
            function unfocusText(){
                entryData.activeFocus = false
            }
            function setActive(){
                project.setActive(styleData.value)
                projectView.maintainCodingMode();
            }
            function openFile(){
                root.editEntry(styleData.value)
            }
            function monitorFile(){
                root.openEntry(styleData.value, true)
            }

            function openExternally(){
                Qt.openUrlExternally(styleData.value.path)
            }

            Rectangle{
                id: boundingRect
                height: 22
                anchors.left: parent.left
                anchors.top: parent.top
                width: entryData.width > 70 ? entryData.width + 30 : 95
                color: entryDelegate.editMode ? "#1b2934" : "transparent"
                Image{
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    source: {
                        if ( styleData.value && styleData.value.isFile ){
                            if (styleData.value === (project.active ? project.active.file : null) )
                                return "qrc:/images/project-file-active.png"
                            else {
                                if ( styleData.value.document ){
                                    if (styleData.value.document.isMonitored)
                                        return "qrc:/images/project-file-monitor.png"
                                    else if ( styleData.value.document.isDirty)
                                        return "qrc:/images/project-file-unsaved.png"
                                }
                                return "qrc:/images/project-file.png"
                            }
                        } else
                            return "qrc:/images/project-directory.png"
                    }
                }

                TextInput{
                    id: entryData
                    anchors.left: parent.left
                    anchors.leftMargin: 25
                    anchors.verticalCenter: parent.verticalCenter
                    color: styleData.value === view.dropEntry ? "#ff0000" : styleData.textColor
                    text: {
                        styleData.value
                            ? styleData.value.name === ''
                            ? 'untitled' : styleData.value.name : ''
                    }
                    font.family: 'Open Sans, Arial, sans-serif'
                    font.pixelSize: 12
                    property int type : {
                        if (styleData.value){
                            if ( styleData.value.document ){
                                if ( root.focusEditor && root.focusEditor.document ){
                                    if ( root.focusEditor.document.file === styleData.value )
                                        return 1
                                }
                                return 2
                            }
                        }
                        return 0
                    }

                    font.weight: type === 1 ? Font.Bold : Font.Light
                    font.italic: type === 2
                    readOnly: !entryDelegate.editMode
                    Keys.onReturnPressed: {
                        root.renameEntry(styleData.value, text)
                        entryDelegate.editMode = false
                    }
                    Keys.onEscapePressed: {
                        entryData.text = styleData.value
                                ? styleData.value.name === ''
                                ? 'untitled' : styleData.value.name : ''
                        entryDelegate.editMode = false
                    }
                }
                MouseArea {
                    id: dragArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    drag.target: parent
                    drag.onActiveChanged: {
                        if ( drag.active ){
                            view.dragEntry = styleData.value
                            view.collapse(styleData.index)
                        }
                        boundingRect.Drag.drop()
                    }
                    onClicked: {
                        if( mouse.button === Qt.RightButton ){
                            if ( styleData.value.isFile ){
                                view.setContextDelegate(entryDelegate)
                                fileContextMenu.popup()
                            } else if ( styleData.value.path === project.rootPath ){
                                view.setContextDelegate(entryDelegate)
                                projectContextMenu.popup()
                            } else {
                                view.setContextDelegate(entryDelegate)
                                dirContextMenu.popup()
                            }
                        } else if ( mouse.button === Qt.LeftButton ) {
                            if ( entryDelegate.editMode){
                                if (!entryData.focus)
                                    entryData.forceActiveFocus()
                                else
                                    mouse.accepted = false
                            }
                            if ( view.contextDelegate )
                                view.contextDelegate.editMode = false
                        }
                    }
                    onDoubleClicked: {
                        if ( styleData.value.isFile )
                            root.openEntry(styleData.value, false)
                        else {
                            var modelIndex = project.fileModel.itemIndex(styleData.value)
                            if (view.isExpanded(modelIndex))
                                view.collapse(modelIndex)
                            else
                                view.expand(modelIndex)
                        }
                    }
                }

                states: [
                    State {
                        when: boundingRect.Drag.active
                        AnchorChanges {
                            target: boundingRect
                            anchors.top: undefined
                            anchors.left: undefined
                        }
                        ParentChange {
                            target: boundingRect
                            parent: view
                        }
                    }
                ]

                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2
                Drag.active: dragArea.drag.active
            }
        }
        rowDelegate: Item{
            height: 22
        }

        DropArea{
            anchors.fill: parent
            onPositionChanged: {
                view.dropEntry = null
                var index = view.indexAt(view.flickableItem.contentX + drag.x, view.flickableItem.contentY + drag.y)
                var item = project.fileModel.itemAt(index)
                if ( item && item !== view.dragEntry && !item.isFile )
                    view.dropEntry = item
            }
            onDropped: {
                if ( view.dropEntry !== null ){
                    root.moveEntry(view.dragEntry, view.dropEntry)
                    view.dragEntry = null
                    view.dropEntry = null
                }
            }
        }
        Connections{
            target: project.fileModel
            onProjectNodeChanged : {
                view.expand(index)
            }
        }

        Menu {
            id: fileContextMenu

            style: ContextMenuStyle{}

            MenuItem{
                text: "Edit File"
                onTriggered: {
                    view.contextDelegate.openFile()
                }
            }
            MenuItem{
                text: "Monitor file"
                onTriggered: {
                    view.contextDelegate.monitorFile()
                }
            }

            MenuItem{
                text: "Set As Active"
                onTriggered: {
                    view.contextDelegate.setActive()
                }
            }
            MenuItem {
                text: "Rename"
                onTriggered: {
                    view.contextDelegate.editMode = true
                    view.contextDelegate.focusText()
                }
            }
            MenuItem {
                text: "Delete"
                onTriggered: {
                    root.removeEntry(view.contextDelegate.entry(), false)
                }
            }

        }

        Menu {
            id: dirContextMenu
            style: ContextMenuStyle{}
            MenuItem{
                text: "Show in Explorer"
                onTriggered: {
                    view.contextDelegate.openExternally()
                }
            }
            MenuItem {
                text: "Rename"
                onTriggered: {
                    view.contextDelegate.editMode = true
                    view.contextDelegate.focusText()
                }
            }
            MenuItem{
                text: "Delete"
                onTriggered: {
                    root.removeEntry(view.contextDelegate.entry(), true)
                }
            }
            MenuItem {
                text: "Add File"
                onTriggered: {
                    root.addEntry(view.contextDelegate.entry(), true)
                }
            }
            MenuItem {
                text: "Add Directory"
                onTriggered: {
                    root.addEntry(view.contextDelegate.entry(), false)
                }
            }
        }

        Menu {
            id: projectContextMenu
            style: ContextMenuStyle{}
            MenuItem{
                text: "Show in Explorer"
                onTriggered: {
                    view.contextDelegate.openExternally()
                }
            }
            MenuItem{
                text: "Close project"
                onTriggered: {
                    root.closeProject(function(){})
                }
            }
            MenuItem {
                text: "Add File"
                onTriggered: {
                    root.addEntry(view.contextDelegate.entry(), true)
                }
            }
            MenuItem {
                text: "Add Directory"
                onTriggered: {
                    root.addEntry(view.contextDelegate.entry(), false)
                }
            }
        }
    }
}

