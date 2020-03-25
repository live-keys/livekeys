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
    
    property Component propertyDelegate : ObjectNodeProperty{}
    property alias nodeDelegate : graph.nodeDelegate
    
    property int inPort: 1
    property int outPort: 2
    property int noPort : 0
    property int inOutPort : 4
    
    signal userEdgeInserted(QtObject edge)
    signal nodeClicked(QtObject node)
    signal doubleClicked(var pos)
    signal rightClicked(var pos)
    
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
        graph.removeNode(node)
    }
    
    function addObjectNode(x, y, label){
        var node = graph.insertNode()
        
        node.item.connectable = Qan.NodeItem.UnConnectable
        node.item.x = x
        node.item.y = y
        node.item.label = label
        node.label = label
        
        return node
    }
    
    function addObjectNodeProperty(node, propertyName, ports){
        var item = node.item
        var propertyItem = root.propertyDelegate.createObject()
        propertyItem.parent = item.propertyContainer
        propertyItem.propertyName = propertyName
        propertyItem.node = node
        
        if ( ports === root.inPort || ports === root.inOutPort ){
            var port = graph.insertPort(node, Qan.NodeItem.Left, Qan.Port.In);
            port.label = propertyName + " In"
            port.y = Qt.binding(function(){ return propertyItem.y + 27 + (propertyItem.height / 2) })
            propertyItem.inPort = port
            port.objectProperty = propertyItem
        }
        if ( ports === root.outPort || ports === root.inOutPort ){
            var port = graph.insertPort(node, Qan.NodeItem.Right, Qan.Port.Out);
            port.label = propertyName + " Out"
            port.y = Qt.binding(function(){ return propertyItem.y + 27 + (propertyItem.height / 2) })
            propertyItem.outPort = port
            port.objectProperty = propertyItem
        }
        
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


