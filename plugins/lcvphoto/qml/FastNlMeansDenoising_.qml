import lcvcore 1.0
import lcvphoto 1.0 as Photo
import base 1.0
import live 1.0

Act{
    id: act
    property Mat input: null
    property real h: 3
    property int templateWindowSize: 21
    property int searchWindowSize: 21

    run: [Photo.Denoising, 'fastNlMeans']
    args: ["$input", "$h", "$templateWindowSize", "$searchWindowSize"]
}
