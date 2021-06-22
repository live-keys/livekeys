import QtQuick 2.3
import lcvcore 1.0
import timeline 1.0

QtObject{

    property string file: ''
    property int position: 0
    property int length: 0

    property Component factory: ImageSegment{}

    property QtObject timeline: null
    property string trackName: ''

    property var track: null

    function create(){
        if ( !file ){
            return
        }

        var segment = factory.createObject()
        segment.file = file
        segment.length = 10
        if ( length > 0 )
            segment.length = length
        if ( position > 0 )
            segment.position = position

        var _track = null
        var _timeline = timeline ? timeline : track.timeline()
        if ( !_track ){
            var tlist = _timeline.trackList
            for ( var i = 0; i < tlist.totalTracks(); ++i ){
                var t = tlist.trackAt(i)
                if ( t.name === trackName ){
                    _track = t
                    break
                }
            }
        }

        if ( _track ){
            if ( _timeline && position === 0 ){
                segment.position = _timeline.cursorPosition
            }

            _track.addSegment(segment)
            if ( _timeline.cursorPosition === segment.position )
                _timeline.cursorPosition += segment.length
        }

        newSegment(segment)

        return segment
    }

    signal newSegment(ImageSegment segment)

}
