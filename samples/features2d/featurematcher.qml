import 'lcvcore' 1.0
import "lcvimgproc" 1.0
import "lcvfeatures2d" 1.0

Row{
    
    id : mainRow
    
    // Sample showing how to use 
    // Canny Edge Detector algorithm
    
    property string imagePath : codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
    
    Component{
        id : imReadfactory
        ImRead{}
    }
    
    
    Column{
        id : trainImages
        Component.onCompleted: {
            var rc = imReadfactory.createObject(trainImages);
            rc.file = imagePath
            var rc = imReadfactory.createObject(trainImages);
            rc.file = imagePath
            var rc = imReadfactory.createObject(trainImages);
            rc.file = imagePath
            var rc = imReadfactory.createObject(trainImages);
            rc.file = imagePath
        }
    }
    
}