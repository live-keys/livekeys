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
    
    property QtObject theme: lk.layers.workspace.themes.current

    objectName: "objectNode"
    property string label: ''

    property var members: []

    property var propertiesOpened: []
    property var outPort: null
    property alias propertyContainer: nodeMemberContainer
    property alias paletteListContainer: paletteContainer

    property var nodeParent: null
    property var editFragment: null
    property var removeNode: null
    property string id: ""
    property var editor: null
    property var objectGraph: null

    property var paletteFunctions: lk.layers.workspace.extensions.editqml.paletteFunctions

    function resizeNode(){
        var max = 350
        if (paletteContainer.width +10 > max){
            max = paletteContainer.width + 10
        }

        for (var i = 0; i < nodeMemberContainer.children.length; ++i){
            if (nodeMemberContainer.children[i].contentWidth > max){
                max = nodeMemberContainer.children[i].contentWidth
            }
        }


        if (max !== root.width){
            root.width = max
            for (var i = 0; i < nodeMemberContainer.children.length; ++i){
                nodeMemberContainer.children[i].width = max - nodeMemberContainer.children[i].anchors.leftMargin
            }
        }
    }

    function paletteByName(name){
        var pg = paletteGroup()
        for ( var i = 0; i < pg.children.length; ++i ){
            if ( pg.children[i].name === name )
                return pg.children[i]
        }
        return null
    }

    function paletteGroup(){
        return paletteContainer
    }

    function getPane(){
        var pane = root.parent
        while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
            pane = pane.parent
        }
        return pane
    }

    function propertyByName(name){
        for (var i = 0; i < nodeMemberContainer.children.length; ++i){
            var child = nodeMemberContainer.children[i]
            if ( !child.isAnObject && child.propertyName === name )
                return child
        }
        return null
    }

    function addProperty(ef){
        var name = ef.identifier()

        var propMember = propertyByName(name)
        if ( propMember )
            return propMember

        propertiesOpened.push(name)

        var graphFn = lk.layers.workspace.extensions.workspace.objectGraphControls

        //TODO: Rename  objectNodeProperty and propertyContainer
        var nodeMember = graphFn.objectNodeProperty.createObject(root.propertyContainer)
        var nodeStyle = root.objectGraph.style.objectNodeMemberStyle
        nodeMember.__initialize(nodeParent, nodeStyle, ef)

        if ( ef.location === EditQml.QmlEditFragment.Slot ){
            nodeMember.addOutPort()
        } else {
            if ( nodeParent.item.id !== "" ){
                nodeMember.addOutPort()
            }
            if ( ef.isWritable )
                nodeMember.addInPort()
        }

        root.members.push(nodeMember)

        return nodeMember
    }

    function addFunctionProperty(name){
        var propMember = propertyByName(name)
        if ( propMember )
            return propMember

        //TODO: Rename  objectNodeProperty and propertyContainer
        var graphFn = lk.layers.workspace.extensions.workspace.objectGraphControls
        var nodeMember = graphFn.objectNodeProperty.createObject(root.propertyContainer)
        var nodeStyle = root.objectGraph.style.objectNodeMemberStyle
        nodeMember.__initializeFunction(nodeParent, nodeStyle, name)

        if ( nodeParent.item.id !== '' )
            nodeMember.addInPort()

        root.members.push(nodeMember)

        return nodeMember
    }

    function addChildObject(ef){
        var graphFn = lk.layers.workspace.extensions.workspace.objectGraphControls

        var nodeMember = graphFn.objectNodeProperty.createObject(root.propertyContainer)
        var nodeStyle = root.objectGraph.style.objectNodeMemberStyle
        nodeMember.__initialize(nodeParent, nodeStyle, ef)

        if ( ef.objectId() )
            nodeMember.addOutPort()
    }

    function sortChildren(){}
    function expand(){}

    property bool supportsFunctions: true
    resizable: false

    function expandDefaultPalette(){
        paletteFunctions.openPaletteInObjectContainer(root, paletteFunctions.defaultPalette)
    }

    function removeMemberByName(name){
        propertiesOpened = propertiesOpened.filter(function(value, index, arr){
            return value !== name
        })
    }

    function addPropertyToNodeByName(name){
        for (var i = 0; i < propertiesOpened.length; ++i){
            if (propertiesOpened[i] === name){
                return
            }
        }

        var codeHandler = editFragment.language

        var position = editFragment.valuePosition() +
                       editFragment.valueLength() - 1

        var addContainer = codeHandler.getAddOptions(position)
        if ( !addContainer )
            return

        addContainer.activeIndex = 1
        addContainer.model.setFilter(name)
        if (addContainer.model.rowCount() !== 1) return

        var type = addContainer.model.data(addContainer.model.index(0, 0), 256 + 3/*QmlSuggestionModel.Type*/)

        for (var i=0; i < propertiesOpened.length; ++i){
            if (!propertiesOpened[i].toString().localeCompare(name)) return
        }

        addSubobject(nodeParent, name, ObjectGraph.PortMode.OutPort, null)
    }

    onSelectedChanged: {
        objectGraph.numOfSelectedNodes += selected ? 1 : -1
        if (objectGraph.numOfSelectedNodes === 0)
            objectGraph.activateFocus()
    }
    onActiveFocusChanged: {
        if ( activeFocus ){
            objectGraph.activeItem = this
        } else if ( objectGraph.activeItem === this ){
            objectGraph.activeItem = null
        }
        objectGraph.checkFocus()
    }

    property QtObject nodeStyle : ObjectNodeStyle{}

    Rectangle{
        id: wrapper
        width: parent.width
        height: nodeTitle.height + paletteContainer.height + nodeMemberContainer.height + 40
        color: root.nodeStyle.background
        radius: root.nodeStyle.radius
        border.color: root.nodeStyle.borderColor
        border.width: root.nodeStyle.borderWidth
        
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
                text: root.label
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
                        paletteFunctions.eraseObject(root)
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
                        paletteFunctions.userAddToObjectContainer(root, {
                            onCancelled: function(){ root.objectGraph.activateFocus() },
                            onAccepted: function(){ root.objectGraph.activateFocus() }
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
                    onClicked: {
                        root.selected = false
                        var paletteList = paletteFunctions.views.openPaletteListForNode(
                            root,
                            paletteContainer,
                            wrapper
                        )

                        if (paletteList){
                            paletteList.anchors.topMargin = nodeTitle.height
                            paletteList.width = Qt.binding(function(){ return wrapper.width })
                        }
                    }
                }
            }
        }

        EditQml.PaletteGroup{
            id: paletteContainer
            anchors.top: parent.top
            anchors.topMargin: nodeTitle.height
            onChildrenChanged: resizeNode()
            editFragment: root.editFragment
        }
        
        Column{
            id: nodeMemberContainer
            spacing: 10
            anchors.top: parent.top
            anchors.topMargin: 50 + paletteContainer.height
            anchors.left: parent.left
            anchors.leftMargin: 5
        }
    }

    Connections {
        target: editFragment
        ignoreUnknownSignals: true
        function onChildAdded(ef, context){
            if ( ef.location === EditQml.QmlEditFragment.Object ){
                root.addChildObject(ef)
            } else {
                root.addProperty(ef)
            }
        }
        function onAboutToBeRemoved(){
            paletteContainer.closePalettes()
            if (removeNode)
                removeNode(nodeParent)
        }
    }
}
