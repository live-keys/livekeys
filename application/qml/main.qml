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

import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import base 1.0
import editor 1.0
import editor.private 1.0
import live 1.0

ApplicationWindow{
    id : root

    visible: true
    width: 1240
    minimumWidth: 640
    height: 700
    minimumHeight: 400
    color : "#293039"
    objectName: "window"

    property WindowControls controls: WindowControls{
        saveFileDialog: fileSaveDialog
        openFileDialog: fileOpenDialog
        openDirDialog: dirOpenDialog
        messageDialog: messageBox
        runSpace: runSpace
        createTimer: createTimer
        paletteBoxFactory: editorBoxFactory
        editSpace: editSpace
        activePane : null
        activeItem : null
        navEditor: null
        wasLiveCoding: false
        codingMode: 0
    }

    property bool documentsReloaded : false
    onActiveChanged: {
        if ( active ){
            project.navigationModel.requiresReindex()
            project.fileModel.rescanEntries()
            project.documentModel.rescanDocuments()
            if ( documentsReloaded && project.active ){
                createTimer.restart()
                documentsReloaded = false
            }
            editor.forceActiveFocus()
        }
    }

    Component.onCompleted: {
        livecv.commands.add(root, {
            'minimize' : [root.showMinimized, "Minimize"],
            'toggleMaximizedRuntime' : [contentWrap.toggleMaximizedRuntime, "Toggle Maximized Runtime"],
            'toggleNavigation' : [contentWrap.toggleNavigation, "Toggle Navigation"],
            'openLogInWindow' : [mainVerticalSplit.openLogInWindow, "Open Log In Window"],
            'openLogInEditor' : [mainVerticalSplit.openLogInEditor, "Open Log In Editor"],
            'toggleLog' : [mainVerticalSplit.toggleLog, "Toggle Log"],
            'toggleLogPrefix' : [logView.toggleLogPrefix, "Toggle Log Prefix"],
            'addHorizontalEditorView' : [mainVerticalSplit.addHorizontalEditor, "Add Horizontal Editor"],
            'addHorizontalFragmentEditorView': [mainVerticalSplit.addHorizontalFragmentEditor, "Add Horizontal Fragment Editor"],
            'removeHorizontalEditorView' : [mainVerticalSplit.removeHorizontalEditor, "Remove Horizontal Editor"]
        })
    }

    title: qsTr("Live CV")

    FontLoader{ id: ubuntuMonoBold;       source: "qrc:/fonts/UbuntuMono-Bold.ttf"; }
    FontLoader{ id: ubuntuMonoRegular;    source: "qrc:/fonts/UbuntuMono-Regular.ttf"; }
    FontLoader{ id: ubuntuMonoItalic;     source: "qrc:/fonts/UbuntuMono-Italic.ttf"; }
    FontLoader{ id: sourceCodeProLight;   source: "qrc:/fonts/SourceCodePro-Light.ttf"; }
    FontLoader{ id: sourceCodeProRegular; source: "qrc:/fonts/SourceCodePro-Regular.ttf"; }
    FontLoader{ id: sourceCodeProBold;    source: "qrc:/fonts/SourceCodePro-Bold.ttf"; }
    FontLoader{ id: openSansLight;        source: "qrc:/fonts/OpenSans-Light.ttf"; }
    FontLoader{ id: openSansBold;         source: "qrc:/fonts/OpenSans-LightItalic.ttf"; }
    FontLoader{ id: openSansRegular;      source: "qrc:/fonts/OpenSans-Regular.ttf"; }
    FontLoader{ id: openSansLightItalic;  source: "qrc:/fonts/OpenSans-Bold.ttf"; }

    LogWindow{
        id : logWindow
        visible : false
        onVisibleChanged: if ( !visible ) logView.visible = false
        Component.onCompleted: width = root.width
    }

    Top{
        id : header
        visible: !livecv.settings.launchMode
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: livecv.settings.launchMode ? 0 : 35
        color: "#08141d"

        property var callback : function(){}

        isTextDirty: editor.isDirty

        property string action : ""

        onNewProject: projectView.newProject()
        onOpenFile : projectView.openFile()
        onOpenProject: projectView.openProject()
        onSaveFile : projectView.focusEditor.saveAs()
        onToggleLogWindow : mainVerticalSplit.toggleLog()
        onOpenCommandsMenu: projectView.openCommandsMenu()

        onOpenSettings: {
            editor.document = project.openFile(livecv.settings.file('editor').path, ProjectDocument.Edit);
            livecv.settings.file('editor').documentOpened(editor.document)
        }

        onOpenLicense: licenseBox.visible = true
    }

    CommandsMenu {
        id: commandsMenu
        anchors.top: header.bottom
        x: 395
        z: 200
    }

    Rectangle {
        id: modeWrapper
        anchors.left: parent.left
        anchors.leftMargin: 550
        width: 220
        height: 30
        color: 'transparent'

        Item{
            anchors.left: parent.left
            anchors.leftMargin: 35
            height : parent.height
            Text{
                color :  "#969aa1"
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 12
                font.family: 'Open Sans, Arial, sans-serif'
                elide: Text.ElideRight
                width: 130
                text : {
                    if (!project.active) return "";
                    if (project.active && project.active.file){
                        if (project.active.file.name === "") return "untitled";
                        else return project.active.file.name;
                    }
                }
            }
        }

        Triangle{
            id: compileButtonShape
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            width: compileButton.containsMouse ? 8 : 7
            height: compileButton.containsMouse ? 13 : 12
            state : "Released"
            rotation: Triangle.Right

            Behavior on height{ NumberAnimation{ duration: 100 } }
            Behavior on width{ NumberAnimation{ duration: 100 } }

            states: [
                State {
                    name: "Pressed"
                    PropertyChanges { target: compileButtonShape; color: "#487db9"}
                },
                State {
                    name: "Released"
                    PropertyChanges { target: compileButtonShape; color: compileButton.containsMouse ? "#768aca" : "#bcbdc1"}
                }
            ]
            transitions: [
                Transition {
                    from: "Pressed"
                    to: "Released"
                    ColorAnimation { target: compileButtonShape; duration: 100}
                },
                Transition {
                    from: "Released"
                    to: "Pressed"
                    ColorAnimation { target: compileButtonShape; duration: 100}
                }
            ]
        }

        MouseArea{
            id : compileButton
            anchors.left: parent.left
            anchors.leftMargin: 0
            width: 50
            height: 30
            hoverEnabled: true
            onPressed: compileButtonShape.state = "Pressed"
            onReleased: compileButtonShape.state = "Released"
            onClicked: {
                if (project.active){
                    livecv.engine.createObjectAsync(
                        project.active.content,
                        livecv.windowControls().runSpace,
                        project.active.file.pathUrl(),
                        project.active,
                        true
                    );
                }
            }
        }

        Item{
            width: modeImage.width + modeImage.anchors.rightMargin + 10
            height: parent.height
            anchors.right: parent.right

            Image{
                id : modeImage
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.verticalCenter: parent.verticalCenter
                source: liveImage.source
            }

            Triangle{
                anchors.right: parent.right
                anchors.rightMargin: 7
                anchors.verticalCenter: parent.verticalCenter
                width: 9
                height: 5
                color: openStatesDropdown.containsMouse ? "#9b6804" : "#bcbdc1"
                rotation: Triangle.Bottom
            }

            MouseArea{
                id : openStatesDropdown
                anchors.fill: parent
                hoverEnabled: true
                onClicked: modeContainer.visible = !modeContainer.visible
            }
        }


        Rectangle{
            width: parent.width
            height: 1
            color: "#1a1f25"
            anchors.bottom: parent.bottom
        }

    }

    Rectangle {
        id: modeContainer
        visible: false
        anchors.right: modeWrapper.right
        anchors.top: modeWrapper.bottom
        property int buttonHeight: 30
        property int buttonWidth: 120
        opacity: visible ? 1.0 : 0
        z: 1000

        Behavior on opacity{ NumberAnimation{ duration: 200 } }


        Rectangle{
            id: liveButton
            anchors.top: parent.top
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            visible: projectView.focusEditor ? projectView.focusEditor.document === project.active : false
            color : "#03070b"
            Text {
                id: liveText
                text: qsTr("Live")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: liveArea.containsMouse ? "#969aa1" : "#5e5e5e"
            }
            Image{
                id : liveImage
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.verticalCenter: parent.verticalCenter
                source : "qrc:/images/live.png"
            }
            MouseArea{
                id : liveArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    controls.codingMode = 0
                    modeContainer.visible = false
                    modeImage.source = liveImage.source
                    modeImage.anchors.rightMargin = liveImage.anchors.rightMargin
                    livecv.engine.createObjectAsync(
                        project.active.content,
                        runSpace,
                        project.active.file.pathUrl(),
                        project.active,
                        true
                    );
                }
            }
        }

        Rectangle{
            id: onSaveButton
            anchors.top: liveButton.visible ? liveButton.bottom : parent.top
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: onSaveText
                text: qsTr("On Save")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: onSaveArea.containsMouse ? "#969aa1" : "#5e5e5e"
            }
            Image{
                id : onSaveImage
                anchors.right: parent.right
                anchors.rightMargin: 25
                anchors.verticalCenter: parent.verticalCenter
                source : "qrc:/images/onsave.png"
            }
            MouseArea{
                id : onSaveArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    controls.codingMode = 1
                    modeContainer.visible = false
                    modeImage.source = onSaveImage.source
                    modeImage.anchors.rightMargin = onSaveImage.anchors.rightMargin
                    controls.wasLiveCoding = false
                }
            }
        }

        Rectangle{
            id: disabledButton
            anchors.top: onSaveButton.bottom
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: disabledText
                text: qsTr("Disabled")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: disabledArea.containsMouse ? "#969aa1" : "#5e5e5e"
            }
            Image{
                id : disabledImage
                anchors.right: parent.right
                anchors.rightMargin: 28
                anchors.verticalCenter: parent.verticalCenter
                source : "qrc:/images/disabled.png"
            }
            MouseArea{
                id : disabledArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    controls.codingMode = 2
                    modeContainer.visible = false
                    modeImage.source = disabledImage.source
                    modeImage.anchors.rightMargin = disabledImage.anchors.rightMargin
                    controls.wasLiveCoding = false
                }
            }
        }
    }

    FileDialog {
        id: fileOpenDialog
        title: "Please choose a file"
        nameFilters: [ "Qml files (*.qml)", "All files (*)" ]
        selectExisting : true
        visible : script.environment.os.platform === 'linux' ? true : false // fixes a display bug in some linux distributions
        onAccepted: {
            if ( project.rootPath === '' ){
                project.openProject(fileOpenDialog.fileUrl)
                if ( project.active ) {
                    projectView.focusEditor.document = project.active
                    projectView.maintainCodingMode()
                }
            } else if ( project.isFileInProject(fileOpenDialog.fileUrl ) ) {
                var doc = project.openFile(fileOpenDialog.fileUrl, ProjectDocument.Edit)
                if ( doc ) {
                    projectView.focusEditor.document = doc
                    projectView.maintainCodingMode()
                }
            } else {
                var fileUrl = fileOpenDialog.fileUrl
                messageBox.show(
                    'File is outside project scope. Would you like to open it as a new project?',
                {
                    button1Name : 'Open as project',
                    button1Function : function(){
                        var projectUrl = fileUrl
                        projectView.closeProject(
                            function(){
                                project.openProject(projectUrl)
                                projectView.focusEditor.document = project.active
                                projectView.maintainCodingMode()
                            }
                        )
                        messageBox.close()
                    },
                    button3Name : 'Cancel',
                    button3Function : function(){
                        messageBox.close()
                    },
                    returnPressed : function(){
                        var projectUrl = fileUrl
                        header.closeProject(
                            function(){
                                project.openProject(projectUrl)
                                projectView.focusEditor.document = project.active
                                projectView.maintainCodingMode()
                            }
                        )
                        messageBox.close()
                    }
                })
            }
        }
        Component.onCompleted: {
            if ( script.environment.os.platform === 'darwin' )
                folder = '~' // fixes a warning message that the path was constructed with an empty filename
            visible = false
            close()
        }
    }

    FileDialog {
        id: dirOpenDialog
        title: "Please choose a directory"
        selectExisting : true
        selectMultiple : false
        selectFolder : true

        visible : script.environment.os.platform === 'linux' ? true : false /// fixes a display bug in some linux distributions
        onAccepted: {
            project.openProject(dirOpenDialog.fileUrl)
            if ( project.active ) {
                projectView.focusEditor.document = project.active
                projectView.maintainCodingMode()
            }
        }
        Component.onCompleted: {
            if ( script.environment.os.platform === 'darwin' )
                folder = '~' // fixes a warning message that the path was constructed with an empty filename
            visible = false
            close()
        }
    }

    FileDialog{
        id : fileSaveDialog
        title: "Please choose a file"
        nameFilters: ["Qml files (*.qml)", "All files (*)"]
        selectExisting : false
        visible : script.environment.os.platform === 'linux' ? true : false /// fixes a display bug in some linux distributions

        property var callback: null

        onAccepted: {
            if ( callback ){
                callback()
                callback = null
            } else if ( projectView.focusEditor.document ){
                var editordoc = projectView.focusEditor.document
                if ( !editordoc.saveAs(fileSaveDialog.fileUrl) ){
                    messageBox.show(
                        'Failed to save file to: ' + fileSaveDialog.fileUrl,
                        {
                            button3Name : 'Ok',
                            button3Function : function(){ messageBox.close(); }
                        }
                    )
                    return;
                }

                if ( !project.isDirProject() ){
                    project.openProject(fileSaveDialog.fileUrl)
                    projectView.focusEditor.document = project.active
                    projectView.maintainCodingMode()
                } else if ( project.isFileInProject(fileSaveDialog.fileUrl ) ){
                    projectView.focusEditor.document = project.openFile(fileSaveDialog.fileUrl, ProjectDocument.Edit)
                    projectView.maintainCodingMode()
                    if ( project.active && project.active !== projectView.focusEditor.document && controls.codingMode !== 2 /* compiling isn't disabled */){
                        engine.createObjectAsync(
                            project.active.content,
                            runSpace,
                            project.active.file.pathUrl(),
                            project.active,
                            true
                        );
                    }
                } else {
                    var fileUrl = fileSaveDialog.fileUrl
                    messageBox.show(
                        'File is outside project scope. Would you like to open it as a new project?',
                    {
                        button1Name : 'Open as project',
                        button1Function : function(){
                            var projectUrl = fileUrl
                            projectView.closeProject(
                                function(){
                                    project.openProject(projectUrl);
                                    projectView.focusEditor.document = project.active
                                    projectView.maintainCodingMode()
                                }
                            )
                            messageBox.close()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(){
                            messageBox.close()
                        },
                        returnPressed : function(){
                            var projectUrl = fileUrl
                            projectView.closeProject(
                                function(){
                                    project.openProject(projectUrl)
                                    projectView.focusEditor.document = project.active
                                    projectView.maintainCodingMode()
                                }
                            )
                            messageBox.close()
                        }
                    })
                }
            }
        }
        onRejected:{
            fileSaveDialog.callback = null
        }
        Component.onCompleted: {
            if ( script.environment.os.platform === 'darwin' )
                folder = '~' // fixes a warning message that the path was constructed with an empty filename
            visible: false
            close()
        }
    }

    Component{
        id: editorFactory

        Editor{
            id: editorComponent
            height: parent ? parent.height : 0
            width: 400
            windowControls: controls
            onInternalActiveFocusChanged: if ( internalActiveFocus ) {
                root.controls.setActiveItem(editorComponent.textEdit, editorComponent)
                projectView.setFocusEditor(editorComponent)
            }

            Component.onCompleted: {
                projectView.setFocusEditor(editorComponent)
                if ( project.active ){
                    editorComponent.document = project.active
                }
                forceFocus()
            }
        }
    }

    Component{
        id: fragmentDocumentFactory
        DocumentFragment{}
    }

    Item{
        id: editSpace

        property QtObject placement : QtObject{
            property int top: 0
            property int right: 1
            property int bottom: 2
            property int left: 3
        }

        property alias content : contentWrap

        function createEmptyEditorBox(parent){
            return editorBoxFactory.createObject(parent ? parent : contentWrap)
        }

        function createEditorBox(child, aroundRect, editorPosition, relativePlacement){
            var eb = editorBoxFactory.createObject(contentWrap)
            eb.setChild(child, aroundRect, editorPosition, relativePlacement)
            return eb;
        }
    }

    Component{
        id: editorBoxFactory

        Rectangle{
            id: editorBoxComponent
            x: 0
            y: visible ? 0 : 20
            width: child ? child.width + 10: 0
            height: child ? child.height + 10 : 0
            visible: false
            opacity: visible ? 1 : 0
            objectName: "editorBox"

            property Item child : null
            children: child ? [child] : []

            Behavior on opacity{
                NumberAnimation{ duration : 200; easing.type: Easing.OutCubic; }
            }
            Behavior on y{
                id : moveYBehavior
                NumberAnimation{ duration : 200; easing.type: Easing.OutCubic; }
            }
            Behavior on x{
                id: moveXBehavior
                NumberAnimation{ duration: 200; easing.type: Easing.OutCubic; }
            }

            function disableMoveBehavior(){
                moveXBehavior.enabled = false
                moveYBehavior.enabled = false
            }

            function updatePlacement(aroundRectangle, editorPosition, relativePlacement){
                if ( relativePlacement === 0 || relativePlacement === 2 ){
                    visible = false
                    moveXBehavior.enabled = false
                    moveYBehavior.enabled = false

                    var startY = editorPosition.y + aroundRectangle.y + 38

                    editorBoxComponent.x = editorPosition.x + aroundRectangle.x + 7
                    editorBoxComponent.y = startY - aroundRectangle.y / 2

                    visible = true

                    var newX = editorBoxComponent.x - editorBoxComponent.width / 2
                    var maxX = contentWrap.width - editorBoxComponent.width
                    editorBoxComponent.x = newX < 0 ? 0 : (newX > maxX ? maxX : newX)

                    var upY = startY - editorBoxComponent.height
                    var downY = startY + aroundRectangle.height + 5

                    if ( relativePlacement === 0 ){ // top placement
                        editorBoxComponent.y = upY > 0 ? upY : downY
                    } else { // bottom placement
                        editorBoxComponent.y = downY + editorBoxComponent.height < contentWrap.height ? downY : upY
                    }
                    moveYBehavior.enabled = true

                } else {
                    visible = false
                    moveXBehavior.enabled = false
                    moveYBehavior.enabled = false

                    var startX = editorPosition.x + aroundRectangle.x + 5

                    editorBoxComponent.x = startX - aroundRectangle.x / 2
                    editorBoxComponent.y = editorPosition.y + aroundRectangle.y + 38

                    visible = true

                    var newY = editorBoxComponent.y - editorBoxComponent.height / 2
                    var maxY = contentWrap.height - editorBoxComponent.height
                    editorBoxComponent.y = newY < 0 ? 0 : (newY > maxY ? maxY : newY)

                    var upX = startX - editorBoxComponent.width
                    var downX = startX + aroundRectangle.width + 5

                    if ( relativePlacement === 1 ){ // right placement
                        editorBoxComponent.x = upX > 0 ? upX : downX
                    } else { // left placement
                        editorBoxComponent.x = downX + editorBoxComponent.width < contentWrap.width ? downX : upX
                    }
                    moveXBehavior.enabled = true
                }
            }

            function setChild(item, aroundRectangle, editorPosition, relativePlacement){
                if ( relativePlacement === 0 || relativePlacement === 2 ){
                    moveXBehavior.enabled = false
                    moveYBehavior.enabled = false

                    var startY = editorPosition.y + aroundRectangle.y + 38

                    editorBoxComponent.x = editorPosition.x + aroundRectangle.x + 7
                    editorBoxComponent.y = startY - aroundRectangle.y / 2

                    editorBoxComponent.child = item

                    moveYBehavior.enabled = true
                    visible = true

                    var newX = editorBoxComponent.x - editorBoxComponent.width / 2
                    var maxX = contentWrap.width - editorBoxComponent.width
                    editorBoxComponent.x = newX < 0 ? 0 : (newX > maxX ? maxX : newX)

                    var upY = startY - editorBoxComponent.height
                    var downY = startY + aroundRectangle.height + 5

                    // console.log("upY", upY)
                    // console.log("downY", downY)

                    if ( relativePlacement === 0 ){ // top placement
                        editorBoxComponent.y = upY > 0 ? upY : downY
                    } else { // bottom placement
                        editorBoxComponent.y = downY + editorBoxComponent.height < contentWrap.height ? downY : upY
                    }

                } else {
                    moveXBehavior.enabled = false
                    moveYBehavior.enabled = false

                    var startX = editorPosition.x + aroundRectangle.x + 5

                    editorBoxComponent.x = startX - aroundRectangle.x / 2
                    editorBoxComponent.y = editorPosition.y + aroundRectangle.y + 38

                    editorBoxComponent.child = item

                    moveXBehavior.enabled = true
                    visible = true

                    var newY = editorBoxComponent.y - editorBoxComponent.height / 2
                    var maxY = contentWrap.height - editorBoxComponent.height
                    editorBoxComponent.y = newY < 0 ? 0 : (newY > maxY ? maxY : newY)

                    var upX = startX - editorBoxComponent.width
                    var downX = startX + aroundRectangle.width + 5

                    if ( relativePlacement === 1 ){ // right placement
                        editorBoxComponent.x = upX > 0 ? upX : downX
                    } else { // left placement
                        editorBoxComponent.x = downX + editorBoxComponent.width < contentWrap.width ? downX : upX
                    }
                }
            }
        }
    }

    SplitView{
        id: mainVerticalSplit
        anchors.top : header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height : parent.height - header.height
        orientation: Qt.Vertical

        handleDelegate: Rectangle{
            implicitWidth: 1
            implicitHeight: 1
            color: "#0e1924"
        }

        function addHorizontalEditor(){
            var editorObject = editorFactory.createObject(0)
            editorObject.height = mainHorizontalSplit.height

            var projectViewWidth = projectView.width
            var viewerWidth = viewer.width

            var editorWidths = []
            for ( var i = 0; i < mainHorizontalSplit.editors.length; ++i ){
                editorWidths.push(mainHorizontalSplit.editors[i].width)
            }

            mainHorizontalSplit.removeItem(viewer)
            mainHorizontalSplit.addItem(editorObject)
            mainHorizontalSplit.addItem(viewer)

            projectView.width = projectViewWidth
            editorObject.width = 400
            viewer.width = viewer.width - editorObject.width

            for ( var i = 0; i < editorWidths.length; ++i ){
                mainHorizontalSplit.editors[i].width = editorWidths[i]
            }
        }

        function addHorizontalFragmentEditor(){
            if ( !projectView.focusEditor || !projectView.focusEditor.document ){
                addHorizontalEditor();
                return
            }

            var cursorBlock = projectView.focusEditor.getCursorFragment()
            if ( cursorBlock.start === cursorBlock.end ){
                addHorizontalEditor()
                return
            }

            var editorObject = editorFactory.createObject(0)
            editorObject.fragmentStart = cursorBlock.start + 1
            editorObject.fragmentEnd = cursorBlock.end + 1
            editorObject.height = mainHorizontalSplit.height

            var projectViewWidth = projectView.width
            var viewerWidth = viewer.width

            var editorWidths = []
            for ( var i = 0; i < mainHorizontalSplit.editors.length; ++i ){
                editorWidths.push(mainHorizontalSplit.editors[i].width)
            }

            mainHorizontalSplit.removeItem(viewer)
            mainHorizontalSplit.addItem(editorObject)
            mainHorizontalSplit.addItem(viewer)

            projectView.width = projectViewWidth
            editorObject.width = 400
            viewer.width = viewer.width - editorObject.width

            for ( var i = 0; i < editorWidths.length; ++i ){
                mainHorizontalSplit.editors[i].width = editorWidths[i]
            }
        }

        function removeHorizontalEditor(){
            mainHorizontalSplit.removeItem(projectView.focusEditor)
        }

        function openLogInWindow(){
            var logItem = logView
            mainVerticalSplit.removeItem(logView)

            logItem.x = 0
            logItem.y = 0
            logItem.width = Qt.binding(function(){ return logWindow.contentWidth; })
            logItem.height = Qt.binding(function(){ return logWindow.contentHeight; })
            logItem.isInWindow = true

            logWindow.content = logItem
            logWindow.show()
        }

        function openLogInEditor(){
            var logItem = logWindow.content
            logWindow.content = null
            logWindow.close()

            logItem.isInWindow = false
            logItem.visible = true
            mainVerticalSplit.addItem(logItem)
            contentWrap.height = contentWrap.height - 200
        }

        function toggleLog(){
            if ( logView.isInWindow ){
                if ( logView.visible ){
                    logWindow.close()
                    logView.visible = false
                } else {
                    header.isLogWindowDirty = false
                    logView.visible = true
                    logWindow.show()
                }
            } else {
                if ( logView.visible ){
                    logView.visible = false
                    mainVerticalSplit.removeItem(logView)
                } else {
                    header.isLogWindowDirty = false
                    logView.visible = true
                    mainVerticalSplit.addItem(logView)
                    contentWrap.height = contentWrap.height - 200
                }
            }
        }

        Rectangle{
            id : contentWrap
            width: parent.width
            height: parent.height - 200

            property var palettes: []

            property var toggledItems: []
            function toggleMaximizedRuntime(){
                if ( toggledItems.length === 0 ){
                    toggledItems = [projectView.width, editor.width]
                    projectView.width = 0
                    editor.width = 0
                } else {
                    projectView.width = toggledItems[0]
                    editor.width = toggledItems[1]
                    toggledItems = []
                }
            }

            function toggleNavigation(){
                projectNavigation.visible = !projectNavigation.visible
            }

            SplitView{
                id: mainHorizontalSplit
                anchors.fill: parent
                orientation: Qt.Horizontal
                handleDelegate: Rectangle{
                    implicitWidth: 1
                    implicitHeight: 1
                    color: "#060d13"
                }

                property var editors: [editor]

                Project{
                    id: projectView
                    height: parent.height
                    width: 240
                    visible : !livecv.settings.launchMode
                    windowControls: controls
                }

                Editor{
                    id: editor
                    height: parent.height
                    width: 400
                    visible : !livecv.settings.launchMode
                    windowControls: controls
                    onInternalActiveFocusChanged: if ( internalFocus ) {
                        root.controls.setActiveItem(editor.textEdit, editor)
                        projectView.setFocusEditor(editor)
                    }

                    Component.onCompleted: {
                        projectView.setFocusEditor(editor)
                        if ( project.active ){
                            editor.document = project.active
                        }
                        forceFocus()
                    }
                }

                Rectangle{
                    id : viewer
                    height : parent.height
                    objectName: "viewer"

                    color : "#000509"

                    Item{
                        id: runSpace
                        anchors.fill: parent
                        property string program: editor.text
                        property variant item

                        Timer{
                            id: createTimer
                            interval: 1000
                            running: true
                            repeat : false
                            onTriggered: {
                                if (project.active === project.inFocus && project.active && controls.codingMode === 0 /* live compile */){
                                    livecv.engine.createObjectAsync(
                                        runSpace.program,
                                        runSpace,
                                        project.active.file.pathUrl(),
                                        project.active
                                    );
                                } else if ( project.active && controls.codingMode === 0 /* live compile */){
                                    livecv.engine.createObjectAsync(
                                        project.active.content,
                                        runSpace,
                                        project.active.file.pathUrl(),
                                        project.active,
                                        true
                                    );
                                }
                            }
                        }

                        Connections{
                            target: livecv.engine
                            onAboutToCreateObject : {
                                if (staticContainer)
                                    staticContainer.beforeCompile()
                                if ( engineMonitor )
                                    engineMonitor.emitBeforeCompile()
                            }
                            onObjectCreated : {
                                error.text = ''
                                if (runSpace.item) {
                                    runSpace.item.destroy();
                                }
                                runSpace.item = object;
                                if ( staticContainer )
                                    staticContainer.afterCompile()
                                if ( engineMonitor )
                                    engineMonitor.emitAfterCompile()
                            }
                            onObjectCreationError : {
                                var lastErrorsText = ''
                                var lastErrorsLog  = ''
                                for ( var i = 0; errors && i < errors.length; ++i ){
                                    var lerror = errors[i]
                                    var errorFile = lerror.fileName
                                    var index = errorFile.lastIndexOf('/')
                                    if ( index !== -1 && index < errorFile.length - 1)
                                        errorFile = errorFile.substring(index + 1)

                                    lastErrorsText +=
                                        (i !== 0 ? '<br>' : '') +
                                        '<a href=\"' + lerror.fileName + ':' + lerror.lineNumber + '\">' +
                                            errorFile + ':' + lerror.lineNumber +
                                        '</a>' +
                                        ' ' + lerror.message
                                    lastErrorsLog +=
                                        (lastErrorsLog === '' ? '' : '\n') +
                                        'Error: ' + lerror.fileName + ':' + lerror.lineNumber + ' ' + lerror.message
                                }
                                error.text = lastErrorsText
                                console.error(lastErrorsLog)
                            }
                        }
                    }

                    Rectangle{
                        id : errorWrap
                        anchors.bottom: parent.bottom
                        height : error.text !== '' ? error.height + 20 : 0
                        width : parent.width
                        color : editor.color
                        Behavior on height {
                            SpringAnimation { spring: 3; damping: 0.1 }
                        }

                        Rectangle{
                            width : 14
                            height : parent.height
                            color : "#601818"
                            visible: error.visible
                        }
                        Text {
                            id: error
                            anchors.left : parent.left
                            anchors.leftMargin: 25
                            anchors.verticalCenter: parent.verticalCenter

                            width: parent.width
                            wrapMode: Text.Wrap
                            textFormat: Text.StyledText

                            linkColor: "#c5d0d7"
                            font.family: "Ubuntu Mono, Courier New, Courier"
                            font.pointSize: editor.font.pointSize
                            text: ''
                            color: "#c5d0d7"
                            visible : text === "" ? false : true

                            onLinkActivated: {
                                project.openFile(link.substring(0, link.lastIndexOf(':')), ProjectDocument.EditIfNotOpen)
                            }
                        }
                    }

                }
            }

            ProjectNavigation{
                id: projectNavigation
                x: controls.navEditor ? controls.navEditor.x : 0
                width: controls.navEditor ? controls.navEditor.width : 0
                height: parent.height
                visible: false
                onOpen: {
                    projectView.focusEditor.document = project.openFile(path, ProjectDocument.EditIfNotOpen)
                    projectView.maintainCodingMode()
                    projectView.focusEditor.forceFocus()
                }
                onCloseFile: {
                    var doc = project.documentModel.isOpened(path)
                    if ( doc ){
                        if ( doc.isDirty ){
                            messageBox.show('File contains unsaved changes. Would you like to save them before closing?',
                            {
                                button1Name : 'Yes',
                                button1Function : function(){
                                    doc.save()
                                    project.closeFile(path)
                                    messageBox.close()
                                },
                                button2Name : 'No',
                                button2Function : function(){
                                    project.closeFile(path)
                                    messageBox.close()
                                },
                                button3Name : 'Cancel',
                                button3Function : function(){
                                    messageBox.close()
                                },
                                returnPressed : function(){
                                    doc.save()
                                    project.closeFile(path)
                                    messageBox.close()
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

            ProjectAddEntry{
                id: projectAddEntry
                anchors.left: parent.left
                width: projectView.width + editor.width
                height: parent.height
                visible: false
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
        }
    }

    property LogContainer logView : LogContainer{
        id: logView
        visible: false
        isInWindow: false
        width: parent ? parent.width : 0
        height: 200

        onItemAdded: {
            if ( !visible  )
                header.isLogWindowDirty = true
        }
    }


    MessageDialogMain{
        id: messageBox
        anchors.fill: parent
        visible: false
        backgroudColor: "#050e16"
    }

    License{
        id: licenseBox
        anchors.fill: parent
        visible: false
    }

    Connections{
        target: project
        onActiveChanged : {
            if (runSpace.item) {
                runSpace.item.destroy();
                runSpace.item = 0
                if ( staticContainer )
                    staticContainer.clearStates()
                if ( engineMonitor )
                    engineMonitor.emitTargetChanged()
            }
            if (active)
                createTimer.restart()
        }
    }

    Connections{
        target: project.documentModel
        onMonitoredDocumentChanged : {
            if (project.active === project.inFocus && controls.codingMode === 0 /* live compile */){
                engine.createObjectAsync(
                    runSpace.program,
                    runSpace,
                    project.active.file.pathUrl(),
                    project.active
                );
            } else if ( project.active && controls.codingMode === 0 /* live compile */){
                engine.createObjectAsync(
                    project.active.content,
                    runSpace,
                    project.active.file.pathUrl(),
                    project.active,
                    true
                );
            }
        }
        onDocumentChangedOutside : {
            messageBox.show(
                'File \'' + document.file.path + '\' changed outside Live CV. Would you like to reload it?',
                {
                    button1Name : 'Yes',
                    button1Function : function(){
                        messageBox.close()
                        document.readContent()
                        root.documentsReloaded = true
                    },
                    button2Name : 'Save',
                    button2Function : function(){
                        messageBox.close()
                        document.save()
                    },
                    button3Name : 'No',
                    button3Function : function(){
                        messageBox.close()
                    }
                }
            )
        }
    }


    // Logo

    Image{
        id: logo

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 20
        visible: true

        opacity: livecv.settings.launchMode ? 0.3 : 1.0
        source : "qrc:/images/logo.png"
    }

    Connections{
        target: project.fileModel
        onError : messageBox.show(message,{
            button2Name : 'Ok',
            button2Function : function(){ messageBox.close(); }
        })
    }

}
