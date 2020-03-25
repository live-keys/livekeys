import QtQuick 2.3
import workspace.nodeeditor 1.0

Item{
    anchors.fill: parent
    
    ObjectGraph{
        id: objectGraph
        
        onDoubleClicked: console.log('Double clicked: ' + pos)
        onNodeClicked: console.log('Node clicked: ' + node + ', Item:' + node.item)
        onUserEdgeInserted: {
            var sourcePort = edge.item.sourceItem
            var destPort = edge.item.destinationItem
            
            console.log('From:' + sourcePort.objectProperty.node.label + '.' + sourcePort.objectProperty.propertyName)
            console.log('To:' + destPort.objectProperty.node.label + '.' + destPort.objectProperty.propertyName)
        }
        
        Component.onCompleted : {
            // add nodes and properties
            var n1 = objectGraph.addObjectNode(250, 100, 'N1')
            var p1 = objectGraph.addObjectNodeProperty(n1, 'Prop1', objectGraph.inPort)
            var p2 = objectGraph.addObjectNodeProperty(n1, 'Prop2', objectGraph.inOutPort)
            
            var n2 = objectGraph.addObjectNode(0, 30, 'N2')
            var n2p1 = objectGraph.addObjectNodeProperty(n2, 'Prop', objectGraph.outPort)
            
            // add connection
            objectGraph.bindPorts(n2p1.outPort, p2.inPort)

            // remove connection
            // objectGraph.unbindPorts(n2p1.outPort, p2.inPort)
            
            // remove object node
            // objectGraph.removeObjectNode(n1)
        }
    }
}
