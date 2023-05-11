import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import editqml 1.0

import workspace.quickqanava 2.0 as Qan

QtObject{
    id: root
    property string propertyName : ''
    property string propertyType: ''

    property string title: ''
    property string subtitle: ''

    property QtObject node : null
    property Item propertyTitle: null
    property Item paletteGroup: null
    property Item objectNodeMember: null
    property var editFragment: null

    property var isAnObject: false
    property bool isMethod: false

    property QtObject childObjectContainer: null

    function __initialize(objectNodeMember, node, editFragment, paletteGroup, propertyTitle){
        root.propertyName = editFragment.identifier()
        root.title = root.propertyName
        root.subtitle = editFragment.typeName()
        root.node = node
        root.objectNodeMember = objectNodeMember
        root.editFragment = editFragment
        root.paletteGroup = paletteGroup
        root.propertyTitle = propertyTitle

        if ( editFragment ) {
            if ( editFragment.location === QmlEditFragment.Object ){
                root.isAnObject = true
                root.title = editFragment.typeName()
                root.subtitle = editFragment.objectId()
            }
            editFragment.incrementRefCount()
        }
    }

    function __initializeFunction(objectNodeMember, node, name, paletteGroup, propertyTitle){
        root.propertyName = name
        root.title = root.propertyName
        root.node = node
        root.objectNodeMember = objectNodeMember
        root.isMethod = true
        root.paletteGroup = paletteGroup
        root.propertyTitle = propertyTitle
    }

    function addInPort(){
        if ( root.objectNodeMember.inPort )
            return

        var port = root.node.item.control.objectGraph.insertPort(root.node, Qan.NodeItem.Left, Qan.Port.In);
        port.label = root.propertyName + " In"
        port.y = Qt.binding(
            function(){
                if (!root.node || !root.node.item)
                    return 0
                return root.node.item.control.paletteGroup.height +
                       root.objectNodeMember.y +
                       (root.propertyTitle.height / 2) +
                       42
            }
        )

        root.objectNodeMember.inPort = port
        port.objectProperty = root.objectNodeMember
    }

    function addOutPort(){
        if (root.outPort )
            return

        var port = root.node.item.control.objectGraph.insertPort(root.node, Qan.NodeItem.Right, Qan.Port.Out);
        port.label = root.propertyName + " Out"
        port.y = Qt.binding(
            function(){
                if (!root.node || !root.node.item)
                    return 0
                return root.node.item.control.paletteGroup.height +
                       root.objectNodeMember.y +
                       (root.propertyTitle.height / 2) +
                       42
            }
        )
        root.objectNodeMember.outPort = port
        port.objectProperty = root.objectNodeMember
    }

    function userAddPalette(){
        var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions

        var palettes = root.editFragment.language.findPalettes(root.editFragment)
        palettes = paletteFunctions.filterOutPalettes(palettes, paletteGroup.palettesOpened, true)
        if (!palettes || palettes.length === 0)
            return null

        var paletteList = paletteFunctions.views.openPaletteList(paletteFunctions.theme.selectableListView, palettes, root.node.item,
        {
             onCancelled: function(){
                 root.node.item.control.objectGraph.activateFocus()
                 paletteList.destroy()
             },
             onSelected: function(index){
                 var palette = root.editFragment.language.expand(root.editFragment, {
                     "palettes" : [palettes[index].name]
                 })
                var paletteBox = paletteFunctions.__factories.createPaletteContainer(palette, root.paletteGroup, {moveEnabled: false})
                root.node.item.control.objectGraph.activateFocus()
             }
        })

        paletteList.forceActiveFocus()
        paletteList.width = root.objectNodeMember.width
        paletteList.anchors.topMargin = root.objectNodeMember.y + 78
    }

    function paletteByName(name){
        var pg = root.paletteGroup
        for ( var i = 0; i < pg.children.length; ++i ){
            if ( pg.children[i].name === name )
                return pg.children[i]
        }
        return null
    }

    function activePaletteGroup(){
        return paletteGroup
    }

    function expand(){}

}
