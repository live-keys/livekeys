import QtQuick 2.3
import QtQuick.Controls 1.2
import timeline 1.0
import workspace 1.0
import live 1.0
import lcvcore 1.0

Item{
    anchors.fill: parent
    
    ImageView{
        width: 600
        height: 400
        image: {
            if ( timeline.properties ){
                return timeline.properties.videoSurface.image
            }
            return null   
        }
        anchors.horizontalCenter: parent.horizontalCenter
    }
    
    Rectangle{
        x: keyframeValue.value
        width: 100
        height: 100
        anchors.bottom: parent.bottom
        color: '#1a3454'
    }
    
    KeyframeValue{
        id: keyframeValue
        track: 'Track.2'
        timeline: timeline
    }
    
    Timeline{
        id: timeline
        fps: 30
        contentLength: fps * 200
        file: '/Users/dinu/Desktop/asd.json'
    }
}
