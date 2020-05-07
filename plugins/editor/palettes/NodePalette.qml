import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0
import lcvcore 1.0
import workspace.nodeeditor 1.0

CodePalette{
    id: palette
    type: "qml/Object"

    property var editingFragment: null
    property var documentHandler: null
    property var editor: null
    property var objectsWithId: ({})
    property var numOfObjects: 0
    property var allObjects: []



    onEditingFragmentChanged: {
        if (!editingFragment) return

        nodeItem.init()
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
                var n = objectGraph.addObjectNode(numOfObjects *400 + 50, numOfObjects *200 + 50, (object.name + (object.id ? ("#" + object.id) : "")))

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

                for (var j = 0; j < object.properties.length; ++j){
                    var property = object.properties[j]
                    var p = objectGraph.addObjectNodeProperty(n, property.name, property.isWritable ? objectGraph.inOutPort : objectGraph.outPort, property.connection)
                    n.item.propertyNames.push(property.name)

                    p.z = 10000 - j
                    if (property.value.length === 2)
                    {
                        props.push({"port": p.inPort, "value": property.value})

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
                        if (propp.propertyName === props[k].value[1]){
                            objectGraph.bindPorts(propp.outPort, props[k].port)
                            break
                        }
                    }
                }
            }
        }

        function clean(){
            for (var i=0; i< allObjects.length; ++i){
                var numofProps = allObjects[i].item.propertyContainer.children.length
                for (var j=0; j < numofProps; ++j)
                    allObjects[i].item.propertyContainer.children[j].destroy()
                objectGraph.removeObjectNode(allObjects[i])
            }

            allObjects = []
            objectsWithId = []
            console.log(numOfObjects)
        }

        ObjectGraph {
            width: 500
            height: 300
            id: objectGraph
            palette: palette
            documentHandler: palette.documentHandler
            editor: palette.editor
        }
    }


    property Connections connTest: Connections{
        id: efConnection
        target: editingFragment
        onObjectAdded: {
            var object = obj.objectInfo()

            var n = objectGraph.addObjectNode(numOfObjects * 400 + 50, numOfObjects * 200 + 50, (object.name + (object.id ? ("#" + object.id) : "")))

            ++numOfObjects

            if (object.id)
                objectsWithId[object.id] = n

            allObjects.push(n)

            if (object.connection){
                n.item.editingFragment = object.connection
                object.connection.incrementRefCount()
            }
        }
        onAboutToRemovePalette: {
            nodeItem.clean()
        }
        ignoreUnknownSignals: true
    }

}
