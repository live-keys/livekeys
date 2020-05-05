import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Grid{
    
    // This sample shows the usage of the erode
    // and dilate elements
    
    columns: 2
    
    ImRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
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
