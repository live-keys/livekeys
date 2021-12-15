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
import fs 1.0 as Fs

Pane{
    id: root
    color : currentTheme ? currentTheme.paneBackground : 'black'
    objectName: "projectFileSystem"

    property QtObject panes: null
    paneFocusItem : root
    paneType: 'projectFileSystem'
    paneState : { return {} }

    property Theme currentTheme : lk.layers.workspace.themes.current

    property QtObject delegateOperations: QtObject{}

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
        model: lk.layers.workspace.project.fileModel
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
                lk.layers.workspace.wizards.switchActive(Fs.UrlInfo.urlFromLocalFile(styleData.value.path))
            }
            function addRunnable(){
                lk.layers.workspace.project.openRunnable(styleData.value.path, [styleData.value.path])
            }
            function openFile(){
                lk.layers.workspace.environment.openFile(
                    styleData.value.path, ProjectDocument.Edit
                )
            }
            function monitorFile(){
                lk.layers.workspace.environment.openFile(
                    styleData.value.path, ProjectDocument.Monitor
                )
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
                            if (lk.layers.workspace.project.active && styleData.value.path === lk.layers.workspace.project.active.path)
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
                            ? lk.layers.workspace.project.fileModel.printableName(styleData.value.name)
                            : ''
                    }
                    font.family: 'Open Sans, Arial, sans-serif'
                    font.pixelSize: 12
                    property int type : {
                        if (styleData.value){
                            if ( styleData.value.document ){
                                var ap = root.panes.activePane
                                if ( ap && ap.objectName === 'editor' && ap.document && ap.document.path === styleData.value.path ){
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
                        lk.layers.workspace.fileSystem.renamePath(styleData.value.path, text)
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
                                lk.layers.workspace.panes.activateItem(entryDelegate, root)
                                lk.layers.workspace.panes.openContextMenu(entryDelegate, root)
//                                fileContextMenu.popup()
                            } else if ( styleData.value.path === lk.layers.workspace.project.rootPath ){
                                view.setContextDelegate(entryDelegate)
                                lk.layers.workspace.panes.activateItem(entryDelegate, root)
                                lk.layers.workspace.panes.openContextMenu(entryDelegate, root)
//                                projectContextMenu.popup()
                            } else {
                                view.setContextDelegate(entryDelegate)
                                lk.layers.workspace.panes.activateItem(entryDelegate, root)
                                lk.layers.workspace.panes.openContextMenu(entryDelegate, root)
//                                dirContextMenu.popup()
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
                            lk.layers.workspace.environment.fileSystem.openFile(
                                Fs.UrlInfo.urlFromLocalFile(styleData.value.path), ProjectDocument.EditIfNotOpen
                            )
                        else {
                            var modelIndex = lk.layers.workspace.project.fileModel.itemIndex(styleData.value)
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
                var item = lk.layers.workspace.project.fileModel.itemAt(index)
                if ( item && item !== view.dragEntry && !item.isFile )
                    view.dropEntry = item
            }
            onDropped: {
                if ( view.dropEntry !== null ){
                    lk.layers.workspace.wizards.movePath(view.dragEntry.path, view.dropEntry.path)
                    view.dragEntry = null
                    view.dropEntry = null
                }
            }
        }
        Connections{
            target: lk.layers.workspace.project.fileModel
            function onProjectNodeChanged(index){
                view.expand(index)
            }
        }
    }
}

