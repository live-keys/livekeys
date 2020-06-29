import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    
    // Usage of BrightnessAndContrast
    
    columns: 1
    
    ImRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }
    
    BrightnessAndContrast{
        id: brightnessAndContrast
        input : src.output.cloneMat()
        brightness: -19
        contrast: 1.53
    }
    
    ImageView{
        image: brightnessAndContrast.result
    }
    
}

