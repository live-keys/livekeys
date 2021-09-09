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

    property QtObject childObjectContainer: null

    signal updateContentWidth()
    onUpdateContentWidth: paletteContainer.updateContentWidth()

    function __initialize(node, style, editFragment){
        root.style = style
        root.propertyName = editFragment.identifier()
        root.title = root.propertyName
        root.subtitle = editFragment.typeName()
        root.node = node
        root.editFragment = editFragment

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

    function __initializeFunction(node, style, name){
        root.style = style
        root.propertyName = name
        root.title = root.propertyName
        root.node = node
        root.width = node.item.width
        root.isMethod = true
    }

    function addInPort(){
        if ( root.inPort )
            return

        var port = root.node.item.objectGraph.insertPort(root.node, Qan.NodeItem.Left, Qan.Port.In);
        port.label = root.propertyName + " In"
        port.y = Qt.binding(
            function(){
                if (!root.node || !root.node.item)
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

    function clean(){
        root.node.item.removeMemberByName(propertyName)
        aboutToRemoveConnection.enabled = false

        var graph = root.node.graph
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

        return root
    }

    function paletteByName(name){
        var pg = paletteContainer
        for ( var i = 0; i < pg.children.length; ++i ){
            if ( pg.children[i].name === name )
                return pg.children[i]
        }
        return null
    }

    function userAddPalette(){
        var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions

        var palettes = root.editFragment.language.findPalettesForFragment(root.editFragment)
        palettes.data = paletteFunctions.filterOutPalettes(palettes.data, paletteGroup().palettesOpened, true)
        if (!palettes.data || palettes.data.length === 0)
            return null

        var paletteList = paletteFunctions.views.openPaletteList(paletteFunctions.theme.selectableListView, palettes.data, root.node.item,
        {
             onCancelled: function(){
                 root.node.item.objectGraph.activateFocus()
                 paletteList.destroy()
             },
             onSelected: function(index){
                 var palette = root.editFragment.language.expand(root.editFragment, {
                     "palettes" : [palettes.data[index].name]
                 })
                var paletteBox = paletteFunctions.__factories.createPaletteContainer(palette, root.paletteGroup(), {moveEnabled: false})
                root.node.item.objectGraph.activateFocus()
             }
        })

        paletteList.forceActiveFocus()
        paletteList.width = root.width
        paletteList.anchors.topMargin = root.y + 78
    }

    function paletteGroup(){
        return paletteContainer
    }

    function expand(){}

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
            id: paletteAddButton
            visible:!isAnObject
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
                onClicked: root.userAddPalette()
            }
        }

        Item{
            id: closeObjectItem
            visible: !isAnObject
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
                    editFragment.language.removeConnection(editFragment)
                    if (editFragment.refCount > 0)
                    {
                        root.clean().destroy()
                    }
                }
            }
        }
    }

    PaletteGroup{
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
        id: aboutToRemoveConnection
        target: editFragment
        function onAboutToBeRemoved(){
            root.clean().destroy()
        }
        ignoreUnknownSignals: true
    }

}
