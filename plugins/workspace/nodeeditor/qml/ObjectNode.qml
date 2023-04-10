import QtQuick 2.3
import QtQuick.Controls 2.1
import editor 1.0
import editor.private 1.0
import editqml 1.0 as EditQml

import workspace.quickqanava 2.0 as Qan
import visual.input 1.0 as Input

Qan.NodeItem{
    id: root
    width: 370
    height: wrapper.height

    objectName: "objectNode"
    resizable: false

    property QtObject nodeStyle : ObjectNodeStyle{}
    property ObjectNodeControl control: ObjectNodeControl{}
    property var sizeInfo: ({minWidth: -1, minHeight: -1, maxWidth: -1, maxHeight: -1})
    property var outPort: null

    function __initialize(ef){
        var editor = ef.language.code.textEdit().getEditor()
        root.control.__initialize(editor, ef, root, nodeMemberContainer, paletteContainer)
    }

    function adjustSize(){
        var w = root.sizeInfo.minWidth > 300 ? root.sizeInfo.minWidth : 300
        root.width = w
        root.sizeInfo.contentWidth = w
        root.sizeInfo.objectContentWidth = w - 30

        root.control.paletteGroup.adjustSize()
        for ( var i = 0; i < nodeMemberContainer.children.length; ++i ){
            var c = nodeMemberContainer.children[i]
            if ( c.adjustSize )
                c.adjustSize()
        }

        var height = 70
        height += root.control.paletteGroup.height
        if ( nodeMemberContainer.children.length > 0 ){
            for ( var i = 0; i < nodeMemberContainer.children.length; ++i ){
                height += nodeMemberContainer.children[i].height
            }
        }
        if ( nodeMemberContainer.children.length > 1 )
            height += (nodeMemberContainer.children.length - 1) * nodeMemberContainer.spacing

        root.height = height
    }

    function measureSizeInfo(){
        var si = paletteContainer.sizeInfo
        var size = {
            minWidth: si.minWidth < 300 ? 300 : si.minWidth,
            minHeight: si.minHeight > 0 ? si.minHeight + 50 : 50,
            maxWidth: si.maxWidth > 300 ? si.maxWidth : -1,
            maxHeight: si.maxHeight > 70 ? si.maxHeight : -1,
            contentWidth: -1
        }

        for ( var i = 0; i < nodeMemberContainer.children.length; ++i ){
            var si = nodeMemberContainer.children[i].sizeInfo

            if ( si.minWidth > 0 ){
                if ( size.minWidth < 0 )
                    size.minWidth = si.minWidth
                else if ( size.minWidth < si.minWidth ){
                    size.minWidth = si.minWidth
                }
            }

            if ( si.maxWidth > 0 ){
                if ( size.maxWidth < 0 )
                    size.maxWidth = si.maxWidth
                else if ( size.maxWidth < si.maxWidth ){
                    size.maxWidth = si.maxWidth
                }
            }
            if ( si.minHeight > 0 ){
                size.minHeight = size.minHeight < 0 ? si.minHeight : size.minHeight + si.minHeight
            }
            if ( si.maxHeight > 0 ){
                size.maxHeight = size.maxHeight < 0 ? si.maxHeight : size.maxHeight + si.maxHeight
            }
        }
        if ( size.minWidth < 300 ){
            size.minWidth = 300
        }
        if ( size.minHeight < 30 ){
            size.minHeight = 30
        }

        root.sizeInfo = size

        root.adjustSize()
    }


    function clean(){
        for (var xi = 0; xi < paletteContainer.children.length; ++xi){
            if (paletteContainer.children[xi].objectName === "paletteContainer")
                paletteContainer.children[xi].destroy()
        }

        for ( var i = 0; i < root.control.members.length; ++i ){
            root.control.members[i].clean().destroy()
        }
        root.control.members = []

        return root
    }


    onSelectedChanged: {
        objectGraph.numOfSelectedNodes += selected ? 1 : -1
        if (objectGraph.numOfSelectedNodes === 0)
            objectGraph.activateFocus()
    }
    onActiveFocusChanged: {
        if ( root.activeFocus ){
            objectGraph.activeItem = this
        } else if ( objectGraph.activeItem === this ){
            objectGraph.activeItem = null
        }
        objectGraph.checkFocus()
    }

    Rectangle{
        id: wrapper
        width: parent.width
        height: root.height
        color: root.nodeStyle.background
        radius: root.nodeStyle.radius
        border.color: root.nodeStyle.borderColor
        border.width: root.nodeStyle.borderWidth

        property alias sizeInfo: root.sizeInfo
        function measureSizeInfo(){ root.measureSizeInfo() }
        
        Rectangle{
            id: nodeTitle
            width: parent.width
            height: 30
            anchors.top: parent.top

            color: root.nodeStyle.titleBackground
            radius: root.nodeStyle.titleRadius
            
            Input.Label{
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 15
                textStyle: root.nodeStyle.titleTextStyle
                text: root.control.label
            }

            Item{
                id: eraseButton
                anchors.right: parent.right
                anchors.rightMargin: 48
                anchors.verticalCenter: parent.verticalCenter
                width: 15
                height: 20
                Image{
                    anchors.centerIn: parent
                    source: "qrc:/images/palette-erase-object.png"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.control.paletteFunctions.eraseObject(root)
                    }
                }
            }

            Item{
                id: composeButton
                anchors.right: parent.right
                anchors.rightMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                width: 15
                height: 20
                Image{
                    anchors.centerIn: parent
                    source: "qrc:/images/palette-add-property.png"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.control.paletteFunctions.userAddToObjectContainer(root, {
                            onCancelled: function(){ root.control.objectGraph.activateFocus() },
                            onAccepted: function(){ root.control.objectGraph.activateFocus() }
                        })
                    }
                }
            }

            Item{
                id: paletteAddButton
                anchors.right: parent.right
                anchors.rightMargin: 12
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
        }

        EditQml.PaletteGroup{
            id: paletteContainer
            anchors.top: parent.top
            anchors.topMargin: nodeTitle.height
            editFragment: root.control.editFragment
        }
        
        Column{
            id: nodeMemberContainer
            spacing: 10
            anchors.top: parent.top
            anchors.topMargin: 50 + paletteContainer.height
            height: root.height - anchors.topMargin
            width: root.width - 10
            anchors.left: parent.left
            anchors.leftMargin: 5
            property alias sizeInfo: root.sizeInfo
            function measureSizeInfo(){ root.measureSizeInfo() }
        }
    }

    Connections {
        target: control.editFragment
        ignoreUnknownSignals: true
        function onChildAdded(ef, context){
            if ( ef.location === EditQml.QmlEditFragment.Object ){
                root.control.addChildObject(ef)
            } else {
                root.control.addProperty(ef)
            }
        }
        function onAboutToBeRemoved(){
            paletteContainer.closePalettes()
            root.control.objectGraph.removeObjectNode(control.nodeParent)
        }
    }
}
