import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Item{
    width: imageView.width
    height: imageView.height
            
    ImageFile{
        id: imageFile
        source: modelData
    }

    Levels{
        id: levels
        input: imageFile.output
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
        id: imageView
        image: brightnessAndContrast.result
    }

}