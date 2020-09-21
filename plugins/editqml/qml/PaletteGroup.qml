import QtQuick 2.7

Column{
    id: paletteGroup
    spacing: 3
    leftPadding: 0
    topPadding: 0
    objectName: "paletteGroup"

    property QtObject editingFragment : null
    property QtObject codeHandler : null
    property var owner: null
    property Connections editingFragmentRemovals: Connections{
        target: editingFragment
        onAboutToRemovePalette : {
            var p = palette.item.parent
            while ( p && p.objectName !== "paletteContainer" )
                p = p.parent

            if ( p && p.objectName === "paletteContainer" )
                p.destroy()
        }
        onPaletteListEmpty: {
            var box = paletteGroup.parent
            if ( editingFragment.bindingPalette() ){
                if ( box.objectName === 'editorBox' ){
                    box.destroy()
                }
            } else {
                if ( box && box.objectName === 'editorBox' ){
                    codeHandler.removeConnection(editingFragment)
                }
            }
        }
        onAboutToBeRemoved : {
            var p = paletteGroup.parent
            if ( p && p.objectName === 'editorBox' ){
                p.destroy()
            }
        }
    }

    onWidthChanged: {

        if (parent && parent.parent && parent.parent.parent &&
            parent.parent.parent.objectName === "objectContainer")
            parent.parent.parent.recalculateContentWidth()
        if (parent && parent.parent && parent.parent.objectName === "objectNode"){
            parent.parent.resizeNode()
        }
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
