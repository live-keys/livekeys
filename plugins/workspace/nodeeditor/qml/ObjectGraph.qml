import QtQuick                   2.8
import QtQuick.Controls          2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts           1.3
import live                      1.0

import workspace.quickqanava 2.0 as Qan

Rectangle{
    id: root
    
    width: 500
    height: 700
    color: '#000511'
    clip: true
    radius: 5
    border.width: 1
    border.color: "#333"
    
    property Component resizeConnectionsFactory: Component{
        Connections {
            target: null
            property var node: null
            ignoreUnknownSignals: true
            onWidthChanged: {
                var maxWidth = 340
                for (var i=0; i < node.propertyContainer.children.length; ++i)
                {
                    var child = node.propertyContainer.children[i]
                    if (child.width > maxWidth) maxWidth = child.width
                }

                if (maxWidth !== node.width)
                    node.width = maxWidth + 20
            }
        }
    }
    property var resizeComponents: []
    property Component propertyDelegate : ObjectNodeProperty{}
    property alias nodeDelegate : graph.nodeDelegate
    property var palette: null
    property var documentHandler: null
    property var editor: null


    property int inPort: 1
    property int outPort: 2
    property int noPort : 0
    property int inOutPort : 3
    
    signal userEdgeInserted(QtObject edge)
    signal nodeClicked(QtObject node)
    signal doubleClicked(var pos)
    signal rightClicked(var pos)
    
    onUserEdgeInserted: {
        var item = edge.item

        var srcPort = item.sourceItem
        var dstPort = item.destinationItem

        var value =
                srcPort.objectProperty.node.item.id + "." + srcPort.objectProperty.propertyName

        var result = root.palette.extension.bindExpressionForFragment(
            dstPort.objectProperty.editingFragment,
            value
        )
        if ( result ){
            root.palette.extension.writeForFragment(
                dstPort.objectProperty.editingFragment,
                {'__ref': value}
            )
        }
    }

    function bindPorts(src, dst){
        var srcNode = src.objectProperty.node
        var dstNode = dst.objectProperty.node
        
        var edge = graph.insertEdge(srcNode, dstNode, graph.edgeDelegate)
        graph.bindEdge(edge, src, dst)
        
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
        graph.removeEdge(edge)
    }


    function removeObjectNode(node){
        --palette.numOfObjects
        graph.removeNode(node)
    }
    
    function addObjectNode(x, y, label){
        var node = graph.insertNode()
        node.item.nodeParent = node
        node.item.removeNode = removeObjectNode
        node.item.addSubobject = addObjectNodeProperty
        node.item.connectable = Qan.NodeItem.UnConnectable
        node.item.x = x
        node.item.y = y
        node.item.label = label
        node.label = label

        node.item.documentHandler = documentHandler
        node.item.editor = editor
        node.item.editingFragment = node.fragment

        var idx = label.indexOf('#')
        if (idx !== -1)
        {
            node.item.id = label.substr(idx+1)
        }
        
        return node
    }
    
    function addObjectNodeProperty(node, propertyName, ports, editingFragment){
        var item = node.item
        var propertyItem = root.propertyDelegate.createObject(item.propertyContainer)

        propertyItem.propertyName = propertyName
        propertyItem.node = node

        propertyItem.editingFragment = editingFragment
        propertyItem.documentHandler = root.documentHandler

        propertyItem.editor = root.editor

        var conn = resizeConnectionsFactory.createObject()
        conn.target = propertyItem
        conn.node = item

        resizeComponents.push(conn)

        if ( ports === root.inPort || ports === root.inOutPort ){
            var port = graph.insertPort(node, Qan.NodeItem.Left, Qan.Port.In);
            port.label = propertyName + " In"
            port.y = Qt.binding(function(){ return propertyItem.y + 42 + (propertyItem.propertyTitle.height / 2) })
            propertyItem.inPort = port
            port.objectProperty = propertyItem
        }
        if ( node.item.id !== "" && (ports === root.outPort || ports === root.inOutPort) ){
            var port = graph.insertPort(node, Qan.NodeItem.Right, Qan.Port.Out);
            port.label = propertyName + " Out"
            port.y = Qt.binding(function(){ return propertyItem.y + 42 + (propertyItem.propertyTitle.height / 2) })
            propertyItem.outPort = port
            port.objectProperty = propertyItem
        }
        
        node.item.properties.push(propertyItem)

        return propertyItem
    }
    
    Qan.GraphView {
        id: graphView
        anchors.fill: parent
        navigable   : true
        gridThickColor: '#222'
        onDoubleClicked : root.doubleClicked(pos)
        onRightClicked : root.rightClicked(pos)
    
        graph: Qan.Graph {
            id: graph
            connectorEnabled: true
            connectorEdgeColor: "#666"
            connectorColor: "#666"
            edgeDelegate: Edge{}
            verticalDockDelegate : VerticalDock{}
            portDelegate: Port{}
            connectorItem : PortConnector{}
            onNodeClicked : root.nodeClicked(node)
            onConnectorEdgeInserted : root.userEdgeInserted(edge)
            nodeDelegate: ObjectNode{}
            Component.onCompleted : {
                styleManager.styles.at(1).lineColor = '#666'
            }
        }
    }  // Qan.GraphView
    
    ResizeArea{
        minimumHeight: 200
        minimumWidth: 400
    }
}


