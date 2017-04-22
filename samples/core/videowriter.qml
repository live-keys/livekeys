import QtQuick 2.3
import lcvcore 1.0
import live 1.0

Column{
    
    // This sample shows the usage of the VideoWriter component
    // Press the record button to start recording, then press pause and 
    // save to write the video to disk.
    
    property string videoPath : project.dir() + '/../_videos/amherst-11_2754_3754.avi'
    property string recordPath : project.dir() + '/../_videos/amherst-11_2754_3754_record.avi'
    
    VideoCapture{
        id : videoArea
        loop : true
        fps : 100
        Component.onCompleted : {
            staticOpen(parent.videoPath)
            paused = true
        }
    }
    
    Row{
        TextButton{
            text: videoArea.paused ? "Record" : "Pause"
            onClicked : videoArea.paused = !videoArea.paused
        }
        TextButton{
            text: "Save"
            onClicked : videoWriter.save();
        }
    }

    
    VideoWriter{
        id : videoWriter
        input: videoArea.output
        Component.onCompleted : {
            staticLoad({
                'filename': parent.recordPath,
                'fourcc': "XVID",
                'fps': videoArea.fps,
                'frameWidth' : 800,
                'frameHeight' : 600
            });
        }
    }
}
