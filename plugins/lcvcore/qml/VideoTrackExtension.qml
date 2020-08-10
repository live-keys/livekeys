import QtQuick 2.3

QtObject{

    property var menu : [
        {
            name : "Add Image...",
            action : function(track){
                var timeline = track.timeline()

                var objectPath = lk.layers.workspace.pluginsPath() + '/lcvcore/ImageSegmentCreator.qml'

                var objectComponent = Qt.createComponent(objectPath);
                if ( objectComponent.status === Component.Error ){
                    throw linkError(new Error(objectComponent.errorString()), this)
                }

                var object = objectComponent.createObject();
                var overlay = lk.layers.window.dialogs.overlayBox(object)

                var currentTrack = track

                object.segmentCreated.connect(function(segment){
                    var availableSpace = currentTrack.availableSpace(timeline.cursorPosition)
                    if ( availableSpace === 0 ){
                        segment.destroy()
                        console.warn("Cannot add segment at cursor position. Not enough space.")
                        overlay.closeBox()
                        return
                    }

                    if ( availableSpace < segment.length ){
                        segment.length = avialableSpace
                    }

                    segment.position = timeline.cursorPosition

                    if ( segment.surface )
                        segment.surface = timeline.properties.videoSurface
                    currentTrack.addSegment(segment)
                    overlay.closeBox()
                })

                object.cancelled.connect(function(){
                    overlay.closeBox()
                })

            },
            enabled : true
        }, {
            name : "Add Video...",
            action : function(track){
                var timeline = track.timeline()
                var objectPath = lk.layers.workspace.pluginsPath() + '/lcvcore/VideoCaptureSegmentCreator.qml'

                var objectComponent = Qt.createComponent(objectPath);
                if ( objectComponent.status === Component.Error ){
                    throw linkError(new Error(objectComponent.errorString()), this)
                }

                var object = objectComponent.createObject();
                var overlay = lk.layers.window.dialogs.overlayBox(object)

                var currentTrack = track

                object.segmentCreated.connect(function(segment){
                    var availableSpace = currentTrack.availableSpace(timeline.cursorPosition)
                    if ( availableSpace === 0 ){
                        segment.destroy()
                        console.warn("Cannot add segment at cursor position. Not enough space.")
                        overlay.closeBox()
                        return
                    }

                    if ( availableSpace < segment.length ){
                        segment.length = avialableSpace
                    }

                    segment.position = timeline.cursorPosition

                    if ( segment.surface )
                        segment.surface = timeline.properties.videoSurface
                    currentTrack.addSegment(segment)
                    overlay.closeBox()
                })

                object.cancelled.connect(function(){
                    overlay.closeBox()
                })
            },
            enabled : true
        }
    ]

}
