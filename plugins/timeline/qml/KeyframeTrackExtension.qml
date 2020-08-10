import QtQuick 2.3
import timeline 1.0

QtObject{

    property Component keyframeFactory: Keyframe{}

    property var menu : [
        {
            name : "Add Keyframe",
            action : function(track){
                var currentTrack = track
                var timeline = track.timeline()
                var availableSpace = currentTrack.availableSpace(timeline.cursorPosition)
                if ( availableSpace === 0 ){
                    console.warn("Cannot add keyframe at cursor position. Keyframe already present there.")
                    return
                }

                var kf = keyframeFactory.createObject()
                kf.position = timeline.cursorPosition
                currentTrack.addSegment(kf)
            },
            enabled: true
        }
    ]
}
