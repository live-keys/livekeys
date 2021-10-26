import QtQuick 2.3
import QtQuick.Controls 1.2
import lcvcore 1.0
import lcvimgproc 1.0
import lcvvideo 1.0

ScrollView{
    anchors.fill : parent
    Row{

        property string videoPath : project.dir() + '/../../../samples/_videos/amherst-11_2754_3754.avi'
        
        VideoCapture{
            id : videoArea
            loop : true
            fps : 30
            visible : false
            Component.onCompleted : staticLoad(parent.videoPath)
        }
        
        ImageView {
            id: background
        }
        
        BackgroundSubtractorMog2{
            id : foregroundMask
            input : videoArea.output
            varThreshold : 100
            learningRate : 0.005
            detectShadows : false
            onOutputChanged : {
               background.image = this.backgroundModel
            }
            Component.onCompleted : staticLoad("mog2")
        }

    }
}

