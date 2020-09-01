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

    property var editingFragment: null
    property var documentHandler: null
    property var editor: null
    property var objectsWithId: ({})
    property var numOfObjects: 0
    property var allObjects: []

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    onEditingFragmentChanged: {
        if (!editingFragment) return

        nodeItem.init()
    }

    function addObject(object, cursorCoords){

        var n
        if (cursorCoords){
            n = objectGraph.addObjectNode(cursorCoords.x, cursorCoords.y, (object.name + (object.id ? ("#" + object.id) : "")))
        }
        else
            n = objectGraph.addObjectNode(numOfObjects *420 + 50, 50, (object.name + (object.id ? ("#" + object.id) : "")))

        ++numOfObjects

        if (object.id)
        {
           objectsWithId[object.id] = n
        }

        allObjects.push(n)

        if (object.connection){
            n.item.editingFragment = object.connection
            object.connection.incrementRefCount()
        }

        return n
    }

    item: Item {
        id: nodeItem

        width: objectGraph.width
        height: objectGraph.height

        function init(){
            var objectList = editingFragment.nestedObjectsInfo()
            var props = []
            for (var i = 0; i < objectList.length; ++i)
            {
                var object = objectList[i]
                var n = addObject(object)

                for (var j = 0; j < object.properties.length; ++j){
                    var property = object.properties[j]
                    var p = objectGraph.addObjectNodeProperty(n, property.name, property.isWritable ? objectGraph.inOutPort : objectGraph.outPort, property.connection)
                    n.item.propertiesOpened.push(property.name)
                    p.z = 1000 - j
                    if (property.value.length === 2)
                    {
                        if (p.inPort)
                            props.push({"port": p.inPort, "value": property.value})
                        else
                            props.push({"value": property.value})
                    }
                }


                for (var so = 0; so < object.subobjects.length; ++so)
                {
                    var subobject = object.subobjects[so]
                    objectGraph.addObjectNodeProperty(n, subobject.name + (subobject.id ? ("#" + subobject.id) : ""), objectGraph.noPort, subobject.connection)
                }

            }

            for (var k = 0; k < props.length; ++k){
                var id = props[k].value[0]
                var node = objectsWithId[id]
                if (node)
                {
                    var nodeProps = node.item.properties
                    for (var pp = 0; pp < nodeProps.length; ++pp)
                    {
                        var propp = node.item.properties[pp]
                        var found = false
                        if (propp.propertyName === props[k].value[1]){
                            objectGraph.bindPorts(propp.outPort, props[k].port)
                            found = true
                            break
                        }

                    }
                    if (!found){
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
                var numofProps = allObjects[i].item.propertyContainer.children.length
                for (var j=0; j < numofProps; ++j){
                    var child = allObjects[i].item.propertyContainer.children[j]
                    if (child.editingFragment)
                        palette.documentHandler.codeHandler.removeConnection(child.editingFragment)
                    child.destroy()
                }
                palette.documentHandler.codeHandler.removeConnection(allObjects[i].item.editingFragment)
            }

            allObjects = []
            objectsWithId = []
        }

        ObjectGraph {
            width: 600
            height: 300
            id: objectGraph
            palette: palette
            documentHandler: palette.documentHandler
            editor: palette.editor
            editingFragment: palette.editingFragment
            style: palette ? palette.paletteStyle.nodeEditor : defaultStyle
        }
    }


    property Connections connTest: Connections{
        id: efConnection
        target: editingFragment
        onObjectAdded: {
            addObject(obj.objectInfo(), cursorCoords)
        }
        onAboutToRemovePalette: {
            nodeItem.clean()
        }
        ignoreUnknownSignals: true
    }

}
