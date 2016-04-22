import lcvcore 1.0
import lcvimgproc 1.0


Row{
    
    // Sample showing how to use 
    // Canny Edge Detector algorithm
    
    property string imagePath : codeDocument.path + '/../_images/buildings_0246.jpg'
    
    ImRead{
        id : imgSource
        file : parent.imagePath
    }
    
    Canny{
        input: imgSource.output
        threshold1 : 70
        threshold2 : 222
    }
    
}
