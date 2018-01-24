import QtQuick 2.3
import lcvcore 1.0

Rectangle{

    // Video player with progress bar and pause button

    property string videoPath : project.dir() + '/../_videos/amherst-11_2754_3754.avi'

    VideoCapture{
        id : videoArea
        loop : true
        fps : 100
        Component.onCompleted : staticOpen(parent.videoPath)
    }

    VideoControls{
        videoCapture : videoArea
        anchors.left : videoArea.left
        anchors.top : videoArea.bottom
        width : videoArea.width
    }

}
