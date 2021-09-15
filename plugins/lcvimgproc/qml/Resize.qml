import QtQuick 2.3
import base 1.0
import lcvcore 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property size size: "0x0"
    property int interpolation: Img.Geometry.INTER_LINEAR
    property double maxWidth: 0
    property double maxHeight: 0

    returns: "qml/object"
    run: function(input, size, maxWidth, maxHeight, interpolation){
        if ( maxWidth > 0 || maxHeight > 0 ){
            var imageSize = input.dimensions()
            if ( imageSize.width === 0 || imageSize.height === 0 )
                return input

            var scale = 0
            if ( maxWidth > 0 ){
                var wScale = maxWidth / imageSize.width
                scale = wScale
            }
            if ( maxHeight > 0 ){
                var hScale = maxHeight / imageSize.height
                scale = scale === 0
                        ? hScale
                        : scale < hScale ? scale : hScale
            }
            return Img.Geometry.scale(input, scale, scale, interpolation)
        } else {
            return Img.Geometry.resize(input, size, interpolation)
        }
    }
    args: ["$input" , "$size", "$maxWidth", "$maxHeight", "$interpolation"]
}
