import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.0
import base 1.0
import editor 1.0
import editor.private 1.0
import live 1.0

EditorLayer{
    id: root
    objectName: "editor"

    Component.onCompleted: {
        lk.layers.workspace.commands.add(root, {
            'saveFile' : [ function(){ if ( hasActiveEditor() ) lk.layers.workspace.panes.activePane.save() }, "Save File"],
            'saveFileAs' : [function(){ if ( hasActiveEditor() ) lk.layers.workspace.panes.activePane.saveAs() }, "Save File As"],
            'closeFile' : [function(){ if ( hasActiveEditor() ) lk.layers.workspace.panes.activePane.closeDocument() }, "Close File"],
            'assistCompletion' : [function(){ if ( hasActiveEditor() ) lk.layers.workspace.panes.activePane.assistCompletion() }, "Assist Completion"],
            'toggleSize' : [function(){ if ( hasActiveEditor() ) lk.layers.workspace.panes.activePane.toggleSize() }, "Toggle Size"],
            'toggleComment' : [function(){ if ( hasActiveEditor() ) lk.layers.workspace.panes.activePane.toggleComment() }, "Toggle Comment"]
        })
    }

    function hasActiveEditor(){
        return lk.layers.workspace.panes.activePane.objectName === 'editor'
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
            return editorBoxFactory.createObject(parent ? parent : lk.layers.workspace.panes.container)
        }

        function createEditorBox(child, aroundRect, panePosition, relativePlacement){
            var eb = editorBoxFactory.createObject(lk.layers.workspace.panes.container)
            eb.setChild(child, aroundRect, panePosition, relativePlacement)
            return eb;
        }

        property Component editorBoxFactory: Component{

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
                        child.codeHandler.removeConnection(child.editingFragment)
                    if (child.objectName === "objectContainer")
                        child.editor.documentHandler.codeHandler.removeConnection(child.editingFragment)
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
