import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    
    // Usage of FastNlMeansDenoising
    
    columns: 2
    
    ImRead{
       id : src
       file : project.dir() + '/../_images/object_101_piano_query.jpg'
    }
    
    FastNlMeansDenoising{
        id: fastNlMeansDenoising
        visible: false
        input : src.output
        h : 20.0
        searchWindowSize: 11
    }
    
    MatView{
        mat: fastNlMeansDenoising.output
    }
    
}

