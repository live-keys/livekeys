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
    anchors.leftMargin: root.control.isAnObject ? 30 : 0

    property Item inPort : null
    property Item outPort : null

    property ObjectNodeMemberControl control: ObjectNodeMemberControl{}
    property QtObject style: ObjectNodeMemberStyle{}
    property var sizeInfo: ({minWidth: -1, minHeight: -1, maxWidth: -1, maxHeight: -1})

    width: 30
    height: 30
    z: -1

    function __initialize(node, style, editFragment){
        root.control.__initialize(root, node, editFragment, paletteContainer, propertyTitle)
        root.style = style
        root.width = node.item.width - (root.control.isAnObject ? 30 : 0)
    }

    function __initializeFunction(node, style, name){
        root.control.__initializeFunction(root, node, name, paletteContainer, propertyTitle)
        root.style = style
        root.width = node.item.width
    }

    function clean(){
        root.control.node.item.control.removeMemberByName(root.control.propertyName)
        aboutToRemoveConnection.enabled = false

        var graph = root.control.node.graph
        if (root.inPort) {
            if (root.inPort.inEdge)
                graph.removeEdge(root.inPort.inEdge)
            graph.removePort(root.control.node, root.inPort)
        }
        if (root.outPort) {
            for (var i=0; i< root.outPort.outEdges.length; ++i)
                graph.removeEdge(root.outPort.outEdges[i])
            graph.removePort(root.control.node, root.outPort)
        }

        var children = root.control.paletteGroup.children
        for (var idx = 0; idx < children.length; ++idx)
            children[idx].destroy()

        return root
    }

    function measureSizeInfo(){
        var si = paletteContainer.sizeInfo

        root.sizeInfo = {
            minWidth: si.minWidth < 280 ? 280 : si.minWidth,
            minHeight: si.minHeight > 30 ? si.minHeight : 30,
            maxWidth: si.maxWidth > 280 ? si.maxWidth : -1,
            maxHeight: si.maxHeight > 30 ? si.maxHeight : -1
        }

        if ( root.parent && root.parent.measureSizeInfo ){
            root.parent.measureSizeInfo()
        }
    }

    function adjustSize(){
        var width = root.control.isAnObject
                ? root.parent.sizeInfo.objectContentWidth
                : root.parent.sizeInfo.contentWidth
        this.width = width
        this.sizeInfo.contentWidth = width
        paletteContainer.adjustSize()
        this.height = 30 + paletteContainer.height
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
            text: root.control.title
            textStyle: root.style.textStyle
        }

        Input.Label{
            id: typeLabel
            anchors.verticalCenter : parent.verticalCenter
            anchors.left: propertyLabel.right
            anchors.leftMargin: 5
            text: root.control.subtitle
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
            visible:!root.control.isAnObject
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
                onClicked: root.control.userAddPalette()
            }
        }

        Item{
            id: closeObjectItem
            visible: !root.control.isAnObject
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
                id: propertyCloseArea
                anchors.fill: parent
                onClicked: {
                    root.control.editFragment.language.removeConnection(root.control.editFragment)
                    if (root.control.editFragment.refCount > 0){
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

        editFragment: root.control.editFragment
    }
    
    Connections {
        id: aboutToRemoveConnection
        target: root.control.editFragment
        function onAboutToBeRemoved(){
            root.clean().destroy()
        }
        ignoreUnknownSignals: true
    }

}
