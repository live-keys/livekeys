import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Grid{
    // Calculate movement in a video.
    
    columns : 1    

    VideoCapture{
        id : videoArea
        loop : true
        Component.onCompleted : {
            staticOpen(project.dir() + '/../../../samples/_videos/amherst-11_2754_3754.avi')
            width = width / 2
            height = height / 2
        }
    }
    
    ChannelSelect{
        id : cs
        input : videoArea.output
        channel: 0
    }
    
    MatBuffer{
        id : mb
        input : cs.result
        width : videoArea.width
        height : videoArea.height
    }
    
    AbsDiff{
        input : cs.result
        input2 : mb.output
        width : videoArea.width
        height : videoArea.height
    }
    
}
