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
       file : project.dir() + '/../../../samples/_images/object_101_piano_query.jpg'
    }
    
    Worker{
        id: worker
        
        Act{
            id: act
            
            property Mat input: src.output.cloneMat()
            
            run: [Photo.Denoising, 'fastNlMeans']
            args: ["$input", 21, 3, 21]
        }
    }
    
    MatView{
        mat: act.result ? act.result : null
    }
    
}

