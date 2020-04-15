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
    property var objects: ({})
    property var edges: []

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
                var n = objectGraph.addObjectNode(i*250 + 50, 100, (object.name + (object.id ? ("#" + object.id) : "")))

                if (object.id)
                {
                   objects[object.id] = n
                }

                if (object.connection)
                    n.fragment = object.connection

                for (var j = 0; j < object.properties.length; ++j)
                {
                    var property = object.properties[j]
                    var p = objectGraph.addObjectNodeProperty(n, property.name, property.isWritable ? objectGraph.inOutPort : objectGraph.outPort, null)
                    n.item.propertyNames.push(property.name)
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

            for (var k = 0; k < props.length; ++k)
            {
                var id = props[k].value[0]
                var node = objects[id]
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

        ObjectGraph {
            width: 500
            height: 300
            id: objectGraph
        }
    }


    property Connections connTest: Connections{
        id: efConnection
        target: editingFragment
        onObjectAdded: {
            var object = obj.objectInfo()

            var n = objectGraph.addObjectNode(300, 300, (object.name + (object.id ? ("#" + object.id) : "")))

            if (object.id)
                objects[object.id] = n
        }
        ignoreUnknownSignals: true
    }

}
