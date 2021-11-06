import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.0
import base 1.0
import editor 1.0
import editor.private 1.0
import workspace 1.0
import live 1.0

EditorLayer{
    id: root
    objectName: "editor"

    function hasActiveEditor(){
        return lk.layers.workspace.panes.activePane.objectName === 'editor'
    }

    property var commandFunctions : ({
        saveFile : function(){
            if ( hasActiveEditor() ){
                var document = lk.layers.workspace.panes.activePane.editor.document
                lk.layers.workspace.wizards.saveDocument(document)
            }
        },
        saveFileAs: function(){
            if ( hasActiveEditor() ){
                var document = lk.layers.workspace.panes.activePane.editor.document
                lk.layers.workspace.wizards.saveDocumentAs(document)
            }
        },
         closeFile: function(){
             if ( hasActiveEditor() ){
                 var document = lk.layers.workspace.panes.activePane.editor.document
                 lk.layers.workspace.wizards.closeDocument(document)
             }
         },
        assistCompletion: function(){
            if ( hasActiveEditor() ){
                var editor = lk.layers.workspace.panes.activePane.editor
                editor.assistCompletion()
            }
        },
        toggleSize: function(){
            if ( hasActiveEditor() ){
                var editorPane = lk.layers.workspace.panes.activePane
                if ( editorPane.width < editorPane.parent.width / 2)
                    editorPane.width = editorPane.parent.width - editorPane.parent.width / 4
                else if ( editorPane.width === editorPane.parent.width / 2 )
                    editorPane.width = editorPane.parent.width / 4
                else
                    editorPane.width = editorPane.parent.width / 2
            }
        },
        toggleComment: function(){
            if ( hasActiveEditor() ){
                var editor = lk.layers.workspace.panes.activePane.editor
                if ( editor.code.language ){
                    editor.code.language.toggleComment(editor.textEdit.selectionStart, editor.textEdit.selectionEnd - editor.textEdit.selectionStart)
                }
            }
        }
    })

    Component.onCompleted: {
        lk.layers.workspace.commands.add(root, {
            'saveFile' : [ root.commandFunctions.saveFile, "Save File"],
            'saveFileAs' : [ root.commandFunctions.saveFileAs, "Save File As"],
            'closeFile' : [ root.commandFunctions.closeFile, "Close File"],
            'assistCompletion' : [ root.commandFunctions.assistCompletion, "Assist Completion"],
            'toggleSize' : [ root.commandFunctions.toggleSize, "Toggle Size"],
            'toggleComment' : [ root.commandFunctions.toggleComment, "Toggle Comment"]
        })
    }


    environment: QtObject{

        property QtObject placement : QtObject{
            property int top: 0
            property int right: 1
            property int bottom: 2
            property int left: 3
        }

        property Item content : null

        function createEmptyEditorBox(parent){
            return __editorBox.createObject(parent ? parent : lk.layers.workspace.panes.container)
        }

        function createEditorBox(child, aroundRect, panePosition, relativePlacement){
            var eb = __editorBox.createObject(lk.layers.workspace.panes.container)
            eb.setChild(child, aroundRect, panePosition, relativePlacement)
            return eb;
        }

        function createSuggestionBox(parent, font){
            var sb = __suggestionBox.createObject(parent)
            sb.__initialize(font)
            return sb
        }

        function addLoadingScreen(editor){
            var anim = __loadingScreen.createObject(editor)
            anim.visible = true
            return anim
        }

        function hasLoadingScreen(editor){
            for ( var i = 0; i < editor.children.length; ++i ){
                var child = editor.children[i]
                if ( child.objectName === 'loadingAnimation' )
                    return true
            }
            return false
        }

        function removeLoadingScreen(editor){
            for ( var i = 0; i < editor.children.length; ++i ){
                var child = editor.children[i]
                if ( child.objectName === 'loadingAnimation' )
                    child.destroy()
            }
        }

        property Component __loadingScreen : Item{
            anchors.fill: parent
            objectName: 'loadingAnimation'
            visible: false

            property color backgroundColor: 'black'

            LoadingAnimation{
                id: loadingAnimation
                visible: parent.visible
                x: parent.width / 2 - width / 2
                y: parent.height / 2 - height / 2
                z: 1000
                size: 8
            }

            Rectangle{
                visible: parent.visible
                anchors.fill: parent
                color: parent.backgroundColor
                opacity: parent.visible ? 0.95 : 0
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
        }

        property Component __suggestionBox : Component {
            SuggestionBox{
                Behavior on opacity {
                    NumberAnimation { duration: 150 }
                }
            }
        }

        property Component __editorBox: Component{

            Rectangle{
                id: editorBoxComponent
                x: 0
                y: visible ? 0 : 20
                width: child ? child.width : 0
                height: child ? child.height : 0
                visible: false
                opacity: visible ? 1 : 0
                objectName: "editorBox"

                property Item child : null
                children: child ? [child] : []

                function close(){
                    if (!child) return;

                    if (child.objectName === "paletteGroup")
                        child.editFragment.language.removeConnection(child.editFragment)
                    if (child.objectName === "objectContainer")
                        child.editor.code.language.removeConnection(child.editFragment)
                }

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
                        var maxX = lk.layers.workspace.panes.container.width - editorBoxComponent.width
                        editorBoxComponent.x = newX < 0 ? 0 : (newX > maxX ? maxX : newX)

                        var upY = startY - editorBoxComponent.height
                        var downY = startY + aroundRectangle.height + 5

                        if ( relativePlacement === 0 ){ // top placement
                            editorBoxComponent.y = upY > 0 ? upY : downY
                        } else { // bottom placement
                            editorBoxComponent.y = downY + editorBoxComponent.height < lk.layers.workspace.panes.container.height ? downY : upY
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
                        var maxY = lk.layers.workspace.panes.container.height - editorBoxComponent.height
                        editorBoxComponent.y = newY < 0 ? 0 : (newY > maxY ? maxY : newY)

                        var upX = startX - editorBoxComponent.width
                        var downX = startX + aroundRectangle.width + 5

                        if ( relativePlacement === 1 ){ // right placement
                            editorBoxComponent.x = upX > 0 ? upX : downX
                        } else { // left placement
                            editorBoxComponent.x = downX + editorBoxComponent.width < lk.layers.workspace.panes.container.width ? downX : upX
                        }
                        moveXBehavior.enabled = true
                    }
                }

                function setChild(item, aroundRectangle, editorPosition, relativePlacement){
                    if ( relativePlacement === 0 || relativePlacement === 2 ){
                        moveXBehavior.enabled = false
                        moveYBehavior.enabled = false

                        var startY = editorPosition.y + aroundRectangle.y

                        editorBoxComponent.x = editorPosition.x + aroundRectangle.x
                        editorBoxComponent.y = startY - aroundRectangle.y / 2

                        editorBoxComponent.child = item

                        moveYBehavior.enabled = true
                        visible = true

                        var newX = editorBoxComponent.x - editorBoxComponent.width / 2
                        var maxX = lk.layers.workspace.panes.container.width - editorBoxComponent.width
                        editorBoxComponent.x = newX < 0 ? 0 : (newX > maxX ? maxX : newX)

                        var upY = startY - editorBoxComponent.height
                        var downY = startY + aroundRectangle.height

                        if ( relativePlacement === 0 ){ // top placement
                            editorBoxComponent.y = upY > 0 ? upY : downY
                        } else { // bottom placement
                            editorBoxComponent.y = downY + editorBoxComponent.height < lk.layers.workspace.panes.container.height ? downY : upY
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
                        var maxY = lk.layers.workspace.panes.container.height - editorBoxComponent.height
                        editorBoxComponent.y = newY < 0 ? 0 : (newY > maxY ? maxY : newY)

                        var upX = startX - editorBoxComponent.width
                        var downX = startX + aroundRectangle.width + 5

                        if ( relativePlacement === 1 ){ // right placement
                            editorBoxComponent.x = upX > 0 ? upX : downX
                        } else { // left placement
                            editorBoxComponent.x = downX + editorBoxComponent.width < lk.layers.workspace.panes.container.width ? downX : upX
                        }
                    }
                }
            }
        }

    }

}
