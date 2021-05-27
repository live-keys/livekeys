import QtQuick 2.3
import base 1.0
import lcvcore 1.0
import lcvimgproc 1.0 as Img

Act{
    id: act
    property int left: 0
    property int top: 0
    property int right: 0
    property int bottom: 0
    property color color: "#00000000"
    property Mat input: null

    returns: "qml/object"
    run: Img.Geometry.pad
    args: ["$input", "$color", "$top", "$right", "$bottom", "$left"]
}
