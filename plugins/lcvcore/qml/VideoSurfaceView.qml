import QtQuick 2.3
import lcvcore 1.0
import timeline 1.0

ImageView{

    property Timeline timeline: null

    image: {
        if ( timeline && timeline.properties && timeline.properties.videoSurface ){
            return timeline.properties.videoSurface.image
        }
        return null
    }

}
