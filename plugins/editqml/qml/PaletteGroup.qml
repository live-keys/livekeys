import QtQuick 2.7
import editqml 1.0

Column{
    id: paletteGroup
    spacing: 3
    leftPadding: 3
    topPadding: 0
    objectName: "paletteGroup"

    function __initialize(ef){
        paletteGroup.editFragment = ef
        paletteGroup.codeHandler = ef.codeHandler
        ef.visualParent = paletteGroup
    }

    property QtObject editFragment : null
    property QtObject codeHandler : null
    property var owner: null
    property var palettesOpened: []
    property Connections editFragmentRemovals: Connections{
        target: editFragment
        function onAboutToRemovePalette(palette){
            var p = palette.item.parent
            while ( p && p.objectName !== "paletteContainer" )
                p = p.parent

            if ( p && p.objectName === "paletteContainer" )
                p.destroy()
        }
        function onPaletteListEmpty(){
            var box = paletteGroup.parent
            if ( editFragment.bindingPalette() ){
                if ( box.objectName === 'editorBox' ){
                    paletteGroup.destroy()
                    box.destroy()
                }
            } else {
                if ( box && box.objectName === 'editorBox' ){
                    editFragment.codeHandler.removeConnection(editFragment)
                    paletteGroup.destroy()
                }
            }
        }
        function onAboutToBeRemoved(){
            var p = paletteGroup.parent

            var ed = p
            while (ed && ed.objectName !== "editorType"){
                ed = ed.parent
            }

            if (ed && paletteGroup.editFragment && paletteGroup.editFragment.location === QmlEditFragment.Imports){
                paletteGroup.editFragment.codeHandler.importsShaped = false
            }

            if ( p && p.objectName === 'editorBox' ){
                for (var i = 0; i < children.length; ++i)
                    if (children[i].objectName === "paletteContainer")
                        children[i].destroy()
                paletteGroup.destroy()
                p.destroy()
            }
        }
    }

    function closePalettes(){
        for (var i = 0; i < paletteGroup.children.length; ++i)
            editFragment.codeHandler.removePalette(paletteGroup.children[i].palette)
    }

    onWidthChanged: {
        // objectNodeProperty palette
        var p = parent
        while (p && p.objectName !== "objectNodeMember")
            p = p.parent

        if (p){
            var objectNodeProperty = p
            while (p && p.objectName !== "objectNode")
                p = p.parent
            var objectNode = p

            objectNodeProperty.updateContentWidth()
            objectNode.resizeNode()
            return
        }

        // objectNode palette
        p = parent
        while (p && p.objectName !== "objectNode")
            p = p.parent

        if (p){
            var objectNode = p
            objectNode.resizeNode()
            return
        }

        // objectContainer palette
        p = parent
        while (p && p.objectName !== "objectContainer")
            p = p.parent

        if (!p)
            return

        var objectContainer = p

        //TODO: Emit signal
        objectContainer.recalculateContentWidth()
    }

    width: {
        var maxWidth = 0;
        if ( children.length > 0 ){
            for ( var i = 0; i < children.length; ++i ){
                if ( children[i].width > maxWidth )
                    maxWidth = children[i].width
            }
        }
        return maxWidth + (maxWidth > 0 ? paletteGroup.leftPadding : 0)
    }
    height: {
        var totalHeight = 0;
        if ( children.length > 0 ){
            for ( var i = 0; i < children.length; ++i ){
                totalHeight += children[i].height
            }
        }
        var result = totalHeight + (children.length - 1) * spacing
        return result + (result > 0 ? paletteGroup.topPadding : 0)
    }
}
