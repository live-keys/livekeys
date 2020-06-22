import QtQuick 2.3
import base 1.0
import lcvcore 1.0
import lcvphoto 1.0 as Photo

Act{
    property Mat input: null
    property double brightness: 0.0
    property double contrast: 1.0

    run: [Photo.Adjustments, 'brightnessAndContrast']
    args: ["$input", "$brightness", "$contrast"]
}
