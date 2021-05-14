import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import editor.private 1.0
import base 1.0
import live 1.0
import workspace 1.0 as Workspace

Rectangle{
    id: root

    property alias textEdit: textEdit
    property alias documentHandler: textEdit.documentHandler

    property QtObject addRootButton: addRootButton

    property var document: null
    onDocumentChanged: documentHandler.setDocument(document)

    property int fragmentStart: 0
    property int fragmentEnd: -1

    property bool importsShaped: false
    property bool rootShaped: false
    property color lineSurfaceColor: "black"
    color: "#03090d"

    objectName: "editorType"

    property alias internalActiveFocus : textEdit.activeFocus
    property alias internalFocus: textEdit.focus
    property alias lineSurfaceVisible: lineSurfaceBackground.visible
    property double lineSurfaceWidth: lineSurfaceBackground.visible ? lineSurface.width : 0

    property var qmlSuggestionBox: null

    function forceFocus(){
        textEdit.forceActiveFocus()
    }

    function cursorWindowCoords(reference){
        var r = reference ? reference : root
        return Qt.point(r.x, r.y - flick.flickableItem.contentY);
    }

    function save(){
        if ( !root.document )
            return;
        if ( root.document.file.exists() ){
            root.document.save()
        } else {
            saveAs()
        }
    }

    function makePositionVisible(y){
        if (y < 0 || y > root.height - 30)
            flick.flickableItem.contentY = Math.min(flick.flickableItem.contentY + y, Math.max(0,flick.flickableItem.contentHeight - root.height))
    }

    function saveAs(){
        lk.layers.window.dialogs.saveFile(
            { filters: [ "Qml files (*.qml)", "All files (*)" ] },
            function(url){
                var editordoc = root.document
                if ( !editordoc.saveAs(url) ){
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

                if ( !project.isDirProject() ){
                    project.openProject(url)
                } else if ( project.isFileInProject(url) ){
                    lk.layers.workspace.project.openFile(url, ProjectDocument.Edit)
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
                                    project.openProject(projectUrl);
                                }
                            )
                            mbox.close()
                        },
                        button3Name : 'Cancel',
                        button3Function : function(mbox){
                            mbox.close()
                        },
                        returnPressed : function(mbox){
                            var projectUrl = fileUrl
                            projectView.closeProject(
                                function(){
                                    project.openProject(projectUrl)
                                }
                            )
                            mbox.close()
                        }
                    })
                }
            }
        )
    }

    function toggleComment(){
        documentHandler.codeHandler.toggleComment(textEdit.selectionStart, textEdit.selectionEnd - textEdit.selectionStart)
    }

    function closeDocument(){
        if ( !root.document )
            return;
        if ( root.document.isDirty ){
            var saveFunction = function(mbox){
                if ( root.document.file.exists() ){
                    root.document.save()
                    root.closeDocumentAction()
                } else {
                    lk.layers.window.dialogs.saveFile(
                        { filters: [ "Qml files (*.qml)", "All files (*)" ] },
                        function(url){
                            if ( !root.document.saveAs(url) ){
                                lk.layers.window.dialogs.message(
                                    'Failed to save file to: ' + url,
                                    {
                                        button3Name : 'Ok',
                                        button3Function : function(){ lk.layers.window.dialogs.messageClose(); }
                                    }
                                )
                                return;
                            }
                            root.closeDocumentAction()
                        }
                    )
                }
                mbox.close()
            }

            lk.layers.window.dialogs.message('File contains unsaved changes. Would you like to save them before closing?',
            {
                button1Name : 'Yes',
                button1Function : function(mbox){
                    saveFunction(mbox)
                },
                button2Name : 'No',
                button2Function : function(mbox){
                    mbox.close()
                    root.closeDocumentAction()
                    root.document = project.documentModel.lastOpened()
                },
                button3Name : 'Cancel',
                button3Function : function(mbox){
                    mbox.close()
                },
                returnPressed : function(mbox){
                    saveFunction(mbox)
                },
            })
        } else
            root.closeDocumentAction()
    }

    function getCursorFragment(){
        if (documentHandler.has(documentHandler.LanguageScope ) ){
            return documentHandler.contextBlockRange(textEdit.cursorPosition)
        }
        return null
    }

    function closeDocumentAction(){
        if ( !project.isDirProject() && document.file.path === project.active.path ){
            lk.layers.window.dialogs.message(
                'Closing this file will also close this project. Would you like to close the project?',
            {
                button1Name : 'Yes',
                button1Function : function(mbox){
                    project.closeProject()
                    mbox.close()
                },
                button3Name : 'No',
                button3Function : function(mbox){
                    mbox.close()
                },
                returnPressed : function(mbox){
                    project.closeProject()
                    mbox.close()
                }
            })
        } else {
            project.closeFile(document.file.path)
            root.document = project.documentModel.lastOpened()
        }
    }

    function getCursorRectangle(){
        return textEdit.cursorRectangle
    }

    function assistCompletion(){
        if (documentHandler.has(DocumentHandler.LanguageCodeCompletion))
            documentHandler.codeHandler.suggestCompletion(textEdit.cursorPosition)
    }

    FontMetrics{
        id: editorMetrics
        font: textEdit.font
    }

    Rectangle{
        id: lineSurfaceBackground
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: visible ? lineSurface.width + 5 : 0
        color: root.lineSurfaceColor
        clip: true
        visible: true

        Flickable{
            anchors.fill: parent
            anchors.bottomMargin: 10
            anchors.leftMargin: 5
            contentY: flick.flickableItem.contentY
            interactive: false

            LineSurface{
                id: lineSurface
                color: "#3e464d"
                viewport: Qt.rect(0,flick.flickableItem.contentY,0,flick.height)
                Component.onCompleted: {
                    setComponents(textEdit);
                }
            }
        }
    }

    ScrollView {
        id: flick

        anchors.fill: parent
        anchors.topMargin: 5
        anchors.leftMargin: 7 + lineSurfaceBackground.width
        anchors.rightMargin: 3

        style: ScrollViewStyle {
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
            decrementControl: null
            incrementControl: null
            frame: Item{}
            corner: Rectangle{color: root.color}
        }

        frameVisible: false

        function ensureVisible(r){
            if (!editor.internalActiveFocus )
                return;
            if (flickableItem.contentX >= r.x)
                flickableItem.contentX = r.x;
            else if (flickableItem.contentX + width <= r.x + r.width + 20)
                flickableItem.contentX = r.x + r.width - width + 20;
            if (flickableItem.contentY >= r.y)
                flickableItem.contentY = r.y;
            else if (flickableItem.contentY + height <= r.y + r.height + 20)
                flickableItem.contentY = r.y + r.height - height + 20;
        }

        flickableItem.interactive: false
        flickableItem.contentHeight: textEdit.totalHeight
        flickableItem.contentWidth: textEdit.totalWidth + 10

        Flickable {
            id: editorFlickable

            NewTextEdit {
                id : textEdit


                anchors.left: parent.left
                anchors.leftMargin: 2
                viewport: Qt.rect(flick.flickableItem.contentX,flick.flickableItem.contentY,flick.width,flick.height)

                fragmentStart: root.fragmentStart
                fragmentEnd: root.fragmentEnd

                documentHandler: DocumentHandler {
                    id: documentHandler
                    editorFocus: textEdit.activeFocus
                    onCursorPositionRequest : {
                        textEdit.forceActiveFocus()
                        textEdit.cursorPosition = position
                    }
                }

                function getEditor(){ return root }

                property int lastLength: 0

                onCursorPositionChanged: {
                    flick.ensureVisible(cursorRectangle)
                    /// disable the model if no text has changed, let the code handler decide otherwise
                    if ( length === lastLength )
                        documentHandler.completionModel.disable()
                    lastLength = length
                }

                focus : true

                color : "#fff"
                font.family: "Source Code Pro, Ubuntu Mono, Courier New, Courier"
                font.pixelSize: lk.settings.file('editor').fontSize
                font.weight: Font.Normal

                selectByMouse: true
                mouseSelectionMode: NewTextEdit.SelectCharacters
                selectionColor: "#3d4856"

                textFormat: NewTextEdit.PlainText

                wrapMode: NewTextEdit.NoWrap

                height: Math.max(paintedHeight, flick.height)
                width: Math.max(paintedWidth, flick.width)

                readOnly: root.document === null || root.document.isMonitored || (importsShaped && rootShaped)
                cursorVisible: !readOnly

                Keys.onPressed: {
                    if ( event.key === Qt.Key_PageUp ){
                        if ( documentHandler.completionModel.isEnabled ){
                            qmlSuggestionBox.highlightPrevPage()
                        } else {
                            var lines = flick.height / cursorRectangle.height
                            var prevLineStartPos = textEdit.text.lastIndexOf('\n', cursorPosition - 1)
                            while ( --lines > 0 ){
                                cursorPosition   = prevLineStartPos + 1
                                prevLineStartPos = textEdit.text.lastIndexOf('\n', cursorPosition - 2)
                                if ( prevLineStartPos === -1 ){
                                    cursorPosition = 0;
                                    break;
                                }
                            }
                        }
                        event.accepted = true
                    } else if ( event.key === Qt.Key_Tab ){
                        if ( event.modifiers & Qt.ShiftModifier ){
                            documentHandler.manageIndent(
                                textEdit.selectionStart, textEdit.selectionEnd - textEdit.selectionStart, true
                            )
                            event.accepted = true
                        } else if ( selectionStart !== selectionEnd ){
                            documentHandler.manageIndent(
                                textEdit.selectionStart, textEdit.selectionEnd - textEdit.selectionStart, false
                            )
                            event.accepted = true
                        } else {
                            documentHandler.insertTab(cursorPosition)
                            event.accepted = true
                        }
                    } else if ( event.key === Qt.Key_Backtab ){
                        documentHandler.manageIndent(
                            textEdit.selectionStart, textEdit.selectionEnd - textEdit.selectionStart, true
                        )
                        event.accepted = true
                    } else if ( event.key === Qt.Key_PageDown ){
                        if ( documentHandler.completionModel.isEnabled ){
                            qmlSuggestionBox.highlightNextPage()
                        } else {
                            var lines = flick.height / cursorRectangle.height
                            var nextLineStartPos = textEdit.text.indexOf('\n', cursorPosition)
                            while ( lines-- > 0 && nextLineStartPos !== -1 ){
                                cursorPosition   = nextLineStartPos + 1
                                nextLineStartPos = textEdit.text.indexOf('\n', cursorPosition)
                            }
                        }
                        event.accepted = true
                    } else if ( event.key === Qt.Key_Down ){
                        if ( documentHandler.completionModel.isEnabled ){
                            event.accepted = true
                            qmlSuggestionBox.highlightNext()
                        }
                    } else if ( event.key === Qt.Key_Up ){
                        if ( documentHandler.completionModel.isEnabled ){
                            event.accepted = true
                            qmlSuggestionBox.highlightPrev()
                        }
                    } else if ( event.key === Qt.Key_Escape ){
                        if ( documentHandler.completionModel.isEnabled ){
                            documentHandler.completionModel.disable()
                        }
                    } else {
                        var command = lk.layers.workspace.keymap.locateCommand(event.key, event.modifiers)
                        if ( command !== '' ){
                            lk.layers.workspace.commands.execute(command)
                            event.accepted = true
                            return
                        }
                    }
                }

                Keys.onReturnPressed: {
                    event.accepted = false
                    if ( documentHandler.completionModel.isEnabled ){
                        documentHandler.insertCompletion(
                            documentHandler.completionModel.completionPosition,
                            cursorPosition,
                            qmlSuggestionBox.getCompletion()
                        )
                        event.accepted = true
                    }
                }

                Behavior on font.pixelSize {
                    NumberAnimation { duration: 40 }
                }

                Connections{
                    target: project.documentModel
                    function onAboutToClose(document){
                        if ( document === root.document ){
                            root.document = null
                        }
                    }
                }

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        if ( lk.layers.workspace ){
                            lk.layers.workspace.panes.openContextMenu(root, root.parent)
                        }
                    }
                }
            }

        }
    }

    Connections{
        target: documentHandler.completionModel
        function onIsEnabledChanged(){
            if ( !documentHandler.completionModel.isEnabled || documentHandler.completionModel.suggestionCount() === 0 ){
                qmlSuggestionBox.visible = false
                qmlSuggestionBox.opacity = 0
                return
            }

            var activePane = editor.parent
            var paneCoords = activePane.mapGlobalPosition()
            var r = textEdit.cursorRectangle

            var rect = {
                x:  textEdit.positionToRectangle(documentHandler.completionModel.completionPosition).x -
                    flick.flickableItem.contentX +
                    167, // compensate for center alignment when creating the editor box
                y: r.y - flick.flickableItem.contentY,
                width: r.width,
                height: r.height
            }

            if (qmlSuggestionBox){
                qmlSuggestionBox.visible = true
                qmlSuggestionBox.opacity = 0.95
                qmlSuggestionBox.parent.updatePlacement(
                    rect,
                    Qt.point(
                        paneCoords.x - 7, paneCoords.y - 43),
                        lk.layers.editor.environment.placement.bottom)

            } else {
                qmlSuggestionBox = paletteControls.createSuggestionBox()

                qmlSuggestionBox.fontFamily = textEdit.font.family
                qmlSuggestionBox.fontSize = textEdit.font.pixelSize
                qmlSuggestionBox.smallFontSize = textEdit.font.pixelSize - 2

                qmlSuggestionBox.opacity = 0.95
                qmlSuggestionBox.model = documentHandler.completionModel

                var editorBox = lk.layers.editor.environment.createEditorBox(
                    qmlSuggestionBox,
                    rect,
                    Qt.point(paneCoords.x, paneCoords.y),
                    lk.layers.editor.environment.placement.bottom
                )

                editorBox.color = 'transparent'
            }

        }

    }

    Workspace.TextButton{
        id: addRootButton
        anchors.left: lineSurfaceBackground.right
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: textEdit.totalHeight + 10
        text: "Add root Item"
        width: 120
        height: 30
        visible: false
        property var callback: null
        onClicked: {
            visible = false
            lk.layers.workspace.extensions.editqml.add(2, true, true)
        }
    }

    Component.onDestruction: {
        if (!qmlSuggestionBox)
            return
        var par = qmlSuggestionBox.parent
        qmlSuggestionBox.destroy()
        qmlSuggestionBox = null
        par.destroy()
    }
}
