import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Grid{
    // Calculate movement in a video.
    
    property string videoPath : project.dir() + '/../_videos/amherst-11_2754_3754.avi'    

    columns : 2    

    VideoCapture{
        id : videoArea
        loop : true
        Component.onCompleted : {
            staticOpen(parent.videoPath)
            width = width / 2
            height = height / 2
        }
    }
    
    ChannelSelect{
        id : cs
        input : videoArea.output
        width : videoArea.width
        height : videoArea.height
    }
    
    MatBuffer{
        id : mb
        input : cs.output
        width : videoArea.width
        height : videoArea.height
    }
    
    AbsDiff{
        input : cs.output
        input2 : mb.output
        width : videoArea.width
        height : videoArea.height
    }
    
}
