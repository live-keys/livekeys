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
import workspace 1.0

Pane{
    id: root
    color : currentTheme ? currentTheme.paneBackground : 'black'
    objectName: "projectFileSystem"

    property QtObject panes: null
    paneFocusItem : root
    paneType: 'projectFileSystem'
    paneState : { return {} }

    property Theme currentTheme : lk.layers.workspace.themes.current

    property Item addEntryOverlay : ProjectAddEntry{
        onAccepted: {
            if ( isFile ){
                var f = project.fileModel.addFile(entry, name)
                lk.layers.workspace.project.openFile(f.path, ProjectDocument.Edit)
            } else {
                project.fileModel.addDirectory(entry, name)
            }
        }
    }

    function addEntry(parentEntry, isFile){
        root.addEntryOverlay.entry = parentEntry
        root.addEntryOverlay.isFile = isFile

        lk.layers.window.dialogs.overlayBox(root.addEntryOverlay)
    }
    function openEntry(entry, monitor){
        lk.layers.workspace.project.openFile(
            entry.path, monitor ? ProjectDocument.Monitor : ProjectDocument.EditIfNotOpen
        )
    }
    function editEntry(entry){
        lk.layers.workspace.project.openFile(entry.path, ProjectDocument.Edit)
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

        lk.layers.window.dialogs.message(message, {
            button1Name : 'Yes',
            button1Function : function(mbox){
                project.fileModel.removeEntry(entry)
                mbox.close()
            },
            button3Name : 'No',
            button3Function : function(mbox){
                mbox.close()
            },
            returnPressed : function(mbox){
                project.fileModel.removeEntry(entry)
                mbox.close()
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

        lk.layers.window.dialogs.message(message, {
            button1Name : 'Yes',
            button1Function : function(mbox){
                project.fileModel.moveEntry(entry, newParent)
                mbox.close()
            },
            button3Name : 'No',
            button3Function : function(mbox){
                mbox.close()
            },
            returnPressed : function(mbox){
                project.fileModel.moveEntry(entry, newParent)
                mbox.close()
            }
        })
    }

    function renameEntry(entry, newName){
        project.fileModel.renameEntry(entry, newName)
    }

    Rectangle{
        id: paneTop
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right

        height: 30
        color: currentTheme ? currentTheme.paneTopBackground : 'black'

        PaneDragItem{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            onDragStarted: root.panes.__dragStarted(root)
            onDragFinished: root.panes.__dragFinished(root)
            display: "Project"
        }

        Text{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#808691"
            text: "Project"
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            font.weight: Font.Normal
        }

        Item{
            anchors.right: parent.right
            width: 30
            height: parent.height

            Image{
                id : paneOptions
                anchors.centerIn: parent
                source : "qrc:/images/pane-menu.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    projectMenu.visible = !projectMenu.visible
                }
            }
        }
    }

    PaneMenu{
        id: projectMenu
        visible: false
        anchors.right: root.right
        anchors.topMargin: 30
        anchors.top: root.top

        style: root.currentTheme.popupMenuStyle

        PaneMenuItem{
            text: qsTr("Move to new window")
            onClicked: {
                projectMenu.visible = false
                root.panes.movePaneToNewWindow(root)
            }
        }
        PaneMenuItem{
            text: qsTr("Remove Project View")
            onClicked: {
                projectMenu.visible = false
                root.panes.removePane(root)
            }
        }
    }

    TreeView {
        id: view
        model: project.fileModel
        anchors.topMargin: 40
        anchors.leftMargin: 10
        anchors.fill: parent

        style: TreeViewStyle{
            backgroundColor: "transparent"
            transientScrollBars: false
            handle: Item {
                implicitWidth: 10
                implicitHeight: 10
                Rectangle {
                    color: "#1f2227"
                    anchors.fill: parent
                }
            }
            scrollBarBackground: Item{
                implicitWidth: 10
                implicitHeight: 10
                Rectangle{
                    anchors.fill: parent
                    color: root.color
                }
            }
            textColor: '#9babb8'
            decrementControl: null
            incrementControl: null
            frame: Rectangle{color: "transparent"}
            corner: Rectangle{color: root.color}
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
                project.setActive(styleData.value.path)
            }
            function addRunnable(){
                project.openRunnable(styleData.value.path, [styleData.value.path])
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
                color: root.currentTheme
                       ? (entryDelegate.editMode ? root.currentTheme.projectPaneItemEditBackground
                                                 : root.currentTheme.projectPaneItemBackground )
                       : 'transparent'
                Image{
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    source: {
                        if ( styleData.value && styleData.value.isFile ){
                            if (project.active && styleData.value.path === project.active.path)
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
                    color: type === 1 ? "#c6d3de" : view && styleData.value === view.dropEntry ? "#ff0000" : styleData.textColor
                    text: {
                        styleData.value
                            ? project.fileModel.printableName(styleData.value.name)
                            : ''
                    }
                    font.family: 'Open Sans, Arial, sans-serif'
                    font.pixelSize: 12
                    property int type : {
                        if (styleData.value){
                            if ( styleData.value.document ){
                                var ap = root.panes.activePane
                                if ( ap && ap.objectName === 'editor' && ap.document && ap.document.file === styleData.value ){
                                    return 1;
                                }
                                return 2
                            }
                        }
                        return 0
                    }

                    font.weight: Font.Light
                    font.italic: type === 2 || type === 1
                    readOnly: !entryDelegate.editMode
                    selectionColor: "#3d4856"
                    Keys.onReturnPressed: {
                        root.renameEntry(styleData.value, text)
                        entryDelegate.editMode = false
                    }
                    Keys.onEscapePressed: {
                        entryData.text = styleData.value
                                ? (!styleData.value.exists())
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
            MenuItem{
                text: "Add As Runnable"
                onTriggered: {
                    view.contextDelegate.addRunnable()
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
                text: "Close Project"
                onTriggered: {
                    lk.layers.workspace.commands.execute('window.workspace.project.close')
                }
            }
            MenuItem{
                text: "New Document"
                onTriggered: {
                    var fe = project.fileModel.addTemporaryFile()
                    lk.layers.workspace.project.openFile(fe.path, ProjectDocument.Edit)
                }
            }

            MenuItem{
                text: "New Runnable"
                onTriggered: {
                    var fe = project.fileModel.addTemporaryFile()
                    lk.layers.workspace.project.openFile(fe.path, ProjectDocument.Edit)
                    project.openRunnable(fe.path, [fe.path])
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

