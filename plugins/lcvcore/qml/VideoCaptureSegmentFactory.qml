import QtQuick 2.3
import lcvcore 1.0
import timeline 1.0

QtObject{

    property string file: ''
    property int position : 0
    property int length : 0

    property Component factory: VideoSegment{
    }

    property Track track: null

    function create(){
        var segment = factory.createObject()
        segment.file = file
        if ( length > 0 )
            segment.length = length
        if ( position > 0 )
            segment.position = position

        if ( track ){
            var timeline = track.timeline()
            if ( timeline && position === 0 ){
                segment.position = timeline.cursorPosition
            }

            track.addSegment(segment)
        }

        newSegment(segment)

        return segment
    }

    signal newSegment(VideoSegment segment)
}
