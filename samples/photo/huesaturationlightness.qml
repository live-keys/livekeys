import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    
    // Usage of HueSaturationLightness
    
    columns: 2
    
    ImRead{
       id : src
       file : project.dir() + '/../_images/object_101_piano_query.jpg'
    }
    
    HueSaturationLightness{
        input: src.output
        hue: 50
        saturation: 120
        lightness: 80
    }
}

