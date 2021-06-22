import QtQuick 2.3
import lcvcore 1.0

QtObject{

    property Component factory: VideoTrack{
    }

    function create(){
        var surface = factory.createObject()
        return surface
    }
}
