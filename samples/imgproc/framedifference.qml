import lcvcore 1.0
import lcvimgproc 1.0

Grid{
    // Calculate movement in a video.
    
    property string videoPath : codeDocument.path + '/../_videos/amherst-11_2754_3754.avi'    

    columns : 2    

    VideoCapture{
        file : parent.videoPath
        id : videoArea
        loop : true
        Component.onCompleted : {
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
