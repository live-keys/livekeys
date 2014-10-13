import "lcvcore" 1.0
import "lcvimgproc" 1.0

Row{
    
    // This sample shows the usage of the Blur
    // element
    
    ImRead{
       id : src
       file : codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
    }
    
    Blur{
        input : src.output
        anchor : Qt.point(3, 3)
        ksize : "5x5"
    }
}
