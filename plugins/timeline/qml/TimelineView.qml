import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import timeline 1.0
import workspace 1.0

Rectangle{
    id: root

    width : parent.width
    color: timelineStyle.borderColor

    Keys.onPressed: {
        if ( event.key === Qt.Key_Space ){
            if ( root.timeline.isRunning )
                root.timeline.stop()
            else
                root.timeline.start()
        }
    }

    property var surface : null
    property int zoom : 1

    property TimelineStyle timelineStyle : TimelineStyle{}
    property Timeline timeline : Timeline{
        fps: 30
        contentLength: 100 * fps
        headerModel.scale: root.zoom
    }
    onTimelineChanged: {
        root.timeline.headerModel.scale = Qt.binding(function(){return root.zoom })
    }
    property TimelineConfig timelineConfig : TimelineConfig{
    }

    property Component segmentDelegate: SegmentView{
        y: 3
        zoom: root.zoom
        height: 18
        backgroundColor: root.timelineStyle.segmentBackground
        borderColor: root.timelineStyle.segmentBorder
        focusBorderColor: root.timelineStyle.segmentBorderFocus
    }

    property Component trackTitleDelegate : TrackTitle{
        trackIndex: index
        timelineStyle: root.timelineStyle
    }

    property Item timelineOptions : Item{
        anchors.fill: parent
        MenuIcon{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            height: 10
            width: 10
            color: root.timelineStyle.iconColor
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    contextMenu.popup()
                }
            }
        }
        PlayPause{
            width: 15
            height: 15
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 40
            isRunning: root.timeline.isRunning
            color: root.timelineStyle.iconColor
            onClicked : {
                if ( value )
                    root.timeline.start()
                else
                    root.timeline.stop()
            }
        }
        Text{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 68
            text: root.timeline.positionToLabel(timelineArea.timeline.cursorPosition, false)
            color: root.timelineStyle.textColor
            font.family : "Source Code Pro, Courier New, Courier"
            font.pixelSize: 14
            font.weight : Font.Normal
        }

        Menu{
            id: contextMenu
            MenuItem {
                text: qsTr("Insert Track")
                onTriggered: root.timeline.addTrack()
            }
        }
    }

    signal mouseHover(int position, int trackIndex)
    signal mouseLeave(int position)
    signal mouseDoubleClicked(int position, int trackIndex)
    signal segmentSelected(Track track, Segment segment)
    signal segmentDoubleClicked(Track track, Segment segment)

    Rectangle{
        id: timelineOptionsContainer
        width: timelineLeftHeader.width
        height: 35
        color: root.timelineStyle.topHeaderBackgroundColor

        children: [root.timelineOptions]
    }

    Rectangle{
        id: timelineLeftHeader
        color: root.timelineStyle.headerRowBackground

        anchors.top: parent.top
        anchors.topMargin: 35

        height: parent.height - 35
        clip: true
        width: 150

        ListView{
            id: timelineTrackLabels

            anchors.fill: parent

            interactive: false
            contentY: timelineView.contentY

            model: root.timeline.trackList
            delegate: root.trackTitleDelegate

            MouseArea{
                anchors.fill: parent
                onWheel: {
                    wheel.accepted = true
                    var newContentY = timelineView.contentY -= wheel.angleDelta.y / 6
                    if ( newContentY > timelineView.contentHeight - timelineView.height )
                        timelineView.contentY = timelineView.contentHeight - timelineView.height
                    else if ( newContentY < 0 )
                        timelineView.contentY = 0
                    else
                        timelineView.contentY = newContentY
                }
                onClicked: mouse.accepted = false;
                onPressed: mouse.accepted = false;
                onReleased: mouse.accepted = false
                onDoubleClicked: mouse.accepted = false;
                onPositionChanged: mouse.accepted = false;
                onPressAndHold: mouse.accepted = false;
            }
        }

        Rectangle{
            width: parent.width
            height: 10
            color: root.timelineStyle.scrollBackground
            anchors.bottom: parent.bottom
        }
    }

    Flickable{
        contentX: timelineView.contentX
        anchors.left: parent.left
        anchors.leftMargin: 150
        height: parent.height
        width: parent.width - 150
        interactive: false
        clip: true

        Rectangle{
            id : timelineTopHeader

            width : root.zoom * root.timeline.contentLength
            height : 35
            color : root.timelineStyle.topHeaderBackgroundColor

            RangeView{
                id: timelineHeaderView
                height: parent.height
                width: parent.width
                model: root.timeline.headerModel

                viewportX: timelineView.contentX
                viewportWidth: timelineView.width

                delegate: Component{
                    Item{
                        x: position
                        height : timelineTopHeader.height

                        Text{
                            anchors.top: parent.top
                            anchors.topMargin: 10
                            anchors.left: isDelimiter ? parent.left : undefined
                            anchors.horizontalCenter: isDelimiter ? undefined : parent.horizontalCenter

                            text: root.timeline.positionToLabel(label, true)
                            color: root.timelineStyle.markerColor
                            font.pixelSize: 8
                            visible: hasLabel
                        }

                        Rectangle{
                            width: 1
                            height: hasLabel ? 8 : 5
                            y: 33 - height
                            color: root.timelineStyle.markerLabelColor
                        }
                    }
                }
            }

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onMouseXChanged: {
                    if ( pressed ){
                        var seekPosition = Math.floor(mouse.x / root.zoom)
                        root.timeline.cursorPosition = seekPosition
                    }
                }
            }


            Rectangle{
                anchors.bottom: parent.bottom
                width : 1
                height : parent.height / 2
                color: root.timelineStyle.cursorColor

                x: root.timeline.cursorPosition * root.zoom
                onXChanged: {
                    if ( x > mainScroll.flickableItem.contentX + timeline.width - 10 ){
                        var newContentXForwardPos = x - 30
                        if ( newContentXForwardPos > mainScroll.flickableItem.contentWidth - timeline.width )
                            newContentXForwardPos = mainScroll.flickableItem.contentWidth - timeline.width
                        mainScroll.flickableItem.contentX = newContentXForwardPos
                    } else if ( x < mainScroll.flickableItem.contentX ){
                        var newContentXBackPos = x - timeline.width + 30
                        if ( newContentXBackPos < 0 )
                            newContentXBackPos = 0
                        mainScroll.flickableItem.contentX = newContentXBackPos
                    }
                }
            }

        } // end header

    }

    Rectangle{
        id: timeline

        anchors.top: parent.top
        anchors.topMargin: 35

        height: parent.height - 35
        width: parent.width - 150
        x: 150

        ScrollView{
            id: mainScroll
            anchors.fill: parent

            style: ScrollViewStyle {
                transientScrollBars: false
                handle: root.timelineStyle.scrollStyleHandle
                scrollBarBackground: root.timelineStyle.scrollStyleBackground
                decrementControl: null
                incrementControl: null
                frame: root.timelineStyle.scrollStyleFrame
                corner: root.timelineStyle.scrollStyleCorner
            }

            ListView{
                id: timelineView
                boundsBehavior: Flickable.StopAtBounds

                height: parent.height
                contentWidth: root.timeline.contentLength * root.zoom + 5
                model: root.timeline.trackList
                delegate: Rectangle{
                    height: 25
                    width: timelineRow.width
                    color: root.timelineStyle.borderColor

                    TimelineRow{
                        id: timelineRow
                        color: root.timelineStyle.rowBackground
                        height: 24
                        anchors.top: parent.top

                        zoomScale: root.zoom

                        elementColor: root.timelineStyle.segmentBackground
                        elementBorderColor: root.timelineStyle.segmentBorder

                        viewportX: timelineView.contentX / timelineRow.zoomScale
                        viewportWidth: timelineView.width / timelineRow.zoomScale
                        model: track.segmentModel
                        modelLength: root.timeline.contentLength

                        segmentDelegate: root.segmentDelegate

                        onSegmentDoubleClicked: {
                            root.segmentDoubleClicked(track, segment)
                        }
                    }

                    Rectangle{
                        width : 1
                        height : parent.height
                        color: root.timelineStyle.cursorColor
                        x: root.timeline.cursorPosition * root.zoom
                    }
                }
            }

        }
    }
}

