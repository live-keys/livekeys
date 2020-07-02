import QtQuick 2.3
import timeline 1.0
import lcvcore 1.0

Item{
    
    VideoSurface{
        id: videoSurface
        width: 500
        height: 400
    }
    
    Timeline{
        id: timeline
        fps: 30
        contentLength: 100 * fps
        Track{
            name: "Track.1"

            VideoSegment{
                position: 50
                length: 100
                file : project.dir() + "/../../../../samples/_videos/amherst-11_2754_3754.avi"
                surface : videoSurface
            }
            
            ImageSegment{
                position: 250
                length: 100
                file : project.dir() + "/../../../../samples/_images/buildings_0246.jpg"
                surface : videoSurface
            }
        }
        Track{
            name: "Track.2"
            
            VideoSegment{
                position: 75 
                length: 100
                file : project.dir() + "/../../../../samples/_videos/amherst-11_2754_3754.avi"
                surface : videoSurface
            }
        }
    }
    
}