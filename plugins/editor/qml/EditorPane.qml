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

import QtQuick 2.4
import editor 1.0
import editor.private 1.0
import workspace 1.0 as Workspace
import base 1.0
import fs 1.0 as Fs
import visual.input 1.0 as Input
import visual.shapes 1.0

Workspace.Pane{
    id : root
    objectName: "editor"

    color : lk.layers.workspace.themes.current.paneBackground
    clip : true
    height: parent ? parent.height : 0
    width: 400

    property alias editor: editor
    property alias document: editor.document
    property alias code: editor.code
    property alias textEdit: editor.textEdit
    property alias internalActiveFocus : editor.internalActiveFocus
    property alias internalFocus: editor.internalFocus
    property var runTrigger: editor.code.language ? editor.code.language.runTrigger : null

    onInternalActiveFocusChanged: {
        if ( internalActiveFocus && panes.activePane !== root ){
            panes.activateItem(textEdit, root)
        }
    }

    property var paletteFunctions: lk.layers.workspace.extensions.editqml.paletteFunctions
    property var panes: null

    property bool codeOnly: !(code.language && code.language.editContainer.editCount !== 0)

    property Theme currentTheme : lk.layers.workspace.themes.current
    property color topColor: currentTheme ? currentTheme.paneTopBackground : 'black'
    property color lineSurfaceColor: currentTheme ? currentTheme.panebackgroundOverlay : 'black'
    property color lineInfoColor:  currentTheme ? currentTheme.paneTopbackgroundOverlay : 'black'
    property color optionsColor: currentTheme ? currentTheme.paneTopBackground : 'black'

    paneType: 'editor'
    paneState : { return {} }
    paneInitialize : function(s){
        if ( s.document ){
            if (typeof s.document === 'string' || s.document instanceof String){
                if ( s.document.startsWith('%project%') ){
                    var documentPath = s.document.replace('%project%', lk.layers.workspace.project.dir())
                    var format = lk.layers.workspace.fileFormats.find(documentPath)
                    var d = lk.layers.workspace.project.openFile(documentPath, {type: 'text', format: format})
                    if ( d )
                        document = d
                } else {
                    var d = lk.layers.workspace.project.documentModel.documentByPathHash(s.document)
                    document = d
                }
            } else {
                document = s.document
            }
        }
    }

    paneCleared: function(){
        for ( var i = 0; i < extraOptions.children.length; ++i ){
            extraOptions.children[i].destroy()
        }
        extraOptions.children = []
        root.destroy()
    }

    paneHelp : function(){
        if ( code && code.has(DocumentHandler.LanguageHelp) ){
            var helpPath = ''
            if ( code.completionModel.isEnabled && editor.__qmlSuggestionBox){
                helpPath = editor.__qmlSuggestionBox.getDocumentation()
            } else {
                helpPath = code.language.help(textEdit.cursorPosition)
            }

            if ( helpPath.length > 0 ){
                var docItem = lk.layers.workspace.documentation.load(helpPath)
                if ( docItem ){
                    var docPane = mainSplit.findPaneByType('documentation')
                    if ( !docPane ){
                        var storeWidth = root.width
                        docPane = root.panes.createPane('documentation', {}, [root.width, root.height])

                        var index = root.parentSplitViewIndex()
                        root.panes.splitPaneHorizontallyWith(root.parentSplitView, index, docPane)

                        root.width = storeWidth
                        docPane.width = storeWidth
                    }

                    docPane.paneInitialize(docItem)
                }
            }

        }

        return null
    }

    paneFocusItem : textEdit
    paneClone: function(){
        return root.panes.createPane('editor', paneState, [root.width, root.height])
    }

    Rectangle{
        anchors.left: parent.left
        anchors.top: parent.top

        width: parent.width
        height: 30

        color : root.topColor

        Workspace.PaneDragItem{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            onDragStarted: root.panes.__dragStarted(root)
            onDragFinished: root.panes.__dragFinished(root)
            display: titleText.text
        }

        Text{
            id: titleText
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#808691"
            text: {
                if ( root.document ){
                    var filename = root.document.fileName()
                    if ( !root.document.isOnDisk() ){
                        var findex = filename.substring(2)
                        filename = 'untitled' + (findex === '0' ? '' : findex)
                    }
                    if ( root.document.isDirty )
                        filename += '*'
                    return filename;
                } else {
                    return ''
                }
            }
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            font.weight: Font.Normal
            elide: Text.ElideRight
            width: Math.max(20, parent.width - 200)
        }

        Rectangle{
            color: 'transparent'
            width: 24
            height: parent.height
            anchors.right: parent.right
            anchors.rightMargin: 140
            visible : root.document !== null

            Image{
                id : switchImage
                anchors.centerIn: parent
                source : "qrc:/images/switch-file.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    lk.layers.workspace.commands.execute('window.workspace.toggleNavigation')
                }
            }
        }

        Input.Button{
            id: shapeAllButton
            anchors.right: parent.right
            anchors.rightMargin: 110
            width: 30
            height: parent.height
            content: codeOnly ? root.currentTheme.buttons.editorShape : root.currentTheme.buttons.editorCode
            onClicked: {
                lk.layers.workspace.panes.activePane = root

                var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
                if (!codeOnly)
                {
                    if (code.language)
                    {
                        lk.layers.editor.environment.removeLoadingScreen(editor)

                        if (code.language.rootFragment){
                            root.paneState.layout = paletteFunctions.getEditorLayout(root)
                        } else {
                            root.paneState.layout = null
                        }
                        code.language.editContainer.clearAllFragments()
                        code.language.importsFragment = null
                        code.language.rootFragment = null
                    }
                } else {
                    if (root.paneState.layout){
                        paletteFunctions.expandLayout(root.editor, root.paneState.layout)
                    } else {
                        // shape all
                        lk.layers.workspace.extensions.editqml.shapeAll(
                            root.editor,
                            function(ef){
                                if ( ef && ef.visualParent.expand)
                                    ef.visualParent.expand()
                                lk.layers.editor.environment.removeLoadingScreen(editor)
                            }
                        )
                    }
                }
            }
        }

        Rectangle{
            color: 'transparent'
            width: 30
            height: parent.height
            anchors.right: parent.right
            anchors.rightMargin: 85
            visible : root.document !== null
            Text{
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 16
                font.weight: Font.Light
                text: 'x'
                color: "#808691"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 3
            }
            MouseArea{
                anchors.fill: parent
                onClicked: lk.layers.workspace.wizards.closeDocument(editor.document)
            }
        }

        Rectangle{
            width: 50
            height: parent.height
            anchors.right: parent.right
            anchors.rightMargin: 31
            visible : root.document !== null
            color: root.lineInfoColor

            property bool lineAndColumn : true

            Text{
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 11
                text: parent.lineAndColumn
                         ? textEdit.lineNumber + ", " + textEdit.columnNumber
                         : textEdit.cursorPosition
                color: "#808691"
                anchors.left: parent.left
                anchors.leftMargin: 7
                anchors.verticalCenter: parent.verticalCenter
            }
            MouseArea{
                anchors.fill: parent
                onClicked: parent.lineAndColumn = !parent.lineAndColumn
            }
        }

        Rectangle{
            anchors.right: parent.right
            width: 30
            height: parent.height
            color: root.optionsColor

            Image{
                id : paneOptions
                anchors.centerIn: parent
                source : "qrc:/images/pane-menu.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    editorAddRemoveMenu.visible = !editorAddRemoveMenu.visible
                }
            }
        }
    }

    Item{
        id: extraOptions

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 30

        width: parent.width
        height: 0
    }

    Workspace.PaneMenu{
        id: editorAddRemoveMenu
        visible: false
        anchors.right: root.right
        anchors.topMargin: 30
        anchors.top: root.top

        property bool supportsShaping: false

        style: root.currentTheme.popupMenuStyle

        Workspace.PaneMenuItem{
            text: qsTr('Split Horizontally')
            onClicked: {
                editorAddRemoveMenu.visible = false
                var clone = root.paneClone()
                root.panes.splitPaneHorizontally(root, clone)
            }
        }
        Workspace.PaneMenuItem{
            text: qsTr('Split Vertically')
            onClicked: {
                editorAddRemoveMenu.visible = false
                var clone = root.paneClone()
                root.panes.splitPaneVertically(root, clone)
            }
        }
        Workspace.PaneMenuItem{
            text: qsTr("Move to New Window")
            onClicked: {
                editorAddRemoveMenu.visible = false
                root.panes.movePaneToNewWindow(root)
            }
        }
        Workspace.PaneMenuItem{
            text: qsTr("Toggle Run Triggers")
            onClicked: {
                var found = false
                var newChildren = []
                for ( var i = 0; i < extraOptions.children.length; ++i ){
                    var c = extraOptions.children[i]
                    if ( c.objectName === 'runControl' ){
                        extraOptions.height -= c.height
                        c.destroy()
                        found = true
                    } else {
                        newChildren.push(c)
                    }
                }
                if ( found ){
                    extraOptions.children = newChildren
                } else {
                    var rc = lk.layers.editor.environment.createRunnableControl(editor, extraOptions)
                    rc.currentTheme = root.currentTheme
                    extraOptions.height += rc.height
                    extraOptions.children.push(rc)
                }
                editorAddRemoveMenu.visible = false
            }
        }

        Workspace.PaneMenuItem{
            text: qsTr("Save Layout")
            visible: editorAddRemoveMenu.supportsShaping
            onClicked: {
                editorAddRemoveMenu.visible = false

                var layout = lk.layers.workspace.extensions.editqml.paletteFunctions.getEditorLayout(editor)
                if ( !layout ){
                    lk.layers.workspace.messages.pushWarning("The document needs to be shaped before layout can be saved.", 100)
                    return
                }

                var editorFile = editor.document.path
                var layoutFile = editorFile + 'a'
                editorFile = Fs.Path.relativePath(lk.layers.workspace.project.dir(), editorFile)

                var layoutObj = {
                    file: editorFile,
                    layout: layout
                }

                var jsonStr = JSON.stringify(layoutObj)
                var file = Fs.File.open(layoutFile, Fs.File.WriteOnly)
                if ( file === null ){
                    lk.layers.workspace.messages.pushWarning("Failed to open file for writing: " + layoutFile, 100)
                    return
                }

                file.writeString(jsonStr)
                file.close()
            }
        }
        Workspace.PaneMenuItem{
            text: qsTr("Save Layout As...")
            visible: editorAddRemoveMenu.supportsShaping
            onClicked: {
                editorAddRemoveMenu.visible = false
                lk.layers.window.dialogs.saveFile({filters: ["Layout files (*.qmla)"]}, function(url){
                    var layoutFile = Fs.UrlInfo.toLocalFile(url)
                    var layout = lk.layers.workspace.extensions.editqml.paletteFunctions.convertEditorStateIntoInstructions(editor)
                    if ( !layout ){
                        lk.layers.workspace.messages.pushWarning("The document needs to be shaped before layout can be saved.", 100)
                        return
                    }

                    var editorFile = editor.document.path
                    var relativeLayoutFile = Fs.Path.relativePath(lk.layers.workspace.project.dir(), layoutFile)
                    if ( relativeLayoutFile !== layoutFile ){ // layout file is in project path
                        editorFile = Fs.Path.relativePath(lk.layers.workspace.project.dir(), editorFile)
                    }

                    var layoutObj = {
                        file: editorFile,
                        layout: layout
                    }

                    var jsonStr = JSON.stringify(layoutObj)
                    var file = Fs.File.open(layoutFile, Fs.File.WriteOnly)
                    if ( file === null ){
                        lk.layers.workspace.messages.pushWarning("Failed to open file for writing: " + layoutFile, 100)
                        return
                    }

                    file.writeString(jsonStr)
                    file.close()
                })
            }
        }
        Workspace.PaneMenuItem{
            text: qsTr("Open Layout")
            visible: editorAddRemoveMenu.supportsShaping
            onClicked: {
                editorAddRemoveMenu.visible = false
                lk.layers.window.dialogs.openFile({filters: ["Layout files (*.qmla)"]}, function(url){
                    var layoutFile = Fs.UrlInfo.toLocalFile(url)
                    var file = Fs.File.open(layoutFile, Fs.File.ReadOnly)
                    var contents = file.readAll()
                    var contentsStr = contents.toString()

                    var layoutObj = JSON.parse(contentsStr)
                    //TODO: Check if parse goes wrong

                    var editorFile = layoutObj.file
                    if ( Fs.Path.isRelative(editorFile ) )
                        editorFile = lk.layers.workspace.project.path(editorFile)

                    var layout = layoutObj.layout

                    if ( editorFile !== editor.document.path ){
                        lk.layers.workspace.messages.pushWarning("Layout file is not compatible with the file in the editor.", 100)
                        return
                    }

                    var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
                    paletteFunctions.expandLayout(root, layout)
                })
            }
        }
        Workspace.PaneMenuItem{
            text: qsTr("Remove Pane")
            onClicked: {
                editorAddRemoveMenu.visible = false
                root.panes.clearPane(root)
            }
        }
    }

    Editor{
        id: editor
        anchors.fill: parent
        anchors.topMargin: 30 + extraOptions.height
        style: root.currentTheme.editorStyle
        lineSurfaceVisible: !(code && code.language && code.language.importsFragment && code.language.rootFragment)
        getGlobalPosition: function(){
            var g = root.mapGlobalPosition()
            g.y += extraOptions.height
            return g
        }

        onKeyPress: {
            var command = lk.layers.workspace.keymap.locateCommand(event.key, event.modifiers)
            if ( command !== '' ){
                lk.layers.workspace.commands.execute(command)
                event.accepted = true
                return
            }
        }

        onRightClicked: {
            if ( lk.layers.workspace ){
                lk.layers.workspace.panes.openContextMenu(editor, root)
            }
        }

        onDocumentChanged: {
            editorAddRemoveMenu.supportsShaping = code.has(DocumentHandler.LanguageLayout)
            paneState = { document : document }
        }
    }

}

