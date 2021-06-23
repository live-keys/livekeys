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
    
    width: 500
    height: 700
    clip: true

    objectName: "objectGraph"

    color: root.style.backgroundColor
    radius: root.style.radius
    border.width: root.style.borderWidth
    border.color: root.isInteractive ? root.style.highlightBorderColor : root.style.borderColor

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls
    property QtObject theme: lk.layers.workspace.themes.current

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
            property QtObject titleTextStyle : Input.TextStyle{}
        }

        property QtObject propertyDelegateStyle : QtObject{
            property color background: "#333"
            property double radius: 5
            property QtObject textStyle: Input.TextStyle{}
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

    property QtObject style: defaultStyle

    
    property Component propertyDestructorFactory: Component {
        Connections {
            id: pdf
            target: null
            property var node: null
            ignoreUnknownSignals: true
            function onPropertyToBeDestroyed(name){
                node.item.removePropertyName(name)
                pdf.destroy()
            }
            function onWidthChanged(){
                var maxWidth = 340
                if (!node || !node.propertyContainer) return
                for (var i=0; i < node.propertyContainer.children.length; ++i)
                {
                    var child = node.propertyContainer.children[i]
                    if (child.width > maxWidth) maxWidth = child.width
                }

                if (maxWidth !== node.width)
                    node.width = maxWidth
            }
        }
    }

    onActiveFocusChanged: {
        checkFocus()
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

    property var isInteractive: false
    property var connections: []
    property Component propertyDelegate : ObjectNodeProperty{
        style: root.style.propertyDelegateStyle
    }

    property Item activeItem: null

    property alias nodeDelegate : graph.nodeDelegate
    property var palette: null
    property var documentHandler: null
    property var editor: null
    property var editingFragment: null

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
            value = srcPort.objectProperty.node.item.id + "." + srcPort.objectProperty.propertyName
        }

        var srcLocation = srcPort.objectProperty.editingFragment.location

        if (srcLocation === QmlEditFragment.Slot)
        {
            // source is event, different direction
            var nodeId = dstPort.objectProperty.node.item.id

            if (nodeId === "") return
            // if (dstPort.objectProperty.editingFragment) return

            var funcName = dstPort.objectProperty.propertyName
            value = nodeId + "." + funcName + "()"

            if (!srcPort.objectProperty.editingFragment)
                return

            var result = srcPort.objectProperty.editingFragment.bindExpression(value)
            if ( result ){
                srcPort.objectProperty.editingFragment.write(
                    {'__ref': value}
                )
            }
        } else {
            if (dstPort.objectProperty.editingFragment){
                var result = dstPort.objectProperty.editingFragment.bindExpression(value)
                if ( result ){
                    dstPort.objectProperty.editingFragment.write(
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
        var addBoxItem = paletteControls.createAddQmlBox(null)
        if (!addBoxItem) return
        var position = editingFragment.valuePosition() + editingFragment.valueLength() - 1
        var addOptions = documentHandler.codeHandler.getAddOptions(position)

        addBoxItem.addContainer = addOptions
        addBoxItem.codeQmlHandler = documentHandler.codeHandler

        addBoxItem.mode = AddQmlBox.DisplayMode.ObjectsOnly

        var rect = Qt.rect(pos.x, pos.y, 1, 1)
        var coords = editor.mapGlobalPosition()
        var cursorCoords = Qt.point(coords.x, coords.y)
        var addBox = lk.layers.editor.environment.createEditorBox(
            addBoxItem, rect, cursorCoords, lk.layers.editor.environment.placement.bottom
        )
        addBox.color = 'transparent'

        addBoxItem.accept = function(type, data){
            var opos = documentHandler.codeHandler.addItem(
                addBoxItem.addContainer.model.addPosition, addBoxItem.addContainer.objectType, data
            )
            documentHandler.codeHandler.addItemToRuntime(editingFragment, data, project.appRoot())
            var ef = documentHandler.codeHandler.openNestedConnection(
                editingFragment, opos
            )
            cursorCoords = Qt.point((pos.x - graphView.containerItem.x ) / zoom, (pos.y - graphView.containerItem.y) / zoom)

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



        if (dstPort.objectProperty && dstPort.objectProperty.editingFragment){
            var ef = dstPort.objectProperty.editingFragment
            var value = ef.defaultValue()
            var result = ef.bindExpression('null')
            dstPort.objectProperty.editingFragment.write(
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

        var children = node.item.propertyContainer.children
        for (var i = 0; i < children.length; ++i){
            children[i].destroyObjectNodeProperty()
        }

        if (node.item.outPort)
            graph.removePort(node, node.item.outPort)

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

            var port = graph.insertPort(node, Qan.NodeItem.Right, Qan.Port.Out);
            node.item.outPort = port
            port.label = node.item.id + " Out"
            port.y = 7
            port.objectProperty = node.item
        }
        
        return node
    }
    
    function addObjectNodeProperty(node, propertyName, ports, editingFragment, options){
        var item = node.item
        var propertyItem = root.propertyDelegate.createObject(item.propertyContainer)

        propertyItem.propertyName = propertyName
        propertyItem.node = node

        propertyItem.editingFragment = editingFragment
        if ( options && options.hasOwnProperty('isMethod') ){
            propertyItem.isMethod = options.isMethod
        }

        var isForObject = propertyItem.isForObject
        propertyItem.width = node.item.width - (isForObject ? 30 : 0)

        propertyItem.documentHandler = root.documentHandler

        if (editingFragment) editingFragment.incrementRefCount()

        propertyItem.editor = root.editor

        var pdestructor = propertyDestructorFactory.createObject()
        pdestructor.target = propertyItem
        pdestructor.node = node

        connections.push(pdestructor)

        if ( ports & ObjectGraph.PortMode.InPort ){
            var port = graph.insertPort(node, Qan.NodeItem.Left, Qan.Port.In);
            port.label = propertyName + " In"
            port.y = Qt.binding(
                function(){
                    if (!node.item) return 0
                    return node.item.paletteContainer.height +
                           propertyItem.y +
                           (propertyItem.propertyTitle.height / 2) +
                           42
                }
            )

            propertyItem.inPort = port
            port.objectProperty = propertyItem
        }
        if ((ports === ObjectGraph.PortMode.OutPort) || (node.item.id !== "" && ports === (ObjectGraph.PortMode.OutPort | ObjectGraph.PortMode.InPort)) ){
            var port = graph.insertPort(node, Qan.NodeItem.Right, Qan.Port.Out);
            port.label = propertyName + " Out"
            port.y = Qt.binding(
                function(){
                    if (!node || !node.item) return 0
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
        onResizeFinished: { graphView.redrawGrid() }
    }
}


