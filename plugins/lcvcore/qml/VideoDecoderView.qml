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
    property alias stream: decoder.stream

    function seekTo(value){
        decoder.seekTo(value)
    }

    property string file: ''
    onFileChanged: {
        if ( file.length > 0 )
            decoder.run(file).forward([root, 'image'])
    }
}

