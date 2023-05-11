import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0
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

    function findObjectById(objectId){
        for ( var k = 0; k < allObjects.length; ++k ){
            var objectNode = allObjects[k].item
            if ( objectNode.control.editFragment.objectId() === objectId ){
                return allObjects[k]
            }

            for ( var l = 0; l < objectNode.control.members.length; ++l ){
                var objectNodeMember = objectNode.control.members[l]

                if(objectNodeMember.control.editFragment && objectNodeMember.control.editFragment.location === QmlEditFragment.Object ){
                    if ( objectNodeMember.control.editFragment.objectId() === objectId )
                        return objectNodeMember
                }
            }
        }
        return null
    }

    function addObject(editFragment, cursorCoords){
        var n = null
        if (cursorCoords){
            n = objectGraph.addObjectNode(
                cursorCoords.x,
                cursorCoords.y,
                editFragment
            )
        } else {
            n = objectGraph.addObjectNode(
                numOfObjects *420 + 50,
                50,
                editFragment
            )
        }

        ++numOfObjects

        if (n.item.control.objectId){
           objectsWithId[n.item.control.objectId] = n
        }

        allObjects.push(n)
        editFragment.incrementRefCount()

        n.item.control.expandDefaultPalette()
        return n
    }

    item: Item {
        id: nodeItem

        width: 600
        height: 300

        property var sizeInfo: ({ minWidth: 300, maxWidth: -1, minHeight: 200, maxHeight: -1 })

        function resize(w, h){
            objectGraph.width = w
            objectGraph.height = h
            objectGraph.redrawGrid()
        }

        function init(){

            var propertyConnections = []

            var language = editFragment.language

            var declarationsResult = language.findDeclarations(editFragment.position() + 1, editFragment.length() - 2)
            if ( declarationsResult.hasReport() ){
                for ( var i = 0; i < declarationsResult.report.length; ++i ){
                    lk.layers.workspace.messages.pushErrorObject(declarationsResult.report[i])
                }
            }
            var fragmentsResult = language.openChildConnections(editFragment, declarationsResult.value)
            if ( fragmentsResult.hasReport() ){
                for ( var i = 0; i < fragmentsResult.report.length; ++i ){
                    lk.layers.workspace.messages.pushErrorObject(fragmentsResult.report[i])
                }
            }

            var childFragmentList = fragmentsResult.value

            for ( var i = 0; i < childFragmentList.length; ++i ){
                var childFragment = childFragmentList[i]
                if ( childFragment.location !== QmlEditFragment.Object )
                    continue;

                var n = addObject(childFragment)

                declarationsResult = language.findDeclarations(childFragment.position() + 1, childFragment.length() - 2)
                if ( declarationsResult.hasReport() ){
                    for ( var j = 0; j < declarationsResult.report.length; ++j ){
                        lk.layers.workspace.messages.pushErrorObject(declarationsResult.report[i])
                    }
                }
                fragmentsResult = language.openChildConnections(childFragment, declarationsResult.value)
                if ( fragmentsResult.hasReport() ){
                    for ( var j = 0; j < fragmentsResult.report.length; ++j ){
                        lk.layers.workspace.messages.pushErrorObject(fragmentsResult.report[i])
                    }
                }

                var currentChildNestedFragments = fragmentsResult.value

                for (var j = 0; j < currentChildNestedFragments.length; ++j ){
                    var current = currentChildNestedFragments[j]
                    if ( current.location === QmlEditFragment.Object ){
                        n.item.control.addChildObject(current)
                    } else {
                        var p = n.item.control.addProperty(current)
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
                                    property: currentConnection.expression.length === 2 ? currentConnection.expression[1] : '',
                                    idOrProperty: currentConnection.expression.length === 1 ? currentConnection.expression[0] : ''
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
                    node = palette.findObjectById(propertyConnection.right.id)
                    if ( node && node.item.objectName !== 'objectNode' )
                        node = null
                } else if ( propertyConnection.right.idOrProperty ){
                    node = palette.findObjectById(propertyConnection.right.idOrProperty)
                    if ( node ){
                        propertyConnection.right.id = propertyConnection.right.idOrProperty
                    } else {
                        node = propertyConnection.left.node
                        propertyConnection.right.property = propertyConnection.right.idOrProperty
                    }
                }

                if (node){
                    if ( propertyConnection.right.property ){ // dealing with property of a node
                        var nodeProperty = node.item.control.propertyByName(propertyConnection.right.property)
                        if ( nodeProperty ){
                            propertyConnection.right.node = node
                            propertyConnection.right.outPort = nodeProperty.outPort
                            propertyConnection.right.inPort = nodeProperty.inPort
                        } else {
                            if ( propertyConnection.isFunction ){
                                nodeProperty = node.item.control.addFunctionProperty(propertyConnection.right.property)
                                propertyConnection.right.node = node
                                propertyConnection.right.inPort = nodeProperty.inPort
                                propertyConnection.right.outPort = nodeProperty.outPort
                            } else {
                                var position = node.item.editFragment.valuePosition() +
                                               node.item.editFragment.valueLength() - 1
                                var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
                                paletteFunctions.addPropertyToObjectContainer(node.item, propertyConnection.right.property, false, position)

                                nodeProperty = node.item.control.propertyByName(propertyConnection.right.property)
                                if ( nodeProperty ){
                                    propertyConnection.right.node = node
                                    propertyConnection.right.outPort = nodeProperty.outPort
                                    propertyConnection.right.inPort = nodeProperty.inPort
                                }
                            }
                        }
                    } else { // dealing with the node directly
                        propertyConnection.right.node = node
                        propertyConnection.right.outPort = node.objectName === 'objectNodeMember' ? node.outPort : node.item.outPort
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
                objectGraph.zoom = 600.0 / (numOfObjects * 420.0 + 50)
                objectGraph.zoomOrigin = 4
            }
        }

        function clean(){
            for (var i = 0; i < allObjects.length; ++i){
                if (!allObjects[i].item)
                    return

                var nodeItem = allObjects[i].item
                nodeItem.clean()
                editFragment.language.removeConnection(allObjects[i].item.control.editFragment)
            }

            allObjects = []
            objectsWithId = []
        }

        ObjectGraph {
            id: objectGraph
            width: parent.width
            height: parent.height
            onWidthChanged: {
                if ( objectGraph.resizeActive ){
                    nodeItem.sizeInfo = {
                        minWidth: objectGraph.width, maxWidth: objectGraph.width,
                        minHeight: objectGraph.height, maxHeight: objectGraph.height
                    }
                }
            }
            onHeightChanged: {
                if ( objectGraph.resizeActive ){
                    nodeItem.sizeInfo = {
                        minWidth: objectGraph.width, maxWidth: objectGraph.width,
                        minHeight: objectGraph.height, maxHeight: objectGraph.height
                    }
                }
            }
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
        editFragment.aboutToRemovePalette.connect(function(removedPalette){
            if ( removedPalette === palette ){
                nodeItem.clean()
            }
        })
    }
}
