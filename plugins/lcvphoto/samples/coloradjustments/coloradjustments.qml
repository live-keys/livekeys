import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    id: grid

    ImageFile{
        id: imageFile
        source: project.dir() + '/../../../../samples/_images/object_101_piano_query.jpg'
    }

    Levels{
        id: levels
        input: imageFile.output.cloneMat()
    }

    HueSaturationLightness{
        id: hueSaturationLightness
        input: levels.result
        hue: 180
        saturation: 100
        lightness: 100
    }
    
    BrightnessAndContrast{
        id: brightnessAndContrast
        input: hueSaturationLightness.result
    }
    
    ImageView{
        image: brightnessAndContrast.result
    }

}
