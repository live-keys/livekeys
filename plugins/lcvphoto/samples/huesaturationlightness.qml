import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    
    // Using HueSaturationLightness and HueSaturationLightnessSliders
    
    columns: 2
    
    ImRead{
       id : src
       file : project.dir() + '/../../../samples/_images/object_101_piano_query.jpg'
    }
    
    HueSaturationLightness{
        id: hueSaturationLightness
        input: src.output.cloneMat()
        hue: hsls.hue
        saturation: hsls.saturation
        lightness: hsls.lightness
    }

    HueSaturationLightnessSliders{
        id: hsls
    }
    
    ImageView{
        image: hueSaturationLightness.result
    }

}

