import QtQuick 2.3
import QtQuick.Controls 1.2
import timeline 1.0
import workspace 1.0
import live 1.0
import lcvcore 1.0

Item{
    anchors.fill: parent
    
    VideoSurface{
        id: videoSurface
        width: 500
        height: 400
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
            contentLength: 100 * fps
            VideoTrack{
                name: "Track.1"
                surface: videoSurface
                
                VideoSegment{
                    position: 75 
                    length: 100
                    file : project.dir() + "/../../../samples/_videos/amherst-11_2754_3754.avi"
                }
    
                VideoSegment{
                    position: 50
                    length: 100
                    file : project.dir() + "/../../../samples/_videos/amherst-11_2754_3754.avi"
                }
            }
            NumberTrack{
                id: track2
                name: "Track.2"
                
                
                NumberAnimationSegment{
                    id: numberAnimationSegment
                    position: 200
                    length: 50
                    from: 0
                    to: 50
                    easing.type: Easing.InOutQuad;
                    easing.amplitude: 1.0; 
                    easing.period: 1.0 
                }
                
                NumberAnimationSegment{
                    id: numberAnimationSegment2
                    position: 300
                    length: 50
                    from: 50
                    to: 0
                    easing.type: Easing.InOutQuad;
                    easing.amplitude: 1.0; 
                    easing.period: 1.0 
                }
            }
        }
    }
}
