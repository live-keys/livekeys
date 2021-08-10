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

            var propertyConnections = []

            var codeHandler = editFragment.language
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
                            var connection = {
                                left: {
                                    node: n,
                                    inPort: p.inPort,
                                    outPort: p.outPort,
                                    id: childFragment.objectId(),
                                    property: current.identifier()
                                },
                                right: {
                                    node: null,
                                    inPort: null,
                                    outPort: null,
                                    id: currentConnection.expression.length === 2 ? currentConnection.expression[0] : '',
                                    property: currentConnection.expression.length === 2 ? currentConnection.expression[1] : currentConnection.expression[0]
                                },
                                isFunction: currentConnection.isFunction ? true : false
                            }

                            propertyConnections.push(connection)
                        }
                    }
                }
            }

            for ( var j = 0; j < propertyConnections.length; ++j ){
                var propertyConnection = propertyConnections[j]

                var node = null
                if ( propertyConnection.right.id ){
                    node = objectsWithId[propertyConnection.right.id]
                } else {
                    node = propertyConnection.left.node
                }

                if (node){
                    var nodeProperty = node.item.propertyByName(propertyConnection.right.property)
                    if ( nodeProperty ){
                        propertyConnection.right.node = node
                        propertyConnection.right.outPort = nodeProperty.outPort
                        propertyConnection.right.inPort = nodeProperty.inPort
                    } else {
                        if ( propertyConnection.isFunction ){
                            nodeProperty = node.item.addFunctionProperty(propertyConnection.right.property)
                            propertyConnection.right.node = node
                            propertyConnection.right.inPort = nodeProperty.inPort
                            propertyConnection.right.outPort = nodeProperty.outPort
                        } else {
                            var position = node.item.editFragment.valuePosition() +
                                           node.item.editFragment.valueLength() - 1
                            var paletteControls = lk.layers.workspace.extensions.editqml.paletteControls
                            paletteControls.addPropertyToObjectContainer(node.item, propertyConnection.right.property, false, position)

                            nodeProperty = node.item.propertyByName(propertyConnection.right.property)
                            if ( nodeProperty ){
                                propertyConnection.right.node = node
                                propertyConnection.right.outPort = nodeProperty.outPort
                                propertyConnection.right.inPort = nodeProperty.inPort
                            }
                        }
                    }
                }
            }

            for ( var j = 0; j < propertyConnections.length; ++j ){
                var propertyConnection = propertyConnections[j]
                if ( propertyConnection.isFunction ){
                    if ( propertyConnection.right.inPort && propertyConnection.left.outPort )
                        objectGraph.bindPorts(propertyConnection.left.outPort, propertyConnection.right.inPort)
                } else {
                    if ( propertyConnection.right.outPort && propertyConnection.left.inPort )
                        objectGraph.bindPorts(propertyConnection.right.outPort, propertyConnection.left.inPort)
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
                        editFragment.language.removeConnection(child.editFragment)
                    child.destroy()
                }
                editFragment.language.removeConnection(allObjects[i].item.editFragment)
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

        editor = editFragment.language.code.textEdit().getEditor()

        objectGraph.editFragment = editFragment
        nodeItem.init()
        editFragment.aboutToRemovePalette.connect(function(palette){
            nodeItem.clean()
        })
    }
}
