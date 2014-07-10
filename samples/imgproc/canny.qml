import 'lcvcore' 1.0
import "lcvimgproc" 1.0

// Sample showing how to use 
// Canny Edge Detector algorithm

Row{
    
    property string imagePath : codeDocument.path + '/../_images/caltech_buildings_DSCN0246.JPG'
    
    ImRead{
        id : imgSource
        file : parent.imagePath
        width : 400
        height : 300
    }
    
    Canny{
        input: imgSource.output
        width : 800
        height : 600
        threshold1 : 70
        threshold2 : 210
    }
    
    
    
}