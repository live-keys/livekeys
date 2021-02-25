import QtQuick 2.3
import base 1.0
import lcvcore 1.0

ImageView{
    id: root

    VideoDecoder{
        id: decoder
        loop: true
    }

    property alias fps: decoder.fps
    property alias paused: decoder.paused
    property alias currentFrame: decoder.currentFrame
    property alias totalFrames: decoder.totalFrames

    function seekTo(value){
        decoder.seekTo(value)
    }

    property string source: ''
    onSourceChanged: {
        if ( source.length > 0 )
            decoder.run(source).forward([root, 'image'])
    }
}

