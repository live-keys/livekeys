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
        VideoTrack{
            name: "Track.1"
            surface: videoSurface

            VideoSegment{
                position: 50
                length: 100
                file : project.dir() + "/../../../../samples/_videos/amherst-11_2754_3754.avi"
            }
            
            ImageSegment{
                position: 250
                length: 100
                file : project.dir() + "/../../../../samples/_images/buildings_0246.jpg"
            }
        }
        VideoTrack{
            name: "Track.2"
            surface: videoSurface
            
            VideoSegment{
                
                position: 75 
                length: 100
                file : project.dir() + "/../../../../samples/_videos/amherst-11_2754_3754.avi"
            }
        }
    }
    
}