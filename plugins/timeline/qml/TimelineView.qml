import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import timeline 1.0
import fs 1.0 as Fs
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons

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
    property bool interactive: true


    signal mouseHover(int position, int trackIndex)
    signal mouseLeave(int position)
    signal mouseDoubleClicked(int position, int trackIndex)
    signal segmentSelected(Track track, Segment segment)
    signal segmentDoubleClicked(Track track, Segment segment, Item delegate)
    signal segmentRightClicked(Track track, Segment segment, Item delegate)
    signal trackTitleRightClicked(int index, Item delegate)

    property SegmentInsertMenu segmentInsertMenu: segmentInsertMenu

    property var headerContextMenu: {

        function addTrack(){
            var objectPath = lk.layers.workspace.pluginsPath() + '/lcvcore/VideoTrackFactory.qml'
            var objectPathUrl = Fs.UrlInfo.urlFromLocalFile(objectPath)

            var objectComponent = Qt.createComponent(objectPathUrl);
            if ( objectComponent.status === Component.Error ){
                throw linkError(new Error(objectComponent.errorString()), timelineArea)
            }

            var object = objectComponent.createObject();

            var videoTrack = object.create()
            videoTrack.name = 'Video Track #' + (root.timeline.trackList.totalTracks() + 1)

            root.timeline.appendTrack(videoTrack)
        }

        var menuOptions = [{
            name: "Add Video Track",
            enabled: true,
            action: function(){
                if ( !root.timeline.properties.videoSurface ){

                    var objectPath = lk.layers.workspace.pluginsPath() + '/lcvcore/VideoSurfaceCreator.qml'
                    var objectPathUrl = Fs.UrlInfo.urlFromLocalFile(objectPath)

                    var objectComponent = Qt.createComponent(objectPathUrl);
                    if ( objectComponent.status === Component.Error ){
                        throw linkError(new Error(objectComponent.errorString()), this)
                    }

                    var object = objectComponent.createObject();
                    var overlay = lk.layers.window.dialogs.overlayBox(object)

                    object.surfaceCreated.connect(function(videoSurface){
                        root.timeline.properties.videoSurface = videoSurface
                        addTrack()
                        overlay.closeBox()
                        object.destroy()
                    })

                    object.cancelled.connect(function(){
                        overlay.closeBox()
                        object.destroy()
                    })
                } else {
                    addTrack()
                }
            }
        }, {
               name: "Add Video Surface...",
               enabled: true,
               action: function(){
                   var objectPath = lk.layers.workspace.pluginsPath() + '/lcvcore/VideoSurfaceCreator.qml'
                   var objectPathUrl = Fs.UrlInfo.urlFromLocalFile(objectPath)

                   var objectComponent = Qt.createComponent(objectPathUrl);
                   if ( objectComponent.status === Component.Error ){
                       throw linkError(new Error(objectComponent.errorString()), this)
                   }

                   var object = objectComponent.createObject();
                   var overlay = lk.layers.window.dialogs.overlayBox(object)

                   object.surfaceCreated.connect(function(videoSurface){
                       root.timeline.properties.videoSurface = videoSurface
                       overlay.closeBox()
                       object.destroy()
                   })

                   object.cancelled.connect(function(){
                       overlay.closeBox()
                       object.destroy()
                   })
               }
           },{
            name: "Add Keyframe Track",
            enabled: true,
            action: function(){

               var objectPath = lk.layers.workspace.pluginsPath() + '/timeline/KeyframeTrackFactory.qml'
               var objectPathUrl = Fs.UrlInfo.urlFromLocalFile(objectPath)

               var objectComponent = Qt.createComponent(objectPathUrl);
               if ( objectComponent.status === Component.Error ){
                   throw linkError(new Error(objectComponent.errorString()), timelineArea)
               }

               var object = objectComponent.createObject();

               var keyframeTrack = object.create()
               keyframeTrack.name = 'Keyframe Track #' + (root.timeline.trackList.totalTracks() + 1)

               root.timeline.appendTrack(keyframeTrack)
            }
        },{
            name: "Save",
            enabled: true,
            action: function(){
                root.timeline.save()
            }
        }, {
            name: "Save As...",
            enabled: true,
            action: function(){
               lk.layers.window.dialogs.saveFile({filters : "Json files (*.json)"}, function(path){
                   var localFile = Fs.UrlInfo.toLocalFile(path)
                   root.timeline.saveAs(localFile)
               })
            }
        }]

        return menuOptions
    }

    property var handleContextMenu: function(item){
        if ( item.objectName === 'timelineTrackTitle' ){
            var track = item.timelineArea.timeline.trackList.trackAt(item.trackIndex)
            var tr = track

            for ( var i = 0; i < trackTitleContextMenu.toClear.length; ++i ){
                trackTitleContextMenu.removeItem(trackTitleContextMenu.toClear[i])
            }

            for ( var i = 0; i < item.menuOptions.length; ++i ){
                var menuitem = trackTitleContextMenu.insertItem(i, item.menuOptions[i].name)
                menuitem.enabled = item.menuOptions[i].enabled
                menuitem.triggered.connect(item.menuOptions[i].action.bind(this, track))
                trackTitleContextMenu.toClear.push(menuitem)
            }
            trackTitleContextMenu.popup()

        } else if ( item.objectName === 'timelineOptions' ){

            var menuOptions = headerContextMenu

            for ( var i = 0; i < contextMenu.toClear.length; ++i ){
                contextMenu.removeItem(contextMenu.toClear[i])
            }

            for ( var i = 0; i < menuOptions.length; ++i ){
                var menuitem = contextMenu.insertItem(i, menuOptions[i].name)
                menuitem.enabled = menuOptions[i].enabled
                menuitem.triggered.connect(menuOptions[i].action)
                contextMenu.toClear.push(menuitem)
            }
            contextMenu.popup()
        }
    }

    property QtObject timelineStyle : TimelineStyle{}
    property Timeline timeline : Timeline{
        fps: 30
        contentLength: 100 * fps
        headerModel.scale: root.zoom
    }
    onTimelineChanged: {
        root.timeline.headerModel.scale = Qt.binding(function(){return root.zoom })
        segmentInsertMenu.segmentSelection = root.timeline.config.loaders()
        segmentInsertMenu.model = Object.keys(segmentInsertMenu.segmentSelection)

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
        timelineArea: root
        onAddSegment: {
            segmentInsertMenu.currentTrack = root.timeline.trackList.trackAt(index)
        }
        onRightClicked: root.trackTitleRightClicked(index, trackTitleItem)
    }

    property Item timelineOptions : Item{
        objectName: "timelineOptions"
        anchors.fill: parent
        property Item timelineArea: root
        Icons.MenuIcon{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            height: 10
            width: 10
            color: root.timelineStyle.iconColor
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    root.handleContextMenu(root.timelineOptions)
                }
            }
        }
        Item{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 30
            width: 15
            height: 15
            Icons.RecordIcon{
                anchors.centerIn: parent
                width: 12
                height: 12
                color: root.timelineStyle.iconColor
                visible: !root.timeline.isRunning
            }
            Icons.StopIcon{
                anchors.centerIn: parent
                width: 12
                height: 12
                padding: 2
                color: root.timelineStyle.iconColor
                visible: root.timeline.isRecording
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    if ( root.timeline.isRecording ){
                        root.timeline.stop()
                    } else {
                        lk.layers.window.dialogs.saveFile({filters: ["Avi file (*.avi)"]}, function(url){
                            var file = Fs.UrlInfo.toLocalFile(url)
                            var writerOptions = {
                                'filename': file,
                                'fourcc' : 'DIVX',
                                'fps' : root.timeline.fps
                            }
                            root.timeline.properties.videoSurface.setWriterOptions(writerOptions)
                            root.timeline.startRecording()
                        })

                    }
                }
            }
        }

        Workspace.PlayPause{
            width: 15
            height: 15
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 50
            isRunning: root.timeline.isRunning
            color: root.timelineStyle.iconColor
            visible: !root.timeline.isRecording
            onClicked : {
                if ( value )
                    root.timeline.start()
                else
                    root.timeline.stop()
            }
        }

        Workspace.Label{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 80
            text: root.timeline.positionToLabel(timelineArea.timeline.cursorPosition, false)
            textStyle: root.timelineStyle.timeLabelStyle
        }

        Menu{
            id: trackTitleContextMenu
            property var toClear: []
            MenuItem {
                text: qsTr("Remove Track")
                onTriggered: root.timeline.removeTrack(index)
            }
        }

        Menu{
            id: contextMenu
            property var toClear: []
        }
    }

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
                    if ( !interactive )
                        return

                    wheel.accepted = true

                    if ( timelineView.height > timelineView.contentHeight ){
                        return
                    }

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
        color: timelineStyle.rowBackground
        x: 150

        ScrollView{
            id: mainScroll
            anchors.fill: parent

            style: ScrollViewStyle {
                transientScrollBars: false
                handle: Item {
                    implicitWidth: 10
                    implicitHeight: 10
                    Rectangle {
                        color: "#1f2227"
                        anchors.fill: parent
                    }
                }
                scrollBarBackground: Item{
                    implicitWidth: 10
                    implicitHeight: 10
                    Rectangle{
                        anchors.fill: parent
                        color: 'transparent'
                    }
                }
                decrementControl: null
                incrementControl: null
                frame: Item{}
                corner: Rectangle{color: 'transparent'}
            }
            ListView{
                id: timelineView
                boundsBehavior: Flickable.StopAtBounds

                height: parent.height
                contentWidth: root.timeline.contentLength * root.zoom + 5
                model: root.timeline.trackList
                delegate: Rectangle{
                    objectName: "timelineRowDelegate"
                    height: 25
                    width: timelineRow.width
                    color: root.timelineStyle.borderColor

                    property int currentIndex: index
                    property ListView parentView: timelineView

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

                        onSegmentRightClicked: {
                            root.segmentRightClicked(track, segment, delegate)
                        }
                        onSegmentDoubleClicked: {
                            root.segmentDoubleClicked(track, segment, timelineRow)
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

    SegmentInsertMenu{
        id: segmentInsertMenu
        width: 100

        onSegmentSelected : {
            var objectPath = lk.layers.workspace.pluginsPath() + '/' + segmentInsertMenu.segmentSelection[segmentInsertMenu.model[index]]

            var objectComponent = Qt.createComponent(objectPath);
            if ( objectComponent.status === Component.Error ){
                throw linkError(new Error(objectComponent.errorString()), timelineArea)
            }

            var object = objectComponent.createObject();
            var overlay = lk.layers.window.dialogs.overlayBox(object)

            object.segmentCreated.connect(function(segment){
                var availableSpace = segmentInsertMenu.currentTrack.availableSpace(root.timeline.cursorPosition)
                if ( availableSpace === 0 ){
                    segment.destroy()
                    console.warn("Cannot add segment at cursor position. Not enough space.")
                    overlay.closeBox()
                    return
                }

                if ( availableSpace < segment.length ){
                    segment.length = availableSpace
                }

                segment.position = root.timeline.cursorPosition

                if ( segment.surface )
                    segment.surface = timelineArea.surface
                segmentInsertMenu.currentTrack.addSegment(segment)
                segmentInsertMenu.currentTrack = null
                overlay.closeBox()
            })

            object.cancelled.connect(function(){
                segmentInsertMenu.currentTrack = null
                overlay.closeBox()
            })
        }
    }
}

