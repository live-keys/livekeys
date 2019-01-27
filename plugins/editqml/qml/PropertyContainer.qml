import QtQuick 2.0
import editor 1.0
import editor.private 1.0

Item{
    id: propertyContainer

    property string title: "Object"
    objectName: "propertyContainer"

    property Item paletteGroup : null
    property alias groupsContainer: container
    property QtObject editingFragment : null
    property QtObject documentHandler : null
    property Item editor: null
    property Item valueContainer : null

    property Component paletteGroupFactory : Component{ PaletteGroup{} }
    property Component paletteContainerFactory: Component{ PaletteContainer{} }
    property Component propertyContainerFactory: null

    property Connections editingFragmentRemovals: Connections{
        target: editingFragment
        onAboutToBeRemoved : {
            propertyContainer.destroy()
        }
    }

    width: container.width + 150
    height: container.height > 30 ? container.height + 5 : 35

    Rectangle{
        id: propertyContainerLabel
        anchors.left: parent.left
        anchors.leftMargin: 10
        height: 20
        width: 100
        color: "#062944"
        radius: 10
        Text{
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            text: propertyContainer.title
            clip: true
            color: '#82909b'
        }
    }

    Item{
        id: paletteAddButton
        anchors.left: parent.left
        anchors.leftMargin: 110
        anchors.top: parent.top
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
                            palette.item.y = 7

                            newPaletteBox.child = palette.item
                            newPaletteBox.palette = palette
                            newPaletteBox.moveEnabled = false

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
        anchors.leftMargin: 130
        anchors.top: parent.top
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
                propertyContainer.documentHandler.codeHandler.removeConnection(propertyContainer.editingFragment)
            }
        }
    }

    PaletteListView{
        id: paletteHeaderList
        visible: model ? true : false
        anchors.top: parent.top
        anchors.topMargin: 20
        width: 250
        color: "#0a141c"
        selectionColor: "#0d2639"
        fontSize: 10
        fontFamily: "Open Sans, sans-serif"
        onFocusChanged : if ( !focus ) model = null

        property var selectedHandler : function(){}
        property var cancelledHandler : function(index){}

        onPaletteSelected: selectedHandler(index)
        onCancelled : cancelledHandler()
    }

    Item{
        id: container

        anchors.top: parent.top
        anchors.topMargin: 5

        anchors.left: parent.left
        anchors.leftMargin: 140

        width: propertyContainer.valueContainer ? propertyContainer.valueContainer.width : 0
        height: propertyContainer.valueContainer ? propertyContainer.valueContainer.height : 0

        children: propertyContainer.valueContainer ? [propertyContainer.valueContainer] : []
        onChildrenChanged: console.log(children.length)
    }

}

