import QtQuick 2.0
import editor 1.0
import editor.private 1.0

Item{
    id: propertyContainer

    property string title: "Object"
    objectName: "propertyContainer"

    property QtObject theme: lk.layers.workspace.themes.current

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
        function onAboutToBeRemoved(){
            if (!isAnObject)
                propertyContainer.destroy()
        }
    }
    width: container.width + 120
    height: container.height > 35 ? container.height : 35
    z: 3000

    onWidthChanged: {
        if (!propertyContainer || !propertyContainer.parent || !propertyContainer.parent.parent ||
                !propertyContainer.parent.parent.parent) return
        var objectContainer = propertyContainer.parent.parent.parent

        if (objectContainer.objectName === "objectContainer")
            objectContainer.recalculateContentWidth()
    }

    Rectangle{
        id: propertyContainerLabel
        anchors.left: parent.left
        anchors.leftMargin: 25
        anchors.top: parent.top
        anchors.topMargin: 5
        height: container.height > 30 ? container.height : 30
        width: 110
        color: theme.colorScheme.middleground
        radius: 3
        border.width: theme.inputStyle.borderThickness
        border.color: theme.colorScheme.middlegroundBorder
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
                    var pane = propertyContainer.parent
                    while (pane && pane.objectName !== "editor" && pane.objectName !== "objectPalette"){
                        pane = pane.parent
                    }
                    var coords = paletteAddButton.mapToItem(pane, 0, 0)

                    var paletteList = paletteControls.addPaletteList(
                        propertyContainer,
                        propertyContainer.valueContainer,
                        Qt.rect(coords.x + 150, coords.y, 30, 30),
                        PaletteControls.PaletteListMode.PropertyContainer
                    )

                    if (paletteList){
                        paletteList.width = 250
                        paletteList.x -= 70
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
                    var objectContainer = propertyContainer.parent.parent.parent

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
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 140

        color: 'black'

        width: propertyContainer.valueContainer ? propertyContainer.valueContainer.width : 0
        height: propertyContainer.valueContainer ? propertyContainer.valueContainer.height : 0

        children: propertyContainer.valueContainer ? [propertyContainer.valueContainer] : []
    }

}

