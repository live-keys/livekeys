import QtQuick 2.3
import timeline 1.0
import lcvcore 1.0

QtObject{

    property Component factory: VideoSurface{
    }

    function create(){
        var surface = factory.createObject()
        return surface
    }
}
