import QtQuick 2.3
import base 1.0
import lcvcore 1.0
import lcvphoto 1.0 as Photo

Act{
    property Mat input: null
    property int hue: 100
    property int saturation: 100
    property int lightness: 100

    returns: "qml/object"
    run: [Photo.Adjustments, 'hueSaturationLightness']
    args: ["$input", "$hue", "$saturation", "$lightness"]
}
