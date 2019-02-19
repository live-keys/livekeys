import QtQuick 2.3
import base 1.0
import live 1.0
import lcvcore 1.0
import lcvphoto 1.0 as Photo

Grid{
    
    // Usage of FastNlMeansDenoising
    
    columns: 2
    
    ImRead{
       id : src
       file : project.dir() + '/../_images/object_101_piano_query.jpg'
    }
    
    Act{
        id: act
        property Mat input: src.output.cloneMat()
        
        onTrigger: Photo.Denoising.fastNlMeans(act, input, 20, 7, 11)
        result: null
    }
    
    MatView{
        mat: act.result
    }
    
}

