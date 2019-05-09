import QtQuick 2.3

QtObject{

    property int codingMode: 0

    property QtObject placement : QtObject{
        property int top: 0
        property int right: 1
        property int bottom: 2
        property int left: 3
    }

    property Item content : null

    function createEmptyEditorBox(parent){
        return editorBoxFactory.createObject(parent ? parent : contentWrap)
    }

    function createEditorBox(child, aroundRect, editorPosition, relativePlacement){
        var eb = editorBoxFactory.createObject(contentWrap)
        eb.setChild(child, aroundRect, editorPosition, relativePlacement)
        return eb;
    }

    property Component editorBoxFactory: Component{

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

}
