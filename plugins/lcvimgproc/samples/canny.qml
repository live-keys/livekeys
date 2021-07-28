import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0


Row{
    
    // Sample showing how to use 
    // Canny Edge Detector algorithm
        
    ImageRead{
        id : imgSource
        file: project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }
    
    Canny{
        id: canny
        input: imgSource.result
        threshold1 : 72
        threshold2 : 222
    }
    
    ImageView {
        image: canny.result
    }
    
}
