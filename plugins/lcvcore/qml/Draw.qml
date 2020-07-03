import QtQuick 2.0
import lcvcore 1.0
import base 1.0

Act{
    property Mat input: null
    onInputChanged: surface = input
    property Mat surface: null
    run: function(surface){
        return surface
    }
    args: ["$surface"]
}
