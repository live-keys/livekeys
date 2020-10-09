import QtQuick                   2.8
import QtQuick.Controls          2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts           1.3
import live                      1.0

import workspace 1.0 as Workspace
import workspace.quickqanava 2.0 as Qan

Rectangle{
    id: root
    
    width: 500
    height: 700
    clip: true

    objectName: "objectGraph"

    color: root.style.backgroundColor
    radius: root.style.radius
    border.width: root.style.borderWidth
    border.color: root.isInteractive ? root.style.highlightBorderColor : root.style.borderColor

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls
    property QtObject paletteStyle: lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    property QtObject defaultStyle : QtObject{
        property color backgroundColor: '#000511'
        property color backgroundGridColor: '#222'

        property color borderColor: '#333'
        property color highlightBorderColor: '#555'
        property double borderWidth: 1
        property double radius: 5

        property color connectorEdgeColor: '#666'
        property color connectorColor: '#666'

        property color selectionColor: "#fff"
        property double selectionWeight: 1

        property QtObject objectNodeStyle : QtObject{
            property color background: "yellow"
            property double radius: 15
            property color borderColor: "#555"
            property double borderWidth: 1

            property color titleBackground: "#666"
            property double titleRadius: 5
            property QtObject titleTextStyle : Workspace.TextStyle{}
        }

        property QtObject propertyDelegateStyle : QtObject{
            property color background: "#333"
            property double radius: 5
            property QtObject textStyle: Workspace.TextStyle{}
        }
    }

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

    Keys.onPressed: {
        if ( lk.layers.workspace ){
            var command = lk.layers.workspace.keymap.locateCommand(event.key, event.modifiers)
            if ( command ){
                lk.layers.workspace.commands.execute(command)
            }
            event.accepted = true
        }
    }

    property QtObject style: defaultStyle

    
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
    property Component propertyDestructorFactory: Component {
        Connections {
            target: null
            property var node: null
            ignoreUnknownSignals: true
            onPropertyToBeDestroyed: {
                node.item.removePropertyName(name)
            }
        }
    }

    property var isInteractive: root.activeFocus || selectedEdge || (numOfSelectedNodes > 0) || paletteListOpened
    property var connections: []
    property Component propertyDelegate : ObjectNodeProperty{
        style: root.style.propertyDelegateStyle
    }
    property alias nodeDelegate : graph.nodeDelegate
    property var palette: null
    property var documentHandler: null
    property var editor: null
    property var editingFragment: null

    property alias zoom: graphView.zoom
    property alias zoomOrigin: graphView.zoomOrigin

    property int inPort: 1
    property int outPort: 2
    property int noPort : 0
    property int inOutPort : 3
    
    property var selectedEdge: null
    property var numOfSelectedNodes: 0
    property var paletteListOpened: false

    signal userEdgeInserted(QtObject edge)
    signal nodeClicked(QtObject node)
    signal doubleClicked(var pos)
    signal rightClicked(var pos)
    signal edgeClicked(QtObject edge)


    onUserEdgeInserted: {
        var item = edge.item

        var srcPort = item.sourceItem
        var dstPort = item.destinationItem

        var name = srcPort.objectProperty.propertyName

        if (name.substr(0,2) === "on" && name.substr(name.length-7,7) === "Changed")
        {
            // source is event, different direction
            var nodeId = dstPort.objectProperty.node.item.id
            if (!nodeId) return
            if (dstPort.objectProperty.editingFragment) return

            var funcName = dstPort.objectProperty.propertyName
            var value = nodeId + "." + funcName + "()"

            var result = root.palette.extension.bindExpressionForFragment(
                srcPort.objectProperty.editingFragment,
                value
            )

            if ( result ){
                root.palette.extension.writeForFragment(
                    srcPort.objectProperty.editingFragment,
                    {'__ref': value}
                )
            }

        } else {
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
        if (selectedEdge)
            selectedEdge.item.color = '#6a6a6a'
        selectedEdge = null
    }

    onDoubleClicked: {
        var addBoxItem = paletteControls.createAddQmlBox(null, paletteStyle)
        if (!addBoxItem) return
        var position = editingFragment.valuePosition() + editingFragment.valueLength() - 1
        var addOptions = documentHandler.codeHandler.getAddOptions(position)

        addBoxItem.addContainer = addOptions

        addBoxItem.objectsOnly = true

        var rect = Qt.rect(pos.x, pos.y, 1, 1)
        var cursorCoords = Qt.point(pos.x, pos.y + 30)
        var addBox = lk.layers.editor.environment.createEditorBox(
            addBoxItem, rect, cursorCoords, lk.layers.editor.environment.placement.bottom
        )

        addBoxItem.accept = function(type, data){
            var opos = documentHandler.codeHandler.addItem(
                addBoxItem.addContainer.model.addPosition, addBoxItem.addContainer.objectType, data
            )
            documentHandler.codeHandler.addItemToRuntime(editingFragment, data, project.appRoot())
            var ef = documentHandler.codeHandler.openNestedConnection(
                editingFragment, opos
            )
            if (ef)
                editingFragment.signalObjectAdded(ef, cursorCoords)
            root.activateFocus()

            addBoxItem.destroy()
            addBox.destroy()
        }

        addBoxItem.cancel = function(){
            root.activateFocus()

            addBoxItem.destroy()
            addBox.destroy()
        }

        addBoxItem.assignFocus()

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
        var item = edge.item

        var srcPort = item.sourceItem
        var dstPort = item.destinationItem


        var value = ''

        var result = root.palette.extension.bindExpressionForFragment(
            dstPort.objectProperty.editingFragment, value
        )

        if ( result ){
            root.palette.extension.writeForFragment(
                dstPort.objectProperty.editingFragment, {'__ref': value ? value : dstPort.objectProperty.editingFragment.defaultValue()}
            )
        } else {
            qWarning("Failed to remove binding.")
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
        node.item.objectGraph = root

        var idx = label.indexOf('#')
        if (idx !== -1){
            node.item.id = label.substr(idx+1)
        }
        
        return node
    }
    
    function addObjectNodeProperty(node, propertyName, ports, editingFragment){
        var item = node.item
        var propertyItem = root.propertyDelegate.createObject(item.propertyContainer)

        propertyItem.propertyName = propertyName
        propertyItem.node = node

        propertyItem.width = node.item.width - 10
        propertyItem.editingFragment = editingFragment
        propertyItem.documentHandler = root.documentHandler

        if (editingFragment) editingFragment.incrementRefCount()

        propertyItem.editor = root.editor

        var conn = resizeConnectionsFactory.createObject()
        conn.target = propertyItem
        conn.node = item

        connections.push(conn)

        var pdestructor = propertyDestructorFactory.createObject()
        pdestructor.target = propertyItem
        pdestructor.node = node

        connections.push(pdestructor)

        if ( ports === root.inPort || ports === root.inOutPort ){
            var port = graph.insertPort(node, Qan.NodeItem.Left, Qan.Port.In);
            port.label = propertyName + " In"
            port.y = Qt.binding(
                function(){
                    return node.item.paletteContainer.height +
                           propertyItem.y +
                           (propertyItem.propertyTitle.height / 2) +
                           42
                }
            )

            propertyItem.inPort = port
            port.objectProperty = propertyItem
            port.multiplicity = Qan.PortItem.Single
        }
        if (ports === root.outPort || (node.item.id !== "" && ports === root.inOutPort) ){
            var port = graph.insertPort(node, Qan.NodeItem.Right, Qan.Port.Out);
            port.label = propertyName + " Out"
            port.y = Qt.binding(
                function(){
                    return node.item.paletteContainer.height +
                           propertyItem.y +
                           (propertyItem.propertyTitle.height / 2) +
                           42
                }
            )
            propertyItem.outPort = port
            port.objectProperty = propertyItem
        }
        
        node.item.properties.push(propertyItem)

        return propertyItem
    }
    
    Qan.GraphView {
        id: graphView
        anchors.fill: parent
        anchors.margins: 1
        navigable   : root.isInteractive
        gridThickColor: root.style.backgroundGridColor
        onDoubleClicked : root.doubleClicked(pos)
        onRightClicked : root.rightClicked(pos)
    
        graph: Qan.Graph {
            id: graph
            connectorEnabled: true
            connectorEdgeColor: root.style.connectorEdgeColor
            connectorColor: root.style.connectorColor
            edgeDelegate: Edge{}
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
                styleManager.styles.at(1).lineColor = root.style.connectorColor
            }
        }
    }  // Qan.GraphView

    Rectangle {
        anchors.fill: parent
        visible: !root.isInteractive
        color: "transparent"
        z: 3000
        MouseArea {
            id: ma
            enabled: !root.isInteractive
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onWheel: {
                wheel.accepted = false
            }
            onDoubleClicked: {
                root.doubleClicked(mouse)
            }
            onClicked: {
                root.activateFocus()
            }
        }
    }

    ResizeArea{
        minimumHeight: 200
        minimumWidth: 400
    }
}


