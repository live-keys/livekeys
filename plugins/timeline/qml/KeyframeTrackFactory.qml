import QtQuick 2.3
import timeline 1.0

QtObject{

    property Component factory: KeyframeTrack{
    }

    function create(){
        var surface = factory.createObject()
        return surface
    }
}
