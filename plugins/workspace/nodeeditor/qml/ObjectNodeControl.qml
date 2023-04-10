import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import editqml 1.0 as EditQml

QtObject{
    id: root
    objectName: "objectNodeControl"

    property var paletteFunctions: lk.layers.workspace.extensions.editqml.paletteFunctions
    property QtObject theme: lk.layers.workspace.themes.current

    property string nodeId: ''
    property var editFragment: null

    property var editor: null
    property string objectId: ''
    property string label: ''
    property var nodeParent: null
    property var objectGraph: null

    property var members: []
    property var propertiesOpened: []

    property Item propertyContainer: null
    property Item paletteGroup: null
    property Item objectNode: null

    property bool supportsFunctions: true


    function __initialize(editor, ef, objectNode, propertyContainer, paletteGroup){
        root.editor = editor
        root.editFragment = ef
        root.objectId = ef.objectId()
        root.nodeId = root.objectId
        root.label = ef.typeName() + (root.objectId ? ("#" + root.objectId) : "")
        root.objectNode = objectNode
        root.propertyContainer = propertyContainer
        root.paletteGroup = paletteGroup
    }

    function paletteByName(name){
        var pg = paletteGroup()
        for ( var i = 0; i < pg.children.length; ++i ){
            if ( pg.children[i].name === name )
                return pg.children[i]
        }
        return null
    }

    function activePaletteGroup(){
        return root.paletteGroup()
    }

    function getPane(){
        var pane = root.objectNode.parent
        while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
            pane = pane.parent
        }
        return pane
    }

    function mapToPane(pane){
        return root.objectNode.mapToItem(pane, 0, 0)
    }

    function propertyByName(name){
        for (var i = 0; i < root.propertyContainer.children.length; ++i){
            var child = root.propertyContainer.children[i]
            if ( !child.control.isAnObject && child.control.propertyName === name )
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
            nodeMember.control.addOutPort()
        } else {
            if ( nodeParent.item.control.nodeId !== "" ){
                nodeMember.control.addOutPort()
            }
            if ( ef.isWritable )
                nodeMember.control.addInPort()
        }

        root.members.push(nodeMember)
        nodeMember.measureSizeInfo()

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

        if ( nodeParent.item.control.nodeId !== '' )
            nodeMember.addInPort()

        root.members.push(nodeMember)
        nodeMember.measureSizeInfo()

        return nodeMember
    }

    function addChildObject(ef){
        var graphFn = lk.layers.workspace.extensions.workspace.objectGraphControls

        var nodeMember = graphFn.objectNodeProperty.createObject(root.propertyContainer)
        var nodeStyle = root.objectGraph.style.objectNodeMemberStyle
        nodeMember.__initialize(nodeParent, nodeStyle, ef)

        root.members.push(nodeMember)
        nodeMember.measureSizeInfo()

        if ( ef.objectId() )
            nodeMember.control.addOutPort()
    }

    function userAddPalette(){
        root.objectNode.selected = false
        var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions

        var palettes = root.editFragment.language.findPalettes(root.editFragment)
        palettes = paletteFunctions.filterOutPalettes(palettes, root.paletteGroup.palettesOpened, true)
        if (!palettes || palettes.length === 0)
            return null

        var paletteList = paletteFunctions.views.openPaletteList(paletteFunctions.theme.selectableListView, palettes, root.objectNode,
        {
            onCancelled: function(){
                root.objectGraph.activateFocus()
                paletteList.destroy()
            },
            onSelected: function(index){
                var palette = root.editFragment.language.expand(root.editFragment, {
                    "palettes" : [palettes[index].name]
                })
                var paletteBox = paletteFunctions.__factories.createPaletteContainer(palette, root.paletteGroup, {moveEnabled: false})
                if ( palette.item ){ // send expand signal
                    root.expand()
                } else {
                    paletteFunctions.expandObjectContainerLayout(root, palette, {expandChildren: false})
                }
                paletteList.destroy()
            }
        })

        paletteList.forceActiveFocus()
        paletteList.anchors.topMargin = 30
        paletteList.width = root.objectNode.width
    }

    function expandDefaultPalette(){
        paletteFunctions.openPaletteInObjectContainer(root.objectNode, paletteFunctions.defaultPalette)
    }

    function removeMemberByName(name){
        propertiesOpened = propertiesOpened.filter(function(value, index, arr){
            return value !== name
        })
    }

    function __sortContainerChildren(){}
    function expand(){}


}
