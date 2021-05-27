import QtQuick 2.3
import QtQuick.Controls 1.3
import timeline 1.0
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons

Item{
    id: root
    width: parent.width
    height: 25

    objectName: "timelineTrackTitle"

    property Item timelineArea: null
    property QtObject timelineStyle : null
    property int trackIndex : 0

    property var menuOptions: null

    signal addSegment(int index)
    signal rightClicked(int index, Item trackTitleItem)

    property alias labelColor: editableLabel.textColor
    property alias borderColor: borderBottom.color
    property alias iconColor: menuIcon.color

    Rectangle{
        id: borderBottom
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: root.timelineStyle.headerBorderColor
    }

    Rectangle{
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 1
        width: 15
        height: parent.height - 1
        color: 'transparent'

        Icons.MenuIcon{
            id: menuIcon
            width: 2
            height: 10
            anchors.centerIn: parent
            color: root.timelineStyle.iconColor
        }

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                root.timelineArea.handleContextMenu(root)
            }
        }
    }

    Workspace.EditableLabel{
        id: editableLabel
        anchors.left: parent.left
        anchors.leftMargin: 20
        height: parent.height
        text: track.name
        style: root.timelineStyle.inputStyle
        onTextChanged: {
            track.name = text
        }
        onRightClicked: root.rightClicked(root.trackIndex, root)
    }

    Component.onCompleted: {
        // load menu for this track type
        root.menuOptions = timelineArea.timeline.config.trackMenu(track)
    }
}
