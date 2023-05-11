import QtQuick 2.7
import editqml 1.0

Column{
    id: paletteGroup
    spacing: 3
    leftPadding: 3
    topPadding: 0
    objectName: "paletteGroup"
    width: 30
    height: 30

    function __initialize(ef){
        paletteGroup.editFragment = ef
        if ( !ef.visualParent )
            ef.visualParent = paletteGroup
    }

    property var sizeInfo: ({minWidth: -1, minHeight: -1, maxWidth: -1, maxHeight: -1, contentWidth: -1})
    property QtObject editFragment : null
    property var owner: null
    property var palettesOpened: []
    property bool fillEditorWidth: false

    property Connections editorConnections: Connections{
        target: null
        function onWidthChanged(){ paletteGroup.measureSizeInfo() }
        function onLineSurfaceWidthChanged(){ paletteGroup.measureSizeInfo() }
    }

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
                    editFragment.language.removeConnection(editFragment)
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
                paletteGroup.editFragment.language.importsFragment = null
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
            editFragment.language.removePalette(paletteGroup.children[i].palette)
    }

    function adjustSizeToEditor(){
        fillEditorWidth = true
        var editor = editFragment.language.code.textEdit().getEditor()
        editorConnections.target = editor
        paletteGroup.measureSizeInfo()
    }

    function adjustSize(){
        if ( paletteGroup.parent && paletteGroup.parent.sizeInfo ){
            if ( paletteGroup.parent.sizeInfo.contentWidth ){
                paletteGroup.width = paletteGroup.parent.sizeInfo.contentWidth
                paletteGroup.sizeInfo.contentWidth = paletteGroup.width - paletteGroup.leftPadding * 2
            }
        } else if ( fillEditorWidth && editFragment ){
            var editor = editFragment.language.code.textEdit().getEditor()
            var editorWidth = editor.width - editor.lineSurfaceWidth - 20
            var width = editorWidth > paletteGroup.sizeInfo.minWidth ? editorWidth : paletteGroup.sizeInfo.minWidth
            paletteGroup.width = width
            paletteGroup.sizeInfo.contentWidth = paletteGroup.width - paletteGroup.leftPadding * 2
        } else {
            paletteGroup.width = paletteGroup.sizeInfo.minWidth
            paletteGroup.height = paletteGroup.sizeInfo.minHeight
            paletteGroup.sizeInfo.contentWidth = paletteGroup.width - paletteGroup.leftPadding * 2
        }

        for ( var i = 0; i < paletteGroup.children.length; ++i ){
            var c = paletteGroup.children[i]
            c.adjustSize()
        }

        var totalHeight = 0
        for ( var i = 0; i < paletteGroup.children.length; ++i ){
            var c = paletteGroup.children[i]
            if ( i > 0 ){
                totalHeight += paletteGroup.spacing
            }
            totalHeight += c.height
        }

        paletteGroup.height = totalHeight
    }

    function measureSizeInfo(){
        var size = { minWidth: -1, minHeight: -1, maxWidth: -1, maxHeight: -1 }
        for ( var i = 0; i < this.children.length; ++i ){
            var si = this.children[i].sizeInfo
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

        sizeInfo = size
        if ( parent && parent.measureSizeInfo ){
            parent.measureSizeInfo()
        } else {
            adjustSize()
        }
    }

    onChildrenChanged: { measureSizeInfo() }

//    Rectangle{
//        width: 400
//        height: 250
//        color: 'pink'

//        MouseArea{
//            anchors.fill: parent
//            onPositionChanged: {
//                console.log("PINK NEW X:", mouse.x, mouse.y)
//            }
//        }
//    }
}
