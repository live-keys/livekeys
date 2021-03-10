import QtQuick 2.3
import QtQuick.Controls 2.1
import editor 1.0
import editor.private 1.0
import editqml 1.0

import workspace 1.0 as Workspace
import workspace.quickqanava 2.0 as Qan

Qan.NodeItem{
    id: root
    
    width: 370
    height: wrapper.height
    
    property QtObject theme: lk.layers.workspace.themes.current

    objectName: "objectNode"
    property string label: ''
    property var properties: []
    property var propertiesOpened: []
    property alias propertyContainer: propertyContainer
    property alias paletteContainer: paletteContainer

    property var nodeParent: null
    property var editingFragment: null
    property var removeNode: null
    property var addSubobject: null
    property string id: ""
    property var documentHandler: null
    property var editor: null
    property var objectGraph: null

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls

    function resizeNode(){
        var max = 350
        if (paletteContainer.width +10 > max){
            max = paletteContainer.width + 10
        }

        for (var i = 0; i < propertyContainer.children.length; ++i){
            if (propertyContainer.children[i].contentWidth > max){
                max = propertyContainer.children[i].contentWidth
            }
        }


        if (max !== root.width){
            root.width = max
            for (var i = 0; i < propertyContainer.children.length; ++i){
                propertyContainer.children[i].width = max - propertyContainer.children[i].anchors.leftMargin
            }
        }
    }

    resizable: false

    function expandDefaultPalette(){
        paletteControls.openDefaultPalette(editingFragment, editor, paletteContainer, root)
    }

    function expandOptions(options){
        var codeHandler = root.documentHandler.codeHandler

        if ( 'palettes' in options){
            var palettes = options['palettes']
            for ( var i = 0; i < palettes.length; ++i){
                paletteControls.openPaletteByName(palettes[i], root.editingFragment, editor, paletteContainer)
            }
        }

        if ( 'properties' in options){
            var newProps = options['properties']
            for ( var i = 0; i < newProps.length; ++i ){

                var propName = newProps[i][0]
                var propType = codeHandler.propertyType(root.editingFragment, propName)

                var propPalette = newProps[i].length > 1 ? newProps[i][1] : ''

                if ( propType === '' ) continue

                var ef = null
                if (newProps[i].length > 2)
                {
                    ef = codeHandler.createReadOnlyPropertyFragment(root.editingFragment, propName)
                } else {
                    var ppos = codeHandler.addProperty(
                        root.editingFragment.valuePosition() + root.editingFragment.valueLength() - 1,
                        root.editingFragment.typeName(),
                        propType,
                        propName,
                        true
                    )
                    ef = codeHandler.openNestedConnection(
                        root.editingFragment, ppos
                    )
                }



                if (ef) {
                    root.editingFragment.signalPropertyAdded(ef, false)
                    if (propPalette.length === 0) continue
                    for (var j = 0; j < propertyContainer.children.length; ++j){
                        if (propertyContainer.children[j].propertyName !== propName) continue

                        paletteControls.openPaletteByName(propPalette, ef, editor, propertyContainer.children[j].paletteContainer)
                        break
                    }
                } else {
                    lk.layers.workspace.messages.pushError("ObjectNode: Can't open declared palette for property " + propName, 1)
                }

            }
        }
    }

    function removePropertyName(name){
        var idx = propertiesOpened.find(function(str){ return str === name })
        if (idx !== -1){
            propertiesOpened.splice(idx, 1)
        }
    }

    function addPropertyToNodeByName(name){
        for (var i = 0; i < propertiesOpened.length; ++i){
            if (propertiesOpened[i] === name){
                return
            }
        }

        var codeHandler = documentHandler.codeHandler

        var position = editingFragment.valuePosition() +
                       editingFragment.valueLength() - 1

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

    property QtObject defaultStyle : QtObject{
        property color background: "#112"
        property double radius: 15
        property color borderColor: "#555"
        property double borderWidth: 1

        property color titleBackground: "#666"
        property double titleRadius: 5
        property QtObject titleTextStyle : Workspace.TextStyle{}
    }

    property QtObject nodeStyle : defaultStyle

    Rectangle{
        id: wrapper
        width: parent.width
        height: nodeTitle.height + paletteContainer.height + propertyContainer.height + 40
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
            
            Workspace.Label{
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
                        paletteControls.eraseObject(root)
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
                        paletteControls.compose(root, true, objectGraph)
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
                        var paletteList = paletteControls.addPaletteList(
                            root,
                            paletteContainer,
                            {"x": 0, "y": 0},
                            PaletteControls.PaletteListMode.NodeEditor,
                            PaletteControls.PaletteListSwap.NoSwap,
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
        
        Column{
            id: propertyContainer
            spacing: 10
            anchors.top: parent.top
            anchors.topMargin: 50 + paletteContainer.height
            anchors.left: parent.left
            anchors.leftMargin: 5
        }

        PaletteGroup {
            anchors.top: parent.top
            anchors.topMargin: nodeTitle.height
            id: paletteContainer
            onChildrenChanged: resizeNode()
            editingFragment: root.editingFragment
        }
    }

    Connections {
        target: editingFragment
        ignoreUnknownSignals: true
        onAboutToBeRemoved: {
            if (removeNode)
                removeNode(nodeParent)
        }
        onObjectAdded: {
            if (!addSubobject) return

            documentHandler.codeHandler.populateObjectInfoForFragment(obj)

            var object = obj.objectInfo()
            addSubobject(nodeParent, object.name + (object.id ? ("#" + object.id) : ""), ObjectGraph.PortMode.None, object.connection)
        }
        onPropertyAdded: {
            documentHandler.codeHandler.populatePropertyInfoForFragment(ef)

            var prop = ef.objectInfo()
            var name = ef.identifier()
            for (var i=0; i < propertiesOpened.length; ++i){
                if (!propertiesOpened[i].toString().localeCompare(name)) return
            }

            var portState = ObjectGraph.PortMode.OutPort

            if (prop.isWritable) portState = portState | ObjectGraph.PortMode.InPort
            addSubobject(nodeParent, name, portState, prop.connection)
        }

    }
}
