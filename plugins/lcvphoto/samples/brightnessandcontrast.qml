import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    
    // Usage of BrightnessAndContrast
    
    columns: 1
    
    ImRead{
       id : src
       file : project.dir() + '/../_images/buildings_0246.jpg'
    }
    
    BrightnessAndContrast{
        input : src.output
        brightness: -19
        contrast: 1.53
    }
    
}

