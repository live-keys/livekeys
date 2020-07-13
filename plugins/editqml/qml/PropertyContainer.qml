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

    property alias paletteAddButtonVisible: paletteAddButton.visible

    property bool isAnObject: false
    property var childObjectContainer: null

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls

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

    Rectangle{
        id: propertyContainerLabel
        anchors.left: parent.left
        anchors.leftMargin: 25
        anchors.top: parent.top
        anchors.topMargin: 5 + topMarginParam
        height: container.height > 30 ? container.height : 30
        width: 110
        color: propertyContainer.paletteStyle ? propertyContainer.paletteStyle.propertyLabelStyle.background : 'black'
        radius: 3
        border.width: propertyContainer.paletteStyle ? propertyContainer.paletteStyle.propertyLabelStyle.borderThickness : 1
        border.color: propertyContainer.paletteStyle ? propertyContainer.paletteStyle.propertyLabelStyle.borderColor : '#232b30'
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
                        var paletteList = paletteControls.createPaletteListView(propertyContainer, paletteStyle.selectableListView)
                        paletteList.anchors.topMargin = 15 + topMarginParam
                        paletteList.width = 250
                        paletteList.forceActiveFocus()
                        paletteList.model = palettes
                        paletteList.cancelledHandler = function(){
                            paletteList.focus = false
                            paletteList.model = null
                            paletteList.destroy()
                        }
                        paletteList.selectedHandler = function(index){
                            paletteList.focus = false
                            paletteList.model = null

                            if ( propertyContainer.valueContainer &&
                                 propertyContainer.valueContainer.objectName === 'paletteGroup' )
                            {
                                var palette = documentHandler.codeHandler.openPalette(editingFragment, palettes, index)
                                paletteControls.openPalette(palette, editingFragment, editor, propertyContainer.valueContainer)
                            }
                            paletteList.destroy()

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

