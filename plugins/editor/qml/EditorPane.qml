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

import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import editor.private 1.0
import workspace 1.0
import base 1.0

Pane{
    id : root

    property alias internalActiveFocus : editor.internalActiveFocus
    property alias internalFocus: editor.internalFocus
    onInternalActiveFocusChanged: {
        if ( panes.activePane !== root ){
            panes.activateItem(textEdit, root)
        }
    }

    property alias editor: editor
    property alias document: editor.document

    property var document: null
    onDocumentChanged: {
        paneState = { document : document }
    }

    property alias documentHandler: editor.documentHandler
    property alias textEdit: editor.textEdit
    property alias loading: loadingAnimation.visible
    property var panes: null

    property bool codeOnly: !(documentHandler.codeHandler && documentHandler.codeHandler.numberOfConnections !== 0)

    onCodeOnlyChanged: {
        if (codeOnly){
            editor.addRootButton.visible = false
        }
    }

    paneType: 'editor'
    paneState : { return {} }
    paneInitialize : function(s){
        if ( s.document ){
            if (typeof s.document === 'string' || s.document instanceof String){
                var d = project.documentModel.documentByPathHash(s.document)
                document = d
            } else {
                document = s.document
            }
        }
    }

    paneHelp : function(){
        if ( documentHandler && documentHandler.has(DocumentHandler.LanguageHelp) ){
            var helpPath = ''
            if ( documentHandler.completionModel.isEnabled ){
                helpPath = qmlSuggestionBox.getDocumentation()
            } else {
                helpPath = documentHandler.codeHandler.help(textEdit.cursorPosition)
            }

            if ( helpPath.length > 0 ){
                var docItem = lk.layers.workspace.documentation.load(helpPath)
                if ( docItem ){
                    var docPane = mainSplit.findPaneByType('documentation')
                    if ( !docPane ){
                        var storeWidth = root.width
                        docPane = root.panes.createPane('documentation', {}, [root.width, root.height])

                        var index = root.parentSplitterIndex()
                        root.panes.splitPaneHorizontallyWith(root.parentSplitter, index, docPane)

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

    property Theme currentTheme : lk.layers.workspace.themes.current

    property color topColor: currentTheme ? currentTheme.paneTopBackground : 'black'
    property color lineSurfaceColor: currentTheme ? currentTheme.panebackgroundOverlay : 'black'
    property color lineInfoColor:  currentTheme ? currentTheme.paneTopbackgroundOverlay : 'black'
    property color optionsColor: currentTheme ? currentTheme.paneTopBackground : 'black'

    color : lk.layers.workspace.themes.current.paneBackground
    clip : true

    objectName: "editor"

    function closeDocument(){ editor.closeDocument() }
    function save(){ editor.save()}
    function saveAs(){ editor.saveAs() }
    function closeDocumentAction(){ editor.closeDocumentAction() }
    function assistCompletion(){ editor.assistCompletion() }
    function getCursorRectangle(){ return editor.getCursorRectangle() }
    function cursorWindowCoords(){
        return editor.cursorWindowCoords(root)
    }

    LoadingAnimation{
        id: loadingAnimation
        visible: false
        x: parent.width/2 - width/2
        y: parent.height/2 - height/2
        z: 1000
        size: 8
    }

    Rectangle{
        visible: loadingAnimation.visible
        anchors.fill: parent
        anchors.topMargin: 30
        color: "#030609"
        opacity: loadingAnimation.visible ? 0.95 : 0
        Behavior on opacity{ NumberAnimation{ duration: 250} }
        z: 900

        MouseArea{
            anchors.fill: parent
            onClicked: mouse.accepted = true;
            onPressed: mouse.accepted = true;
            onReleased: mouse.accepted = true;
            onDoubleClicked: mouse.accepted = true;
            onPositionChanged: mouse.accepted = true;
            onPressAndHold: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

    }

    function startLoadingMode(){
        loadingAnimation.visible = true
    }

    function stopLoadingMode(){
        loadingAnimation.visible = false
    }

    function hasActiveEditor(){
        return root.panes.activePane.objectName === 'editor'
    }

    function toggleSize(){
        if ( root.width < parent.width / 2)
            root.width = parent.width - parent.width / 4
        else if ( root.width === parent.width / 2 )
            root.width = parent.width / 4
        else
            root.width = parent.width / 2
    }

    function toggleComment(){
        editor.toggleComment()
    }

    Rectangle{
        anchors.left: parent.left
        anchors.top: parent.top

        width: parent.width
        height: 30

        color : root.topColor

        PaneDragItem{
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
                    var filename = root.document.file.name
                    if ( !root.document.file.exists() ){
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

        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 110
            width: 30
            height: parent.height
            color: root.optionsColor

            Image{
                id : codeDesignToggle
                anchors.centerIn: parent
                source : "qrc:/images/switch-to-" + (codeOnly? "design": "source") + ".png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {

                    lk.layers.workspace.panes.activePane = root

                    if (!codeOnly)
                    {
                        if (documentHandler.codeHandler)
                        {
                            // unfortunate naming, but this actually disables loading
                            if (loading)
                                lk.layers.workspace.commands.execute("editqml.shape_all")
                            documentHandler.codeHandler.removeAllEditingFragments()
                            editor.importsShaped = false
                            editor.rootShaped = false
                        }
                    } else {
                        lk.layers.workspace.commands.execute("editqml.shape_all")
                        // shape all
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
                onClicked: root.closeDocument()
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

    PaneMenu{
        id: editorAddRemoveMenu
        visible: false
        anchors.right: root.right
        anchors.topMargin: 30
        anchors.top: root.top

        style: root.currentTheme.popupMenuStyle

        PaneMenuItem{
            text: qsTr('Split Horizontally')
            onClicked: {
                editorAddRemoveMenu.visible = false
                var clone = root.paneClone()
                var index = root.parentSplitterIndex()
                root.panes.splitPaneHorizontallyWith(root.parentSplitter, index, clone)
            }
        }
        PaneMenuItem{
            text: qsTr('Split Vertically')
            onClicked: {
                editorAddRemoveMenu.visible = false
                var clone = root.paneClone()
                var index = root.parentSplitterIndex()
                root.panes.splitPaneVerticallyWith(root.parentSplitter, index, clone)
            }
        }
        PaneMenuItem{
            text: qsTr("Move to New Window")
            onClicked: {
                editorAddRemoveMenu.visible = false
                root.panes.movePaneToNewWindow(root)
            }
        }
        PaneMenuItem{
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
        anchors.topMargin: 30
        color: root.color
        lineSurfaceColor: root.lineSurfaceColor
    }

}

