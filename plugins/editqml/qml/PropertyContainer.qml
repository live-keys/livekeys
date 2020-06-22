import QtQuick 2.0
import editor 1.0
import editor.private 1.0

Item{
    id: propertyContainer

    property string title: "Object"
    objectName: "propertyContainer"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    property Item paletteGroup : null
    property alias groupsContainer: container
    property QtObject editingFragment : null
    property QtObject documentHandler : null
    property Item editor: null
    property Item valueContainer : null

    property Component paletteGroupFactory : Component{ PaletteGroup{} }
    property Component paletteContainerFactory: Component{ PaletteContainer{} }
    property Component propertyContainerFactory: null

    property alias paletteAddButtonVisible: paletteAddButton.visible

    property bool isAnObject: false
    property var childObjectContainer: null

    property Connections editingFragmentRemovals: Connections{
        target: editingFragment
        onAboutToBeRemoved : {
            propertyContainer.destroy()
        }
    }
    property int topMarginParam: 0
    width: container.width + 120
    height: container.height > 35 ? container.height : 35
    z: 3000

    onWidthChanged: {
        if (!propertyContainer || !propertyContainer.parent || !propertyContainer.parent.parent ||
                !propertyContainer.parent.parent.parent) return
        var objectContainer = propertyContainer.parent.parent.parent

        objectContainer.recalculateContentWidth()
    }

    function expandPalette(paletteName){
        var palette = editor.documentHandler.codeHandler.expand(editingFragment, {
            "palettes" : [paletteName]
        })

        var newPaletteBox = paletteContainerFactory.createObject(propertyContainer.valueContainer)

        palette.item.x = 5
        palette.item.y = 2

        newPaletteBox.child = palette.item
        newPaletteBox.palette = palette

        newPaletteBox.name = palette.name
        newPaletteBox.type = palette.type
        newPaletteBox.moveEnabledSet = false
        newPaletteBox.documentHandler = propertyContainer.editor.documentHandler
        newPaletteBox.cursorRectangle = propertyContainer.editor.getCursorRectangle()
        newPaletteBox.editorPosition = propertyContainer.editor.cursorWindowCoords()
        newPaletteBox.paletteContainerFactory = function(arg){
            return parent.paletteContainerFactory.createObject(arg)
        }
    }

    function expandDefaultPalette(){
        var defaultPaletteName = editor.documentHandler.codeHandler.defaultPalette(editingFragment)
        if ( defaultPaletteName.length ){
            expandPalette(defaultPaletteName)
        }
    }

    Rectangle{
        id: propertyContainerLabel
        anchors.left: parent.left
        anchors.leftMargin: 25
        anchors.top: parent.top
        anchors.topMargin: 5 + topMarginParam
        height: container.height
        width: 110
        color: propertyContainer.paletteStyle ? propertyContainer.paletteStyle.propertyLabelStyle.background : 'black'
        radius: 3
        border.width: propertyContainer.paletteStyle ? propertyContainer.paletteStyle.propertyLabelStyle.borderThickness : 0
        border.color: propertyContainer.paletteStyle ? propertyContainer.paletteStyle.propertyLabelStyle.borderColor : 'black'
        Text{
            anchors.left: parent.left
            anchors.leftMargin: 25
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.top: parent.top
            anchors.topMargin: 6
            text: propertyContainer.title
            clip: true
            color: '#82909b'
        }

        Item{
            id: paletteAddButton
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 4

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
                    var palettes = propertyContainer.documentHandler.codeHandler.findPalettes(
                        editingFragment.position(), true)
                    if (palettes.size() ){
                        paletteHeaderList.forceActiveFocus()
                        paletteHeaderList.model = palettes
                        paletteHeaderList.cancelledHandler = function(){
                            paletteHeaderList.focus = false
                            paletteHeaderList.model = null
                        }
                        paletteHeaderList.selectedHandler = function(index){
                            paletteHeaderList.focus = false
                            paletteHeaderList.model = null

                            if ( propertyContainer.valueContainer &&
                                 propertyContainer.valueContainer.objectName === 'paletteGroup' )
                            {
                                var palette = documentHandler.codeHandler.openPalette(editingFragment, palettes, index)

                                var newPaletteBox = paletteContainerFactory.createObject(propertyContainer.valueContainer)

                                palette.item.x = 5
                                palette.item.y = 2

                                newPaletteBox.child = palette.item
                                newPaletteBox.palette = palette
                                newPaletteBox.moveEnabledSet = false

                                newPaletteBox.name = palette.name
                                newPaletteBox.type = palette.type
                                newPaletteBox.documentHandler = documentHandler
                                newPaletteBox.cursorRectangle = propertyContainer.editor.getCursorRectangle()
                                newPaletteBox.editorPosition = editor.cursorWindowCoords()
                                newPaletteBox.paletteContainerFactory = function(arg){
                                    return propertyContainer.paletteContainerFactory.createObject(arg)
                                }

                            }

                        }
                    }
                }
            }
        }

        Item{
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.top: parent.top
            anchors.topMargin: 2
            width: 15
            height: 20
            Text{
                anchors.verticalCenter: parent.verticalCenter
                text: 'x'
                color: '#ffffff'
            }
            MouseArea{
                id: propertyCloseArea
                anchors.fill: parent
                onClicked: {
                    var objectContainer = propertyContainer.parent.parent

                    propertyContainer.editingFragment.decrementRefCount()
                    if (propertyContainer.editingFragment.refCount === 0)
                        objectContainer.editingFragment.removeChildFragment(propertyContainer.editingFragment)

                    // remove name from objectContainer
                    objectContainer.propertiesOpened =
                            objectContainer.propertiesOpened.filter(
                                function(value, index, arr){
                                    return value !== propertyContainer.editingFragment.identifier()
                                }
                            )
                    propertyContainer.destroy()
                }
            }
        }

    }




    PaletteListView{
        id: paletteHeaderList
        visible: model ? true : false
        anchors.top: parent.top
        anchors.topMargin: 15 + topMarginParam
        width: 250
        color: "#0a141c"
        selectionColor: "#0d2639"
        fontSize: 10
        fontFamily: "Open Sans, sans-serif"
        onFocusChanged : if ( !focus ) model = null
        z: 3000

        property var selectedHandler : function(){}
        property var cancelledHandler : function(index){}

        onPaletteSelected: selectedHandler(index)
        onCancelled : cancelledHandler()
    }

    Rectangle{
        id: container

        anchors.top: parent.top
        anchors.topMargin: 5 + topMarginParam
        anchors.left: parent.left
        anchors.leftMargin: 140

        color: 'black'

        width: propertyContainer.valueContainer ? propertyContainer.valueContainer.width : 0
        height: propertyContainer.valueContainer ? propertyContainer.valueContainer.height : 0

        children: propertyContainer.valueContainer ? [propertyContainer.valueContainer] : []
    }

}

