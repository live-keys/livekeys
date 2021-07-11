import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Grid{
    
    // This sample shows the usage of the erode
    // and dilate elements
    
    columns: 1
    
    ImageRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }
    
    StructuringElement{
        id : se
        size : "11x11"
    }
    
    Dilate{
        id: dilate
        input : src.result
        kernel : se.result
        iterations : 5
    }
    
    
    Erode{
        id: erode
        input : src.result
        kernel : se.result
        iterations : 5
    }
    
    ImageView {
        image: dilate.result
    }
    
    ImageView {
        image: erode.result
    }
}
