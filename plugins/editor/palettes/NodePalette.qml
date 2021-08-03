import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0
import lcvcore 1.0
import workspace.nodeeditor 1.0
import editqml 1.0

CodePalette{
    id: palette
    type: "qml/Object"

    property var editor: null
    property var objectsWithId: ({})
    property var numOfObjects: 0
    property var allObjects: []

    property QtObject theme: lk.layers.workspace.themes.current

    function addObject(object, cursorCoords){
        var objectName = object.typeName()
        var objectId = object.objectId()

        var n = null
        if (cursorCoords){
            n = objectGraph.addObjectNode(
                    cursorCoords.x,
                    cursorCoords.y,
                    (objectName + (objectId ? ("#" + objectId) : ""))
                )
        } else {
            n = objectGraph.addObjectNode(numOfObjects *420 + 50, 50, (objectName + (objectId ? ("#" + objectId) : "")))
        }

        ++numOfObjects

        if (objectId){
           objectsWithId[objectId] = n
        }

        allObjects.push(n)

        n.item.editFragment = object
        object.incrementRefCount()

        n.item.expandDefaultPalette()
        return n
    }

    item: Item {
        id: nodeItem

        width: objectGraph.width
        height: objectGraph.height

        function resize(w, h){
            objectGraph.width = w
            objectGraph.height = h
            objectGraph.redrawGrid()
        }

        function init(){

            var props = []

            var codeHandler = editFragment.codeHandler
            var childFragmentList = codeHandler.openNestedFragments(editFragment)
            for ( var i = 0; i < childFragmentList.length; ++i ){
                var childFragment = childFragmentList[i]
                if ( childFragment.location !== QmlEditFragment.Object )
                    continue;

                var n = addObject(childFragment)

                var currentChildNestedFragments = codeHandler.openNestedFragments(childFragment)

                for (var j = 0; j < currentChildNestedFragments.length; ++j ){
                    var current = currentChildNestedFragments[j]
                    if ( current.location === QmlEditFragment.Object ){
                        n.item.addChildObject(current)
                    } else {
                        var p = n.item.addProperty(current)
                        p.z = 1000 - j

                        var currentConnection = current.readValueConnection()
                        if ( currentConnection ){
                            //TODO: Do for length === 1 (local properties && direct object references && functions)
                            if ( currentConnection.expression.length === 2 ){
                                if (p.inPort)
                                    props.push({"port": p.inPort, "value": currentConnection.expression})
                                else
                                    props.push({"value": object.properties[j].value})
                            }
                        }
                    }
                }
            }

            for (var k = 0; k < props.length; ++k){
                var id = props[k].value[0]
                var node = objectsWithId[id]
                if (node)
                {
                    var nodeProps = node.item.properties
                    var propp = node.item.propertyByName(props[k].value[1])
                    if ( propp ){
                        objectGraph.bindPorts(propp.outPort, props[k].port)
                    } else {
                        node.item.addPropertyToNodeByName(props[k].value[1])
                        for (var ppx = 0; ppx < nodeProps.length; ++ppx)
                        {
                            var proppx = node.item.properties[ppx]
                            if (proppx.propertyName === props[k].value[1]){
                                if (props[k].port)
                                    objectGraph.bindPorts(proppx.outPort, props[k].port)
                                break
                            }

                        }
                    }
                }
            }
            if (numOfObjects !== 0){
                objectGraph.zoomOrigin = 0
                objectGraph.zoom = 600.0/(numOfObjects*420.0 + 50)
                objectGraph.zoomOrigin = 4
            }
        }

        function clean(){
            for (var i=0; i< allObjects.length; ++i){
                if (!allObjects[i].item) return
                var numofProps = allObjects[i].item.propertyContainer.children.length
                for (var j=0; j < numofProps; ++j){
                    var child = allObjects[i].item.propertyContainer.children[j]
                    if (child.editFragment)
                        editFragment.codeHandler.removeConnection(child.editFragment)
                    child.destroy()
                }
                editFragment.codeHandler.removeConnection(allObjects[i].item.editFragment)
            }

            allObjects = []
            objectsWithId = []
        }

        ObjectGraph {
            id: objectGraph
            width: 600
            height: 300
            palette: palette
            editor: palette.editor
            editFragment: palette.editFragment
            style: theme.nodeEditor
        }
    }


    onInit: {
        if (!editFragment)
            return

        editor = editFragment.codeHandler.code.textEdit().getEditor()

        objectGraph.editFragment = editFragment
        nodeItem.init()
        editFragment.objectAdded.connect(function(obj, cursorCoords){
            addObject(obj.objectInfo(), cursorCoords)
        })
        editFragment.aboutToRemovePalette.connect(function(palette){
            nodeItem.clean()
        })
    }
}
