import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import editqml 1.0

import workspace.quickqanava 2.0 as Qan
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons
import visual.input 1.0 as Input

Item{
    id: root
    objectName: "objectNodeMember"

    anchors.left: parent.left
    anchors.leftMargin: isAnObject ? 30 : 0
    height: propertyTitle.height + paletteContainer.height
    property int contentWidth: 355 - anchors.leftMargin
    onWidthChanged: {
        var maxWidth = 340
        if ( !root.node || !root.node.propertyContainer)
            return

        for (var i=0; i < root.node.propertyContainer.children.length; ++i){
            var child = root.node.propertyContainer.children[i]
            if (child.width > maxWidth)
                maxWidth = child.width
        }

        if (maxWidth !== node.width)
            root.node.width = maxWidth
    }

    z: -1

    property QtObject style: ObjectNodeMemberStyle{}

    property string propertyName : ''
    property string propertyType: ''
    property alias propertyTitle: propertyTitle
    property alias paletteListContainer: paletteContainer

    property string title: ''
    property string subtitle: ''
    
    property Item inPort : null
    property Item outPort : null
    property QtObject node : null
    property var editFragment: null

    property var isAnObject: false
    property bool isMethod: false

    signal updateContentWidth()
    onUpdateContentWidth: paletteContainer.updateContentWidth()

    function addInPort(){
        if ( root.inPort )
            return

        var port = root.node.item.objectGraph.insertPort(root.node, Qan.NodeItem.Left, Qan.Port.In);
        port.label = root.propertyName + " In"
        port.y = Qt.binding(
            function(){
                if (!root.node.item)
                    return 0
                return root.node.item.paletteListContainer.height +
                       root.y +
                       (root.propertyTitle.height / 2) +
                       42
            }
        )

        root.inPort = port
        port.objectProperty = root
    }

    function addOutPort(){
        if (root.outPort )
            return

        var port = root.node.item.objectGraph.insertPort(root.node, Qan.NodeItem.Right, Qan.Port.Out);
        port.label = root.propertyName + " Out"
        port.y = Qt.binding(
            function(){
                if (!root.node || !root.node.item) return 0
                return root.node.item.paletteListContainer.height +
                       root.y +
                       (root.propertyTitle.height / 2) +
                       42
            }
        )
        root.outPort = port
        port.objectProperty = root
    }

    function __initialize(node, style, editFragment){
        root.style = style
        root.propertyName = editFragment.identifier()
        root.title = root.propertyName
        root.subtitle = editFragment.typeName()
        root.node = node
        root.editFragment = editFragment

        //TODO: Handle functions
        //        if ( options && options.hasOwnProperty('isMethod') ){
        //            propertyItem.isMethod = options.isMethod
        //        }

        if ( editFragment ) {
            if ( editFragment.location === QmlEditFragment.Object ){
                root.isAnObject = true
                root.title = editFragment.typeName()
                root.subtitle = editFragment.objectId()
            }
            editFragment.incrementRefCount()
        }
        root.width = node.item.width - (root.isAnObject ? 30 : 0)
    }

    function destroyObjectNodeProperty(){
        root.node.item.removeMemberByName(propertyName)

        var graph = node.graph
        if (root.inPort) {

            if (root.inPort.inEdge)
                graph.removeEdge(root.inPort.inEdge)

            graph.removePort(node, root.inPort)
        }
        if (root.outPort) {
            for (var i=0; i< root.outPort.outEdges.length; ++i)
                graph.removeEdge(root.outPort.outEdges[i])

            graph.removePort(node, root.outPort)
        }

        var children = root.paletteListContainer.children
        for (var idx = 0; idx < children.length; ++idx)
            children[idx].destroy()

        root.destroy()
    }

    Rectangle {
        id: propertyTitle
        radius: root.style.radius
        color: root.style.background
        width: parent.width - 10
        height: 30

        Input.Label{
            id: propertyLabel
            anchors.verticalCenter : parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            text: root.title
            textStyle: root.style.textStyle
        }

        Input.Label{
            id: typeLabel
            anchors.verticalCenter : parent.verticalCenter
            anchors.left: propertyLabel.right
            anchors.leftMargin: 5
            text: root.subtitle
            textStyle: Input.TextStyle {
                font: root.style.textStyle.font
                color: "gray"
            }
        }

        Loader{
            id: iconLoader
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: propertyLabel.right
            anchors.leftMargin: 5
            sourceComponent: {
                return root.editFragment && root.editFragment.location === QmlEditFragment.Slot
                    ? root.eventIcon
                    : root.isMethod ? root.methodIcon : null
            }
        }


        Item{
            visible:!isAnObject
            id: paletteAddButton
            anchors.right: parent.right
            anchors.rightMargin: 25
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 20
            Image{
                anchors.centerIn: parent
                source: "qrc:/images/palette-add.png"
            }
            MouseArea{
                id: paletteAddMouse
                anchors.fill: parent
                onClicked: {
                    var paletteControls = lk.layers.workspace.extensions.editqml.paletteControls
                    var coords = root.mapToItem(node.item, 0, 0)
                    var paletteList = paletteControls.views.openPaletteListForNode(
                        root,
                        paletteContainer,
                        node.item
                    )

                    if (paletteList){
                        paletteList.anchors.topMargin = coords.y + 30
                        paletteList.anchors.left = node.item.left
                        paletteList.anchors.leftMargin = coords.x
                        paletteList.width = Qt.binding(function(){return root.width})
                    }
                }
            }
        }

        Item{
            visible: !isAnObject
            id: closeObjectItem
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.top: parent.top
            width: 20
            height: 20
            Text{
                text: 'x'
                color: '#ffffff'
                font.pixelSize: 18
                font.family: "Open Sans"
                font.weight: Font.Light
            }
            MouseArea{
                id: paletteCloseArea
                anchors.fill: parent
                onClicked: {
                    editFragment.codeHandler.removeConnection(editFragment)
                    if (editFragment.refCount > 0)
                    {
                        destroyObjectNodeProperty()
                    }
                }
            }
        }
    }

    PaletteGroup {
        id: paletteContainer
        anchors.top: parent.top
        anchors.topMargin: propertyTitle.height
        function updateContentWidth(){
            var max = 355
            for (var i=0; i<children.length; ++i){
                if (children[i].width + 10 > max)
                    max = children[i].width + 10
            }
            root.contentWidth = max + root.anchors.leftMargin
            if (node)
                node.item.resizeNode()
        }
        onChildrenChanged: {
            updateContentWidth()
        }
        editFragment: root.editFragment

    }
    
    Connections {
        target: editFragment
        function onAboutToBeRemoved(){
            destroyObjectNodeProperty()
        }
        ignoreUnknownSignals: true
    }

}
