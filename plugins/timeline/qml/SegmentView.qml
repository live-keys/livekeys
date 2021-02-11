import QtQuick 2.0
import timeline 1.0

Loader{
    id: root

    x: position * root.zoom
    width: length * root.zoom
    height: 15
    clip: true
    objectName: "segmentView"

    property color focusBorderColor: "white"
    property color borderColor: "grey"
    property color backgroundColor: "transparent"
    property double zoom: 1

    property QtObject timelineRow : null

    function __segmentFocused(segment){
        if ( root.parent )
            root.parent.segmentFocused(segment)
    }

    signal segmentDoubleClicked(Segment segment)
    onSegmentDoubleClicked: {
        if ( timelineRow )
            timelineRow.segmentDoubleClicked(segment)
    }

    sourceComponent: segment
        ? (segment instanceof Keyframe) ? keyframe : resizableSegment
        : null

    onYChanged: {
        if ( y !== 3 ){
            if ( Math.abs(y) > root.height ){
                if ( timelineRow === null ){
                    timelineRow = root.parent
                }

                var rowDelegate = timelineRow.parent
                var timelineView = rowDelegate.parentView

                var newIndex = Math.round(y / rowDelegate.height)

                if ( newIndex * rowDelegate.height < timelineView.contentHeight ){
                    if ( newIndex >= 0){
                        y = newIndex * rowDelegate.height + 3
                        root.parent = timelineView
                        return
                    } else {
                        y = 3
                        root.parent = timelineView
                        return
                    }
                } else {
                    y = timelineView.contentHeight - rowDelegate.height + 3
                    root.parent = timelineView
                    return
                }
            }
            y = 3
        }
    }

    property Component resizableSegment : Component{

        Item{
            id: rootItem
            anchors.fill: parent

            Keys.onPressed: {
                if (event.key === Qt.Key_Delete || event.key === Qt.Key_Backspace) {
                    event.accepted = true
                    segment.remove()
                } else if (event.key === Qt.Key_Left){

                    if (event.modifiers & Qt.ShiftModifier) {
                        var newLength = length - 1
                        if (newLength > 0) {
                            segment.length = newLength
                            root.width = newLength * root.zoom
                        }
                    } else {
                        var newPosition = position - 1
                        if (newPosition >= 0) {
                            segment.position = newPosition
                            root.x = newPosition * root.zoom
                        }
                    }
                    event.accepted = true
                } else if (event.key === Qt.Key_Right) {
                    if (event.modifiers & Qt.ShiftModifier) {
                        var newLength = length + 1
                        if (position + length < segment.segmentModel().contentWidth) {
                            segment.length = newLength
                            root.width = newLength * root.zoom
                        }
                    } else {
                        var newPosition = position + 1
                        if (position + length < segment.segmentModel().contentWidth) {
                            segment.position = newPosition
                            root.x = newPosition * root.zoom
                        }
                    }
                    event.accepted = true
                }
            }

            Rectangle {
                id: centerMove
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0
                width: parent.width
                height: parent.height
                color: root.backgroundColor
                border.width: 1
                border.color: root.borderColor

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#fff"
                    text: segment ? segment.label : ""
                    font.family: "Open Sans, sans-serif"
                    font.pixelSize: 10
                }

                MouseArea {
                    anchors.fill: parent
                    drag.target: root
                    drag.axis: Drag.XAndYAxis
                    drag.minimumX: 0
                    drag.maximumX: segment ? segment.segmentModel().contentWidth - root.width : 0

                    onPressed: {
                        rootItem.forceActiveFocus()
                        root.__segmentFocused(segment)
                    }
                    onDoubleClicked: {
                        root.segmentDoubleClicked(segment)
                    }
                    onReleased: {
                        if ( root.timelineRow && root.parent !== root.timelineRow ){
                            var prevPosition = segment.position

                            var rowDelegate = root.timelineRow.parent
                            var timelineView = rowDelegate.parentView

                            var index = timelineView.indexAt(1, root.y)
                            var track = timelineView.model.trackAt(index)

                            if ( track.segmentModel !== segment.segmentModel() ){
                                var prevSegmentModel = segment.segmentModel()
                                if ( prevSegmentModel.takeSegment(segment) ){

                                    segment.position = Math.round(root.x / root.zoom)
                                    if (!track.addSegment(segment) ){
                                        segment.position = prevPosition
                                        prevSegmentModel.addSegment(segment)
                                    }

                                    root.x = position * root.zoom
                                }
                            } else {
                                root.parent = timelineRow
                                root.y = 3
                            }
                        } else {
                            segment.position = Math.round(root.x / root.zoom)
                            root.x = position * root.zoom
                        }
                    }
                }
            }

            Rectangle {
                id: stretchLeft
                anchors.left: parent.left
                anchors.top: parent.top
                width: 1
                height: parent.height
                color: rootItem.activeFocus ? root.focusBorderColor : root.borderColor

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.SizeHorCursor

                    property int oldMouseX

                    onPressed: {
                        root.__segmentFocused(segment)
                        oldMouseX = mouseX
                    }
                    onPositionChanged: {
                        if (pressed) {
                            var newWidth = root.width + oldMouseX - mouseX
                            var newLength = Math.round(newWidth / root.zoom)
                            var newX = root.x + mouseX - oldMouseX

                            if ( newLength - segment.length > segment.maxStretchLeft ){
                                newLength = segment.length + segment.maxStretchLeft
                                newWidth = Math.round(newLength * root.zoom)
                                newX = root.x + root.width - newWidth
                            }

                            if (newLength > 0
                                    && newX + newWidth < segment.segmentModel().contentWidth) {
                                if (newX > 0) {
                                    root.width = newWidth
                                    root.x = newX
                                } else {
                                    root.width = root.x + root.width
                                    root.x = 0
                                }
                            } else {
                                root.x = root.x
                                        + root.width - (1 * root.zoom)
                                root.width = 1 * root.zoom
                            }
                        }
                    }

                    onReleased: {
                        segment.stretchLeftTo(Math.round(root.x / root.zoom))
                        root.width = length * root.zoom
                        root.x = position * root.zoom
                    }
                }
            }
            Rectangle {
                id: strechRight
                anchors.right: parent.right
                anchors.top: parent.top
                width: 1
                height: parent.height
                color: rootItem.activeFocus ? root.focusBorderColor : root.borderColor

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.SizeHorCursor

                    property int oldMouseX

                    onPressed: {
                        root.__segmentFocused(segment)
                        oldMouseX = mouseX
                    }
                    onPositionChanged: {
                        if (pressed) {
                            var newWidth = root.width + (mouseX - oldMouseX)
                            var newLength = Math.round(newWidth / root.zoom)

                            var lengthDelta = newLength - segment.length

                            if ( lengthDelta > segment.maxStretchRight ){
                                newLength = segment.length + segment.maxStretchRight
                                newWidth = Math.round(newLength * root.zoom)
                            }

                            if (newLength > 0) {
                                if (position + newLength < segment.segmentModel().contentWidth)
                                    root.width = newWidth
                                else
                                    root.width = (root.modelLength - position) * root.zoom
                            } else
                                root.width = 1 * root.zoom
                        }
                    }
                    onReleased: {
                        segment.stretchRightTo(Math.round((root.x + root.width) / root.zoom))
                        root.width = length * root.zoom
                    }
                }
            }
        }
    }

    property Component keyframe : Component{

        Item{
            id: keyframeItem

            width: parent.height
            height: parent.height

            Keys.onPressed: {
                if (event.key === Qt.Key_Delete || event.key === Qt.Key_Backspace) {
                    event.accepted = true
                    segment.remove()
                }
            }

            Rectangle{
                anchors.centerIn: parent
                width: parent.width * 0.70
                height: parent.height * 0.70
                color: root.backgroundColor
                border.width: 1
                border.color: keyframeItem.activeFocus ? root.focusBorderColor : root.borderColor
                rotation: 45
            }


            MouseArea{
                anchors.fill: parent
                drag.target: root
                drag.axis: Drag.XAndYAxis
                drag.minimumX: -7.5
                drag.maximumX: segment ? (segment.segmentModel().contentWidth - 7.5 - 1) : 0

                onPressed: {
                    root.__segmentFocused(segment)
                    keyframeItem.forceActiveFocus()
                }
                onDoubleClicked: {
                    root.segmentDoubleClicked(segment)
                }
                onReleased: {
                    segment.position = Math.round((root.x + 7.5) / root.zoom)
                    root.y = 3
                    root.x = segment.position * root.zoom - 7.5
                }
            }

            Component.onCompleted: {
                parent.width = parent.height
                root.x = segment.position * root.zoom - 7.5
            }
        }
    }

}



