import QtQuick 2.3
import base 1.0
import lcvcore 1.0
import lcvphoto 1.0 as Photo

Act{
    property Mat input: null
    property var channels: undefined
    property var lightness: undefined

    run: [Photo.Adjustments, 'levels']
    args: ["$input", "$lightness", "$channels"]
}
