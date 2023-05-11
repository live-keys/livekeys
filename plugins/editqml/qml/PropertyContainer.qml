import QtQuick 2.0
import editor 1.0
import editor.private 1.0
import editqml 1.0 as EditQml
import workspace.icons 1.0 as Icons

Item{
    id: propertyContainer
    objectName: "propertyContainer"
    width: 0
    height: 0
    property var sizeInfo: ({minWidth: -1, minHeight: -1, maxWidth: -1, maxHeight: -1})
    z: 3000

    property PropertyContainerControl control: PropertyContainerControl{}

    property alias paletteAddButtonVisible: paletteAddButton.visible

    function __initialize(editor, ef){
        propertyContainer.control.__initialize(editor, ef, container)
        ef.visualParent = propertyContainer
    }

    function clean(){
        propertyContainer.control.clean()
        return propertyContainer
    }

    function measureSizeInfo(){
        var size = { minWidth: 0, minHeight: -1, maxWidth: -1, maxHeight: -1, contentWidth: -1 }
        for ( var i = 0; i < container.children.length; ++i ){
            var si = container.children[i].sizeInfo
            if ( si.minWidth > 0 ){
                if ( size.minWidth < 0 )
                    size.minWidth = si.minWidth
                else if ( size.minWidth < si.minWidth){
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
        if ( size.minHeight < 35 )
            size.minHeight = 35

        size.minWidth += 140

        propertyContainer.sizeInfo = size

        if ( propertyContainer.parent && propertyContainer.parent.measureSizeInfo ){
            propertyContainer.parent.measureSizeInfo()
        }
    }

    function adjustSize(){
        this.width = propertyContainer.parent.width
        this.sizeInfo.contentWidth = this.width - 140

        var height = 0
        if ( control.valueContainer.adjustSize ){
            control.valueContainer.adjustSize()
            height += control.valueContainer.height
        } else {
            height = 30
        }

        this.height = height > 30 ? height : 30
    }


    property Connections editFragmentRemovals: Connections{
        target: control.editFragment
        function onAboutToBeRemoved(){
            if (!propertyContainer.control.isAnObject){
                propertyContainer.destroy()
            }
        }
    }

    Rectangle{
        id: propertyContainerLabel
        anchors.left: parent.left
        anchors.leftMargin: 25
        anchors.top: parent.top
        anchors.topMargin: 5
        height: container.height > 30 ? container.height : 30
        width: 110
        color: propertyContainer.control.theme.colorScheme.middleground
        radius: 3
        border.width: propertyContainer.control.theme.inputStyle.borderThickness
        border.color: propertyContainer.control.theme.colorScheme.middlegroundBorder

        Loader{
            id: iconLoader
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.right: parent.right
            anchors.rightMargin: 20
            sourceComponent: {
                if ( !propertyContainer.editFragment )
                    return null
                return propertyContainer.editFragment.location === EditQml.QmlEditFragment.Slot
                    ? propertyContainer.eventIcon
                    : propertyContainer.editFragment.isMethod() ? propertyContainer.methodIcon : null
            }
        }

        Text{
            anchors.left: parent.left
            anchors.leftMargin: 25
            anchors.right: parent.right
            anchors.rightMargin: iconLoader.sourceComponent ? 38 : 20
            anchors.top: parent.top
            anchors.topMargin: 6
            text: propertyContainer.control.title
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
            visible: !propertyContainer.isAnObject
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

                    var paletteList = control.paletteFunctions.views.openPaletetListBoxForContainer(
                        propertyContainer,
                        propertyContainer.control.valueContainer,
                        Qt.rect(coords.x + 150, coords.y, 30, 30),
                        PaletteFunctions.PaletteListMode.PropertyContainer
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
            anchors.leftMargin: 4
            anchors.top: parent.top
            anchors.topMargin: 4
            width: 15
            height: 20
            Icons.XIcon{
                width: 6
                height: 6
                strokeWidth: 1
                color: propertyContainer.control.theme.colorScheme.foregroundFaded
            }

            MouseArea{
                id: propertyCloseArea
                anchors.fill: parent
                onClicked: {
                    var objectContainer = propertyContainer.parent.parent.parent

                    propertyContainer.control.editFragment.decrementRefCount()
                    if (propertyContainer.control.editFragment.refCount === 0)
                        objectContainer.control.editFragment.removeChildFragment(propertyContainer.control.editFragment)

                    // remove name from objectContainer
                    objectContainer.control.propertiesOpened =
                        objectContainer.control.propertiesOpened.filter(
                            function(value){
                                return value !== propertyContainer.control.editFragment.identifier()
                            }
                        )
                    if ( propertyContainer.parent && propertyContainer.parent.measureSizeInfo ){
                        var p = propertyContainer.parent
                        propertyContainer.parent = null
                        p.measureSizeInfo()
                    }

                    propertyContainer.destroy()
                }
            }
        }
    }

    Item{
        id: container

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 140

        function measureSizeInfo(){
            propertyContainer.measureSizeInfo()
        }

        property alias sizeInfo: propertyContainer.sizeInfo

        width: propertyContainer.control.valueContainer ? propertyContainer.control.valueContainer.width : 0
        height: propertyContainer.control.valueContainer ? propertyContainer.control.valueContainer.height : 0

        children: propertyContainer.control.valueContainer ? [propertyContainer.control.valueContainer] : []
    }

}

