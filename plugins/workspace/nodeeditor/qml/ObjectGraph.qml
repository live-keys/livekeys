import QtQuick                   2.8
import QtQuick.Controls          2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts           1.3
import live                      1.0

import workspace.quickqanava 2.0 as Qan
import editqml 1.0
import visual.input 1.0 as Input

Rectangle{
    id: root
    objectName: "objectGraph"
    
    width: 500
    height: 700
    clip: true

    color: root.style.backgroundColor
    radius: root.style.radius
    border.width: root.style.borderWidth
    border.color: root.isInteractive ? root.style.highlightBorderColor : root.style.borderColor

    property alias resizeActive: resizeArea.resizeActive

    property QtObject style: ObjectGraphStyle{}

    function activateFocus(){
        if ( activeFocus )
            return

        if ( lk.layers.workspace ){
            var p = root
            while ( p && !p.paneType ){
                p = p.parent
            }
            if( p ){
                lk.layers.workspace.panes.activateItem(root, p)
            } else {
                root.forceActiveFocus()
            }
        } else {
            root.forceActiveFocus()
        }
    }

    onActiveFocusChanged: {
        checkFocus()
    }

    function redrawGrid(){
        graphView.requestUpdateGrid()
    }

    Keys.onPressed: {
        if ( lk.layers.workspace ){
            var command = lk.layers.workspace.keymap.locateCommand(event.key, event.modifiers)
            if ( command ){
                lk.layers.workspace.commands.execute(command)
            }
            event.accepted = true
        }
    }

    function checkFocus(){

        var interac = false
        if ( activeFocus ){
            interac = true
        }
        if ( activeItem )
            interac = true

        isInteractive = interac
    }

    property var insertPort: graph.insertPort
    property var isInteractive: false

    property var connections: []

    property Component propertyDelegate : ObjectNodeMember{
        style: root.style.objectNodeMemberStyle
    }

    property Item activeItem: null

    property alias nodeDelegate : graph.nodeDelegate
    property var palette: null
    property var editor: null
    property var editFragment: null

    onEditFragmentChanged: {
        if (!editFragment)
            return
        editor = editFragment.language.code.textEdit().getEditor()
    }

    property alias zoom: graphView.zoom
    property alias zoomOrigin: graphView.zoomOrigin

    enum PortMode {
        None = 0,
        InPort = 1,
        OutPort = 2
    }
    
    property var selectedEdge: null
    property var numOfSelectedNodes: 0
    property var paletteListOpened: false

    signal userEdgeInserted(QtObject edge)
    signal nodeClicked(QtObject node)
    signal doubleClicked(var pos)
    signal rightClicked(var pos)
    signal edgeClicked(QtObject edge)
    signal clicked()

    Connections{
        target: editFragment
        function onChildAdded(ef, context){
            var objectName = ef.typeName()
            var objectId = ef.objectId()

            var cursorCoords = null
            if ( context && context.location === 'ObjectGraph.AddObject' ){
                cursorCoords = context.coords
            }

            var n = null
            if (cursorCoords){
                n = root.addObjectNode(
                        cursorCoords.x,
                        cursorCoords.y,
                        ef
                    )
            } else {
                n = root.addObjectNode(graph.getNodeCount() * 420 + 50, 50, (objectName + (objectId ? ("#" + objectId) : "")))
            }
            n.item.control.editFragment = ef
            ef.incrementRefCount()

            n.item.control.expandDefaultPalette()
        }
    }

    onClicked: {
        deselectEdge()
    }

    onUserEdgeInserted: {
        var item = edge.item

        var srcPort = item.sourceItem
        var dstPort = item.destinationItem

        if (dstPort.inEdge){
            removeEdge(dstPort.inEdge)
        }

        if (dstPort.objectName === "objectNode"){
            removeEdge(edge)
            return
        }

        var name = ""
        var value = ""
        if (!srcPort.objectProperty.propertyName){
            name = srcPort.objectProperty.id
            value = name
        }
        else {
            name = srcPort.objectProperty.propertyName
            if (name.indexOf('#') !== -1)
                value = name.substr(name.indexOf('#') +1)
            else
                value = srcPort.objectProperty.node.item.id + "." + srcPort.objectProperty.propertyName

        }

        var srcLocation = srcPort.objectProperty.control.editFragment.location

        if (srcLocation === QmlEditFragment.Slot)
        {
            // source is event, different direction
            var nodeId = dstPort.objectProperty.node.item.control.nodeId

            if (nodeId === "") return
            // if (dstPort.objectProperty.editFragment) return

            var funcName = dstPort.objectProperty.propertyName
            value = nodeId + "." + funcName + "()"

            if (!srcPort.objectProperty.editFragment)
                return

            var result = srcPort.objectProperty.editFragment.bindFunctionExpression(value)
            if ( result ){
                srcPort.objectProperty.editFragment.write(
                    {'__ref': value}
                )
            }
        } else {
            if (dstPort.objectProperty.editFragment){
                var result = dstPort.objectProperty.editFragment.bindExpression(value)
                if ( result ){
                    dstPort.objectProperty.editFragment.write(
                        {'__ref': value}
                    )
                }
            }
        }

        srcPort.outEdges.push(edge)
        dstPort.inEdge = edge
    }

    onEdgeClicked: {
        root.activateFocus()
        if (selectedEdge){
            selectedEdge.item.color = '#3f444d'
        }
        selectedEdge = edge
        edge.item.color = '#dbdede'
    }

    onNodeClicked: {
        deselectEdge()
    }

    function deselectEdge(){
        if (selectedEdge)
            selectedEdge.item.color = '#3f444d'
        selectedEdge = null
    }

    onDoubleClicked: {
        var position = root.editFragment.valuePosition() + root.editFragment.valueLength() - 1
        var addOptions = root.editFragment.language.getAddOptions({ editFragment: root.editFragment })

        var coords = root.editor.parent.mapGlobalPosition()
        var cursorCoords = Qt.point(coords.x, coords.y)

        var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions

        var addBoxItem = paletteFunctions.views.openAddOptionsBox(
            addOptions,
            root.editFragment.language,
            {
                aroundRect: Qt.rect(pos.x, pos.y, 1, 1),
                panePosition: cursorCoords,
                relativePlacement: lk.layers.editor.environment.placement.bottom
            },
            {
                categories: ['objects'],
                onCancelled: function(box){
                    box.child.finalize()
                },
                onFinalized: function(box){
                    root.activateFocus()
                    box.child.destroy()
                    box.destroy()
                },
                onAccepted: function(box, selection){
                    if ( selection.extraProperties ){
                        paletteFunctions.views.openAddExtraPropertiesBox(selection.name, {
                            onAccepted: function(propertiesToAdd){
                                var ch = editFragment.language
                                var opos = ch.addObjectToCode(selection.position, { type: selection.name, id: selection.id }, propertiesToAdd)
                                ch.createObjectInRuntime(editFragment, { type: selection.name, id: selection.id }, propertiesToAdd)

                                var declaration = ch.findDeclaration(opos)
                                var fragmentsResult = ch.openChildConnections(editFragment, [declaration])
                                if ( fragmentsResult.hasReport() ){
                                    for ( var i = 0; i < fragmentsResult.report.length; ++i ){
                                        lk.layers.workspace.messages.pushErrorObject(fragmentsResult.report[i])
                                    }
                                }

                                var ef = fragmentsResult.value.length ? fragmentsResult.value[0] : null

                                cursorCoords = Qt.point((pos.x - graphView.containerItem.x ) / zoom, (pos.y - graphView.containerItem.y) / zoom)
                                if (ef){
                                    editFragment.signalChildAdded(ef, {location: 'ObjectGraph.AddObject', coords: cursorCoords })
                                } else {
                                    lk.layers.workspace.messages.pushError("Error: Failed to create object child: " + selection.name, 1)
                                }
                            },
                            onCancelled: function(){}
                        })


                    } else {
                        var ch = editFragment.language
                        var opos = ch.addObjectToCode(selection.position, { type: selection.name, id: selection.id } )
                        ch.createObjectInRuntime(editFragment, { type: selection.name, id: selection.id })

                        var declaration = ch.findDeclaration(opos)
                        var fragmentsResult = ch.openChildConnections(editFragment, [declaration])
                        if ( fragmentsResult.hasReport() ){
                            for ( var i = 0; i < fragmentsResult.report.length; ++i ){
                                lk.layers.workspace.messages.pushErrorObject(fragmentsResult.report[i])
                            }
                        }

                        var ef = fragmentsResult.value.length ? fragmentsResult.value[0] : null

                        cursorCoords = Qt.point((pos.x - graphView.containerItem.x ) / zoom, (pos.y - graphView.containerItem.y) / zoom)
                        if (ef)
                            editFragment.signalChildAdded(ef, {location: 'ObjectGraph.AddObject', coords: cursorCoords} )
                    }
                    box.child.finalize()
                }
            }
        )
    }

    function bindPorts(src, dst){
        var srcNode = src.objectProperty.control.node
        var dstNode = dst.objectProperty.control.node
        var edge = null
        if (srcNode === dstNode){
            edge = graph.insertEdge(srcNode, dstNode, graph.edgeDelegateCurved)
        } else {
            edge = graph.insertEdge(srcNode, dstNode, graph.edgeDelegate )
        }

        graph.bindEdge(edge, src, dst)
        
        src.outEdges.push(edge)
        dst.inEdge = edge

        return edge
    }

    function findEdge(src, dst){
        var srcEdges = src.node.outEdges
        for ( var i = 0; i < srcEdges.length; ++i ){
            var edge = srcEdges.at(i)
            if ( edge.item.sourceItem === src && edge.item.destinationItem === dst)
                return edge
        }
        return null
    }

    function unbindPorts(src, dst){
        var edge = findEdge(src, dst)
        removeEdge(edge)
    }

    function removeEdge(edge){
        var item = edge.item

        var srcPort = item.sourceItem
        var dstPort = item.destinationItem

        if (dstPort.objectProperty && dstPort.objectProperty.editFragment){
            var ef = dstPort.objectProperty.editFragment
            var value = ef.defaultValue()
            var result = ef.bindExpression('null')
            dstPort.objectProperty.editFragment.write(
                {'__ref': value}
            )
            ef.commit(value)
        }

        graph.removeEdge(edge)
        if ( edge === selectedEdge )
            selectedEdge = null
    }

    function removeObjectNode(node){
        --palette.numOfObjects
        if (node.item.selected)
            --numOfSelectedNodes
        if (numOfSelectedNodes === 0)
            root.activateFocus()

        // clear everything inside node

        node.item.clean()

        if (node.item.outPort)
            graph.removePort(node, node.item.outPort)

        graph.removeNode(node)
    }
    
    function addObjectNode(x, y, editFragment){
        var node = graph.insertNode()
        node.item.__initialize(editFragment)
        node.item.control.nodeParent = node
        node.item.connectable = Qan.NodeItem.UnConnectable
        node.item.x = x
        node.item.y = y
        node.label = node.item.control.label

        node.item.control.objectGraph = root

        if (node.item.control.nodeId !== ''){
            var port = graph.insertPort(node, Qan.NodeItem.Right, Qan.Port.Out);
            node.item.outPort = port
            port.label = node.item.id + " Out"
            port.y = 7
            port.objectProperty = node.item
        }
        
        return node
    }
    
    Qan.GraphView {
        id: graphView
        anchors.fill: parent
        anchors.margins: 1
        navigable   : true
        gridThickColor: root.style.backgroundGridColor
        onDoubleClicked : root.doubleClicked(pos)
        onRightClicked : root.rightClicked(pos)
        onPressed: { root.activateFocus() }
        onClicked: root.clicked()
    
        graph: Qan.Graph {
            id: graph
            connectorEnabled: true
            connectorEdgeColor: root.style.connectorEdgeColor
            connectorColor: root.style.connectorColor
            edgeDelegate: Edge{}
            property Component edgeDelegateCurved: Edge { lineType: Qan.EdgeStyle.Curved }
            verticalDockDelegate : VerticalDock{}
            portDelegate: Port{}
            selectionDelegate: Selection{}
            connectorItem : PortConnector{}
            onEdgeClicked: root.edgeClicked(edge)
            onNodeClicked : root.nodeClicked(node)
            onConnectorEdgeInserted : root.userEdgeInserted(edge)
            selectionColor: "#fff"
            selectionWeight: 1
            nodeDelegate: ObjectNode{
                nodeStyle: root.style.objectNodeStyle
            }
            Component.onCompleted : {
                graph.connector.edgeComponent = graph.edgeDelegate
                graph.connector.createEdgeHook = function(src, dst){
                    if (src === dst)
                        return graph.edgeDelegateCurved
                    return graph.edgeDelegate
                }
                graphView.navigable = Qt.binding(function(){ return root.isInteractive })
                styleManager.styles.at(1).lineColor = root.style.connectorColor
            }
        }
    }

    ResizeArea{
        id: resizeArea
        minimumHeight: 200
        minimumWidth: 400
        onResizeFinished: { root.redrawGrid() }
    }
}


