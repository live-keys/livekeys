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

        var srcLocation = srcPort.objectProperty.editFragment.location

        if (srcLocation === QmlEditFragment.Slot)
        {
            // source is event, different direction
            var nodeId = dstPort.objectProperty.node.item.id

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
        var addOptions = root.editFragment.language.getAddOptions(position)

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
                                var opos = ch.addItem(selection.position, selection.objectType, selection.name, propertiesToAdd)
                                ch.addItemToRuntime(editFragment, selection.name, propertiesToAdd)
                                var ef = ch.openNestedConnection(editFragment, opos)
                                if (ef)
                                    editFragment.signalChildAdded(ef)

                            },
                            onCancelled: function(){}
                        })


                    } else {
                        var ch = editFragment.language
                        var opos = ch.addItem(selection.position, selection.objectType, selection.name)
                        ch.addItemToRuntime(editFragment, selection.name)
                        var ef = ch.openNestedConnection(editFragment, opos)
                        if (ef)
                            editFragment.signalChildAdded(ef)
                    }
                    box.child.finalize()
                }
            }
        )
    }

    function bindPorts(src, dst){
        var srcNode = src.objectProperty.node
        var dstNode = dst.objectProperty.node
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
    
    function addObjectNode(x, y, label){
        var node = graph.insertNode()
        node.item.nodeParent = node
        node.item.connectable = Qan.NodeItem.UnConnectable
        node.item.x = x
        node.item.y = y
        node.item.label = label
        node.label = label

        node.item.editor = editor
        node.item.objectGraph = root

        var idx = label.indexOf('#')
        if (idx !== -1){
            node.item.id = label.substr(idx+1)

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
    }  // Qan.GraphView

    ResizeArea{
        minimumHeight: 200
        minimumWidth: 400
        onResizeFinished: { root.redrawGrid() }
    }
}


