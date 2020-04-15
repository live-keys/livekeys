import QtQuick 2.3
import QtQuick.Controls 1.3
import timeline 1.0
import workspace 1.0

Item{
    id: root
    width: parent.width
    height: 25

    property TimelineStyle timelineStyle : null
    property int trackIndex : 0

    signal addSegment(int index)

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

        MenuIcon{
            id: menuIcon
            width: 5
            height: 6
            anchors.centerIn: parent
            color: root.timelineStyle.iconColor
        }

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                contextMenu.popup()
            }
        }
    }

    EditableLabel{
        id: editableLabel
        anchors.left: parent.left
        anchors.leftMargin: 20
        height: parent.height
        text: track.name
        textColor: root.timelineStyle.textColor
        onTextChanged: {
            track.name = text
        }
    }

    Menu{
        id: contextMenu
        MenuItem {
            text: qsTr("Add Segment...")
            onTriggered: root.addSegment(root.trackIndex)
        }
        MenuItem {
            text: qsTr("Insert Track")
            onTriggered: timelineArea.timeline.addTrack()
        }
        MenuItem {
            text: qsTr("Remove Track")
            onTriggered: timelineArea.timeline.removeTrack(index)
        }
    }
}
