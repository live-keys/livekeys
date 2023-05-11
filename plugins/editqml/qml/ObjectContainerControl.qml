import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import editqml 1.0 as EditQml

QtObject{
    id: root
    objectName: "objectContainerControl"

    property var paletteFunctions: lk.layers.workspace.extensions.editqml.paletteFunctions
    property QtObject theme: lk.layers.workspace.themes.current

    property string title: "Object"

    property QtObject editFragment : null
    property Item editor: null
    property Item pane : null
    property Item frame: null
    property Item container: null
    property Item objectContainer: null
    property Item paletteGroup: null

    property int titleHeight: 30
    property bool compact: true
    property bool isBuilder: false
    property int topSpacing: 0
    property bool isForProperty: false
    property var propertiesOpened: []

    property bool supportsFunctions: false


    function __initialize(editor, ef, objectContainer, container, frame){
        root.editor = editor
        root.editFragment = ef
        root.title = ef.typeName() + (ef.objectId() ? ("#" + ef.objectId()) : "")
        root.container = container
        root.objectContainer = objectContainer
        root.frame = frame
        ef.visualParent = objectContainer

        var paletteBoxGroup = paletteFunctions.__factories.createPaletteGroup(root.container, ef)

        paletteBoxGroup.leftPadding = 7
        paletteBoxGroup.topPadding = 7
        root.paletteGroup = paletteBoxGroup
    }

    function addProperty(ef){
        var pc = propertyByName(ef.identifier())
        if ( pc )
            return pc

        var propertyContainer = root.paletteFunctions.__factories.createPropertyContainer(root.editor, ef, root.container)
        ef.incrementRefCount()

        root.propertiesOpened.push(ef.identifier())
        root.__sortContainerChildren()
        propertyContainer.measureSizeInfo()

        return propertyContainer
    }

    function activePaletteGroup(){
        return root.paletteGroup()
    }

    function addFunctionProperty(_name){
        return null
    }

    function propertyByName(name){
        for (var i = 1; i < container.children.length; ++i){
            if (!container.children[i])
                continue
            if (container.children[i].objectName === "propertyContainer"){
                if ( container.children[i].control.editFragment.identifier() === name )
                    return container.children[i]
            }
        }
        return null
    }

    function addChildObject(ef){
        var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions

        var childObjectContainer = paletteFunctions.__factories.createObjectContainer(root.editor, ef, root.container)
        childObjectContainer.x = 20
        childObjectContainer.y = 10
        childObjectContainer.measureSizeInfo()

        ef.incrementRefCount()

        return childObjectContainer
    }

    function paletteByName(name){
        for ( var i = 0; i < root.paletteGroup.children.length; ++i ){
            if ( root.paletteGroup.children[i].name === name )
                return root.paletteGroup.children[i]
        }
        return null
    }

    function mapToPane(pane){
        return root.frame.mapToItem(pane, 0, 0)
    }

    function getPane(){
        var pane = root.frame.parent
        while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
            pane = pane.parent
        }
        return pane
    }

    function getLayoutState(){
        return { isCollapsed: root.compact }
    }

    function expand(options){
        if (!compact)
            return
        compact = false
        var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
        var language = root.editor.code.language
        var ef = root.editFragment

        var declarationsResult = language.findDeclarations(ef.position() + 1, ef.length() - 2)
        if ( declarationsResult.hasReport() ){
            for ( var i = 0; i < declarationsResult.report.length; ++i ){
                lk.layers.workspace.messages.pushErrorObject(declarationsResult.report[i])
            }
        }

        var fragmentsResult = language.openChildConnections(ef, declarationsResult.value)
        if ( fragmentsResult.hasReport() ){
            for ( var i = 0; i < fragmentsResult.report.length; ++i ){
                lk.layers.workspace.messages.pushErrorObject(fragmentsResult.report[i])
            }
        }

        var fragments = fragmentsResult.value

        var objectFragments = fragments.filter(edf => edf.location === EditQml.QmlEditFragment.Object )
        var propertyFragments = fragments.filter(edf => edf.location === EditQml.QmlEditFragment.Property )

        for (var i = 0; i < objectFragments.length; ++i){
            if ( !objectFragments[i].visualParent ){
                root.addChildObject(objectFragments[i])
            }
        }

        var expandDefaultPalette = options && options.expandDefaultPalette ? options.expandDefaultPalette : true

        for (var i = 0; i < propertyFragments.length; ++i){
            if ( !propertyFragments[i].visualParent ){
                var pc = addProperty(propertyFragments[i])
                if ( expandDefaultPalette ){
                    paletteFunctions.openPaletteInPropertyContainer(pc, paletteFunctions.defaultPalette)
                }
            }
        }

        if ( expandDefaultPalette )
            paletteFunctions.openPaletteInObjectContainer(root.objectContainer, paletteFunctions.defaultPalette)

        var id = root.editFragment.objectId()
        var check = (root.title.indexOf('#') === -1)
        if (id && check)
            root.title = root.title + "#" + id

        root.__sortContainerChildren()
    }

    function collapse(){
        if (compact)
            return
        if ( root.editFragment.fragmentType() & EditQml.QmlEditFragment.Group ){
            compact = true
            return
        }
        root.__cleanExpandedMembers()

        compact = true
        root.propertiesOpened.length = 0
        root.objectContainer.measureSizeInfo()
    }

    function closeAsPane(){
        root.frame.closeAsPane()
    }

    function clean(){
        __cleanExpandedMembers()
        root.paletteGroup.destroy()
    }

    function __cleanExpandedMembers(){
        editFragment.removePalettes()

        var pg = root.paletteGroup
        for (var xi = 0; xi < pg.children.length; ++xi)
            if (pg.children[xi].objectName === "paletteContainer")
                pg.children[xi].destroy()
        pg.children = []
        pg.palettesOpened = []

        for (var pi = 1; pi < root.container.children.length; ++pi){
            var child = root.container.children[pi]
            if (child.objectName === "propertyContainer"){
                var ef = child.control.editFragment
                child.clean().destroy()
                editor.code.language.removeConnection(ef)
            } else if (child.objectName === "objectContainer"){
                var ef = child.control.editFragment
                child.clean().destroy()
                editor.code.language.removeConnection(ef)
            }
        }
    }

    function __sortContainerChildren(){
        if (!root.container.parent)
            return
        var children = root.container.children
        if (children.length === 0)
            return

        var childrenCopy = []
        childrenCopy.push(children[0])
        children[0].z = children.length

        for (var i = 1; i < children.length; ++i){
            if (!children[i]) continue
            if (children[i].objectName === "propertyContainer"){
                childrenCopy.push(children[i])
                children[i].z = children.length - childrenCopy.length
            }
        }

        for (var i = 1; i < children.length; ++i){
            if (!children[i])
                continue
            if (children[i].objectName === "objectContainer"){
                children[i].control.topSpacing = 5
                children[i].z = children.length - childrenCopy.length
                childrenCopy.push(children[i])
            }
        }

        root.container.children = childrenCopy
    }

    function __whenEditFragmentConnectionChanged(){
        root.isBuilder = root.editFragment.isBuilder()
    }

}
