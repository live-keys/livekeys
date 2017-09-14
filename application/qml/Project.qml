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

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Cv 1.0

Rectangle{
    id: root
    color : "#0b1924"
    gradient: Gradient{
        GradientStop { position: 0.0;  color: "#061119" }
        GradientStop { position: 0.01; color: "#050e16" }
    }

    signal addEntry(ProjectEntry parentEntry, bool isFile)
    signal openEntry(ProjectEntry entry, bool monitor)
    signal editEntry(ProjectEntry entry)
    signal removeEntry(ProjectEntry entry, bool isFile)
    signal moveEntry(ProjectEntry entry, ProjectEntry newParent)
    signal renameEntry(ProjectEntry entry, string newName)
    signal closeProject()

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
                                if ( project.inFocus ){
                                    if ( project.inFocus.file === styleData.value )
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
                            } else if ( styleData.value.path === project.path ){
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
                    root.closeProject()
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

