import QtQuick 2.3
import lcvcore 1.0

Rectangle{
    
    // A simple video capture element
    
    property string videoPath : project.dir() + '/../_videos/amherst-11_2754_3754.avi'
    
    VideoCapture{
        id : videoArea
        loop : true
        fps : 100
        Component.onCompleted : staticOpen(parent.videoPath)
    }
    
}
