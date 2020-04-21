import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    
    // Using HueSaturationLightness and HueSaturationLightnessSliders
    
    columns: 2
    
    ImRead{
       id : src
       file : project.dir() + '/../_images/object_101_piano_query.jpg'
    }
    
    HueSaturationLightness{
        input: src.output
        hue: hsls.hue
        saturation: hsls.saturation
        lightness: hsls.lightness
    }

    HueSaturationLightnessSliders{
        id: hsls
        hue: 50
        saturation: 130
        lightness: 80
    }

}

