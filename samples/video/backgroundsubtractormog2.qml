import QtQuick.Controls 1.2
import lcvcore 1.0
import lcvimgproc 1.0
import lcvvideo 1.0

ScrollView{
    anchors.fill : parent
    Row{

        property string videoPath : codeDocument.path + '/../_videos/amherst-11_2754_3754.avi'
        
        VideoCapture{
            file : parent.videoPath
            id : videoArea
            loop : true
            fps : 30
            visible : false
        }
        
        MatView {
            id: background
        }
        
        BackgroundSubtractorMog2{
            id : foregroundMask
            input : videoArea.output
            varThreshold : 100
            learningRate : 0.005
            detectShadows : false
            onOutputChanged : {
               background.mat = this.backgroundModel
            }
        }

    }
}

