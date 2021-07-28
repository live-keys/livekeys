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
            var objectList = editFragment.nestedObjectsInfo()
            var props = []
            for (var i = 0; i < objectList.length; ++i)
            {
                var object = objectList[i]
                var n = addObject(object)

                for (var j = 0; j < object.properties.length; ++j){
                    var property = object.properties[j]

                    var p = null
                    if (n.item.propertiesOpened.indexOf(property.name) === -1){
                        p = objectGraph.addObjectNodeProperty(n, property.name, ObjectGraph.PortMode.OutPort | (property.isWritable ? ObjectGraph.PortMode.InPort : 0), property.connection)
                        n.item.propertiesOpened.push(property.name)
                        p.z = 1000 - j
                    } else {
                        var children = n.item.propertyContainer.children
                        for (var idx = 0; idx < children.length; ++idx)
                            if (children[idx].propertyName === property.name){
                                p = children[idx]
                                break
                            }
                    }

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
                    objectGraph.addObjectNodeProperty(n, subobject.name + (subobject.id ? ("#" + subobject.id) : ""), subobject.id ? ObjectGraph.PortMode.OutPort : ObjectGraph.PortMode.None, subobject.connection)
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
                if (!allObjects[i].item) return
                var numofProps = allObjects[i].item.propertyContainer.children.length
                for (var j=0; j < numofProps; ++j){
                    var child = allObjects[i].item.propertyContainer.children[j]
                    if (child.editingFragment)
                        editFragment.codeHandler.removeConnection(child.editingFragment)
                    child.destroy()
                }
                editFragment.codeHandler.removeConnection(allObjects[i].item.editingFragment)
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
            editingFragment: palette.editFragment
            style: theme.nodeEditor
        }
    }


    onInit: {
        if (!editFragment) return
        editor = editFragment.codeHandler.documentHandler.textEdit().getEditor()
        editFragment.codeHandler.populateNestedObjectsForFragment(editFragment)
        objectGraph.editingFragment = editFragment
        nodeItem.init()
        editFragment.objectAdded.connect(function(obj, cursorCoords){
            addObject(obj.objectInfo(), cursorCoords)
        })
        editFragment.aboutToRemovePalette.connect(function(palette){
            nodeItem.clean()
        })
    }
}
