import lcvcore 1.0
import lcvcontrols 1.0

Rectangle{
    
    // Video player with progress bar and pause button
    
    property string videoPath : codeDocument.path + '/../_videos/amherst-11_2754_3754.avi'
    
    VideoCapture{
        file : parent.videoPath
        id : videoArea
        loop : true
        fps : 100
    }
    
    VideoControls{
        videoCapture : videoArea
        
        anchors.left : videoArea.left
        anchors.top : videoArea.bottom
        height : 25
        width : videoArea.width
        color : "#192a3b"
    }
    
    
}
