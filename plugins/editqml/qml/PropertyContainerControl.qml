import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import editqml 1.0 as EditQml
import workspace.icons 1.0 as Icons

QtObject{
    id: root

    property string title: "Object"
    objectName: "propertyContainerControl"

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject paletteListContainer: null
    property QtObject editFragment : null

    property QtObject code : null
    property Item editor: null
    property Item valueContainer : null

    property bool isAnObject: false
    property var childObjectContainer: null

    property var paletteFunctions: lk.layers.workspace.extensions.editqml.paletteFunctions

    property Component methodIcon: Icons.MenuIcon{}
    property Component eventIcon: Icons.EventIcon{ color: theme.colorScheme.middlegroundOverlayDominantBorder; width: 15; height: 15 }


    function __initialize(editor, ef, paletteListContainer, ){
        root.title = ef.identifier()
        root.editor = editor
        root.code = editor.code
        root.editFragment = ef
        root.paletteListContainer = paletteListContainer

        var codeHandler = ef.language
        var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions

        if ( ef.valueLocation === EditQml.QmlEditFragment.Object ){
            root.isAnObject = true

            var childObjectContainer = paletteFunctions.__factories.createObjectContainer(editor, ef, null)

            root.childObjectContainer = childObjectContainer
            root.valueContainer = childObjectContainer
            childObjectContainer.control.isForProperty = true
        } else {
            root.isAnObject = false
            root.valueContainer = paletteFunctions.__factories.createPaletteGroup(null, ef)
        }
    }


    function expand(){
        if ( childObjectContainer ){
            childObjectContainer.expand()
        }
    }

    function paletteByName(name){
        if ( isAnObject )
            return childObjectContainer.paletteByName(name)

        for ( var i = 0; i < valueContainer.children.length; ++i ){
            if ( valueContainer.children[i].name === name )
                return valueContainer.children[i]
        }

        return null
    }

    function activePaletteGroup(){
        if ( isAnObject )
            return childObjectContainer.control.activePaletteGroup()

        return valueContainer
    }

    function getLayoutState(){
        return {}
    }

    function clean(){
        if (valueContainer.objectName === "paletteGroup"){
            var pg = valueContainer
            for (var xi = 0; xi < pg.children.length; ++xi)
                if (pg.children[xi].objectName === "paletteContainer")
                    pg.children[xi].destroy()
            pg.destroy()
        }
        if (valueContainer.objectName === "objectContainer"){
            valueContainer.clean().destroy()
        }
    }
}
