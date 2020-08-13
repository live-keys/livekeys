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
        image: videoSurface.image
        anchors.horizontalCenter: parent.horizontalCenter
    }
    
    Rectangle{
        x: track2.value
        width: 100
        height: 100
        anchors.bottom: parent.bottom
        color: '#1a3454'
    }
    
    TimelineView{
        id: timelineArea
        anchors.top: parent.top
        anchors.topMargin: 400
        width : parent.width
        height: 200 
        surface: videoSurface
        focus : true
        timeline: Timeline{
            fps: 30
            contentLength: fps * 200
            properties: QtObject{
                property VideoSurface videoSurface: VideoSurface{
                    id: videoSurface
                    imageWidth: 800
                    imageHeight: 600
                }
            }
            VideoTrack{
                name: "Track.1"
                
                VideoSegment{
                    position: 75 
                    length: 100 
                    file : project.dir() + "/../../../samples/_videos/amherst-11_2754_3754.avi"
                }
                
                ImageSegment{
                    position: 250
                    length: 100
                    file : project.dir() + "/../../../samples/_images/buildings_0246.jpg"
                }
            }
            KeyframeTrack{
                id: track2
                name: "Track.2"
                
                Keyframe{
                    position: 100
                    value: 50
                    easing.type: Easing.InOutQuad;
                    easing.amplitude: 1.0; 
                    easing.period: 1.0 
                }
                Keyframe{
                    position: 150
                    value: 150
                    easing.type: Easing.InOutQuad;
                    easing.amplitude: 1.0; 
                    easing.period: 1.0 
                }
            }
        }
    }
}
