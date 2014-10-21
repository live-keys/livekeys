import "lcvcore" 1.0
import "lcvimgproc" 1.0

Grid{
    
    // This sample shows the usage of the erode
    // and dilate elements
    
    columns: 2
    
    ImRead{
       id : src
       file : codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
    }
    
    StructuringElement{
        id : se
        shape : StructuringElement.MORPH_ELLIPSE
        ksize : "11x11"
    }
    
    Dilate{
        input : src.output
        kernel : se.output
        iterations : 5
    }
    
    
    Erode{
        input : src.output
        kernel : se.output
        iterations : 5
    }
}
