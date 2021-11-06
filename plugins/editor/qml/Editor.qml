import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import editor.private 1.0
import editor.style 1.0
import base 1.0
import live 1.0
import visual.input 1.0 as Input

Rectangle{
    id: root
    objectName: "editorType"

    property alias textEdit: textEdit
    property alias code: textEdit.code

    property alias internalActiveFocus : textEdit.activeFocus
    property alias internalFocus: textEdit.focus
    property alias lineSurfaceVisible: lineSurfaceBackground.visible
    property double lineSurfaceWidth: lineSurfaceBackground.visible ? lineSurface.width : 0

    property int fragmentStart: 0
    property int fragmentEnd: -1

    property var document: null
    onDocumentChanged: code.setDocument(document)

    property QtObject style: EditorStyle{}

    color: root.style.backgroundColor

    signal keyPress(var event)
    signal rightClicked(var mouse)

    property var __qmlSuggestionBox: null

    property var getGlobalPosition: function(){
        return root.mapToGlobal(0, 0)
    }

    function forceFocus(){
        textEdit.forceActiveFocus()
    }

    function cursorWindowCoords(reference){
        var r = reference ? reference : root
        return Qt.point(r.x, r.y - flick.flickableItem.contentY);
    }

    function makePositionVisible(y){
        if (y < 0 || y > root.height - 30)
            flick.flickableItem.contentY = Math.min(flick.flickableItem.contentY + y, Math.max(0,flick.flickableItem.contentHeight - root.height))
    }

    function getCursorFragment(){
        if (code.has(code.LanguageScope ) ){
            return code.contextBlockRange(textEdit.cursorPosition)
        }
        return null
    }

    function getCursorRectangle(){
        return textEdit.cursorRectangle
    }

    function assistCompletion(){
        if (code.has(DocumentHandler.LanguageCodeCompletion))
            code.language.suggestCompletion(textEdit.cursorPosition)
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
        color: root.style.lineSurfaceBackgroundColor
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
                color: root.style.lineSurfaceTextColor
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
                    color: root.style.scrollbarHandleColor
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
            if (!root.internalActiveFocus )
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

                code: DocumentHandler {
                    id: code
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
                        code.completionModel.disable()
                    lastLength = length
                }

                focus : true

                color : root.style.textStyle.color
                font.italic: root.style.textStyle.font.italic
                font.family: root.style.textStyle.font.family
                font.pixelSize: root.style.textStyle.font.pixelSize
                font.weight: root.style.textStyle.font.weight

                selectionColor: root.style.selectionBackgroundColor
                selectedTextColor: root.style.selectionColor
                selectByMouse: true
                mouseSelectionMode: NewTextEdit.SelectCharacters

                textFormat: NewTextEdit.PlainText

                wrapMode: NewTextEdit.NoWrap

                height: Math.max(paintedHeight, flick.height)
                width: Math.max(paintedWidth, flick.width)

                readOnly: root.document === null || root.document.isMonitored || (code && code.language && code.language.importsFragment && code.language.rootFragment)
                cursorVisible: !readOnly && activeFocus

                Keys.onPressed: {
                    if ( event.key === Qt.Key_PageUp ){
                        if ( code.completionModel.isEnabled ){
                            __qmlSuggestionBox.highlightPrevPage()
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
                            code.manageIndent(
                                textEdit.selectionStart, textEdit.selectionEnd - textEdit.selectionStart, true
                            )
                            event.accepted = true
                        } else if ( selectionStart !== selectionEnd ){
                            code.manageIndent(
                                textEdit.selectionStart, textEdit.selectionEnd - textEdit.selectionStart, false
                            )
                            event.accepted = true
                        } else {
                            code.insertTab(cursorPosition)
                            event.accepted = true
                        }
                    } else if ( event.key === Qt.Key_Backtab ){
                        code.manageIndent(
                            textEdit.selectionStart, textEdit.selectionEnd - textEdit.selectionStart, true
                        )
                        event.accepted = true
                    } else if ( event.key === Qt.Key_PageDown ){
                        if ( code.completionModel.isEnabled ){
                            __qmlSuggestionBox.highlightNextPage()
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
                        if ( code.completionModel.isEnabled ){
                            event.accepted = true
                            __qmlSuggestionBox.highlightNext()
                        }
                    } else if ( event.key === Qt.Key_Up ){
                        if ( code.completionModel.isEnabled ){
                            event.accepted = true
                            __qmlSuggestionBox.highlightPrev()
                        }
                    } else if ( event.key === Qt.Key_Escape ){
                        if ( code.completionModel.isEnabled ){
                            code.completionModel.disable()
                        }
                    } else {
                        root.keyPress(event)
                    }
                }

                Keys.onReturnPressed: {
                    event.accepted = false
                    if ( code.completionModel.isEnabled ){
                        code.insertCompletion(
                            code.completionModel.completionPosition,
                            cursorPosition,
                            __qmlSuggestionBox.getCompletion()
                        )
                        event.accepted = true
                    }
                }

                Behavior on font.pixelSize {
                    NumberAnimation { duration: 40 }
                }

                MouseArea{
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        root.rightClicked(mouse)
                    }
                }
            }

        }
    }

    Connections{
        target: project.documentModel
        function onAboutToClose(document){
            if ( document === root.document ){
                root.document = null
            }
        }
    }

    Connections{
        target: code.completionModel
        function onIsEnabledChanged(){
            if ( !code.completionModel.isEnabled || code.completionModel.suggestionCount() === 0 ){
                __qmlSuggestionBox.visible = false
                return
            }

            var paneCoords = root.getGlobalPosition()
            var r = textEdit.cursorRectangle

            var rect = {
                x:  textEdit.positionToRectangle(code.completionModel.completionPosition).x -
                    flick.flickableItem.contentX +
                    167, // compensate for center alignment when creating the editor box
                y: r.y - flick.flickableItem.contentY,
                width: r.width,
                height: r.height
            }

            if (__qmlSuggestionBox){
                __qmlSuggestionBox.visible = true
                __qmlSuggestionBox.parent.updatePlacement(
                    rect,
                    Qt.point(paneCoords.x - 7, paneCoords.y - 43),
                    lk.layers.editor.environment.placement.bottom
                )

            } else {
                __qmlSuggestionBox = lk.layers.editor.environment.createSuggestionBox(null, textEdit.font)
                __qmlSuggestionBox.model = code.completionModel

                var editorBox = lk.layers.editor.environment.createEditorBox(
                    __qmlSuggestionBox,
                    rect,
                    Qt.point(paneCoords.x, paneCoords.y),
                    lk.layers.editor.environment.placement.bottom
                )

                editorBox.color = 'transparent'
            }
        }
    }

    Component.onDestruction: {
        if (!__qmlSuggestionBox)
            return
        var par = __qmlSuggestionBox.parent
        __qmlSuggestionBox.destroy()
        __qmlSuggestionBox = null
        par.destroy()
    }
}
