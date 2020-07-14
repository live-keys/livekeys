import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    id: grid

    VideoDecoderView{
        id: videoDecoderView
        visible: false
        file: project.dir() + '/../../../../samples/_videos/amherst-11_2754_3754.avi'
    }

    Levels{
        id: levels
        input: videoDecoderView.image
    }

//    HueSaturationLightness{
//        id: hueSaturationLightness
//        input: levels.result
//        hue: 180
//        saturation: 100
//        lightness: 100
//    }

//    BrightnessAndContrast{
//        id: brightnessAndContrast
//        input: hueSaturationLightness.result
//    }

    ImageView{
        image: levels.result
    }

}
