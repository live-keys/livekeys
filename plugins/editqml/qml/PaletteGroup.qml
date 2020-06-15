import QtQuick 2.0

Column{
    id: paletteGroup
    spacing: 10
    objectName: "paletteGroup"

    property QtObject editingFragment : null
    property QtObject codeHandler : null
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

    width: {
        var maxWidth = 0;
        if ( children.length > 0 ){
            for ( var i = 0; i < children.length; ++i ){
                if ( children[i].width > maxWidth )
                    maxWidth = children[i].width
            }
        }
        return maxWidth
    }
    height: {
        var totalHeight = 0;
        if ( children.length > 0 ){
            for ( var i = 0; i < children.length; ++i ){
                totalHeight += children[i].height
            }
        }
        if ( children.length > 1 )
            return totalHeight + (children.length - 1) * spacing
        else
            return totalHeight
    }
}
