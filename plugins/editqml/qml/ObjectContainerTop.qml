import QtQuick 2.3
import workspace 1.0 as Workspace
import editqml 1.0
import workspace.icons 1.0 as Icons
import visual.shapes 1.0

Rectangle{
    id: objectContainerTitle
    radius: 3

    property QtObject theme: lk.layers.workspace.themes.current

    property bool compact : true
    property bool isBuilder : false

    property var objectContainer: null

    signal toggleCompact()
    signal erase()
    signal rebuild()
    signal toggleConnections()
    signal assignFocus()
    signal addPalette()
    signal compose()
    signal paletteToPane()
    signal close()
//    signal toggleBuilder()
    signal createObject()

    MouseArea{
        anchors.fill: parent
        onClicked: objectContainerTitle.assignFocus()
    }

    HeadingButton{
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 12
        visible: !objectContainer.pane
        content: theme.icons.headingCollapseExpand
        onContentItemChanged: contentItem.compact = Qt.binding(function(){ return objectContainerTitle.compact })
        onClicked: objectContainerTitle.toggleCompact()
    }

    Text{
        anchors.left: parent.left
        anchors.leftMargin: 40
        width: parent.width - editIcons.width - 50
        anchors.verticalCenter: parent.verticalCenter
        text: objectContainer.control.title
        clip: true
        elide: Text.ElideRight
        color: '#82909b'
    }

    Row{
        id: editIcons
        height: parent.height
        anchors.right: parent.right
        anchors.rightMargin: 5
        spacing: 3

//        Item{
//            id: itemSelection
//            anchors.verticalCenter: parent.verticalCenter
//            width: 15
//            height: 20
//            Rectangle{
//                anchors.top: parent.top
//                anchors.topMargin: 2
//                anchors.left: parent.left
//                anchors.leftMargin: 3
//                width: 10
//                height: 10
//                radius: 2
//            }
//            MouseArea{
//                id: itemSelectionMouseArea
//                anchors.fill: parent
//                hoverEnabled: true
//                onClicked: {
//                    objectContainerTitle.toggleBuilder()
//                }
//            }
//            Workspace.Tooltip{
//                mouseOver: itemSelectionMouseArea.containsMouse
//                text: "Toggle builder"
//            }
//        }

        HeadingButton{
            anchors.verticalCenter: parent.verticalCenter
            content: theme.icons.moveToNewPane
            tooltip: "Move to new pane"
            onClicked: objectContainerTitle.paletteToPane()
        }

        HeadingButton{
            anchors.verticalCenter: parent.verticalCenter
            visible: objectContainer.control.isForProperty && objectContainer.control.editFragment && objectContainer.control.editFragment.isNull
            content: theme.icons.createObject
            tooltip: "Create Object"
            onClicked: objectContainerTitle.createObject()
        }

        HeadingButton{
            anchors.verticalCenter: parent.verticalCenter
            visible: objectContainer.control.editFragment && !(objectContainer.control.editFragment.fragmentType() & QmlEditFragment.ReadOnly)
            content: theme.icons.eraseObject
            tooltip: "Erase Object"
            onClicked: objectContainerTitle.erase()
        }

        HeadingButton{
            anchors.verticalCenter: parent.verticalCenter
            visible: objectContainerTitle.isBuilder
            content: theme.icons.buildSection
            tooltip: "Rebuild Section"
            onClicked: objectContainerTitle.rebuild()
        }

        HeadingButton{
            anchors.verticalCenter: parent.verticalCenter
            content: theme.icons.viewConnections
            tooltip: "View Connections"
            onClicked: objectContainerTitle.toggleConnections()
        }

        HeadingButton{
            anchors.verticalCenter: parent.verticalCenter
            content: theme.icons.openPalette
            tooltip: "Open Palette"
            onClicked: objectContainerTitle.addPalette()
        }

        HeadingButton{
            anchors.verticalCenter: parent.verticalCenter
            visible: objectContainer.control.editFragment
                  && objectContainer.control.editFragment.type() !== 'qml/QtQuick#Component'
                  && !((objectContainer.control.editFragment.fragmentType() & QmlEditFragment.ReadOnly)
                  && ! (objectContainer.control.editFragment.fragmentType() & QmlEditFragment.Group))
            content: theme.icons.addContent
            tooltip: "Add Content"
            onClicked: objectContainerTitle.compose()
        }

        HeadingButton{
            anchors.verticalCenter: parent.verticalCenter
            visible: !(objectContainer.control.editFragment && objectContainer.control.editFragment.parentFragment()) && !objectContainer.control.pane
            content: theme.icons.closeObject
            tooltip: "Close"
            onClicked: objectContainerTitle.close()
        }
    }
}
