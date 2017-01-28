import QtQuick 2.3
import lcvcore 1.0
import lcvvideo 1.0

Rectangle{
    
    property string videoPath : project.dir() + '/../_videos/amherst-11_2754_3754.avi'
    
    VideoCapture{
        file : parent.videoPath
        id : videoArea
        loop : true
        fps : 30
        visible : false
    }
    
    CalcOpticalFlowPyrLK{
        input : videoArea.output
        MouseArea{
            anchors.fill : parent
            onClicked : {
                parent.addPoint(Qt.point(mouse.x, mouse.y))
            }
        }
    }
    
    
}
