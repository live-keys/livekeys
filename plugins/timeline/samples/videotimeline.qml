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
            Track{
                name: "Track.1"
    
                VideoSegment{
                    position: 50
                    length: 100
                    file : project.dir() + "/../../../samples/_videos/amherst-11_2754_3754.avi"
                    surface : timelineArea.surface
                }
                
                ImageSegment{
                    position: 250
                    length: 100
                    file : project.dir() + "/../../../samples/_images/buildings_0246.jpg"
                    surface : timelineArea.surface
                }
            }
            Track{
                name: "Track.2"
                
                VideoSegment{
                    position: 75 
                    length: 100
                    file : project.dir() + "/../../../samples/_videos/amherst-11_2754_3754.avi"
                    surface : timelineArea.surface
                }
            }
        }
    }
}
