import 'lcvcore' 1.0
import "lcvimgproc" 1.0
import "lcvfeatures2d" 1.0

Row{
    
    // Sample showing how to use 
    // Canny Edge Detector algorithm
    
    property string imagePath : codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
    
    ImRead{
        id : imgSource
        file : parent.imagePath
    }
    
    FastFeatureDetector{
    }
    
}