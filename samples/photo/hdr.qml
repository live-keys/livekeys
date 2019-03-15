import QtQuick 2.3
import lcvcore 1.0 as Cv
import lcvphoto 1.0
import base 1.0

Item{
    // This sample shows the usage of the hdr module.
    // Images are first aligned using AlignMTB, then calibrated and merged
    // using either Robertson or Debevec, then tonemapping is applied, which 
    // also converts the resulting image to 8 bit channels.
    
    height: parent.height
    width: parent.width
    
    Column{
        Repeater{
            id: repeater
            model: ['coast_1.6.jpg', 'coast_6.jpg', 'coast_30.jpg'].map(
                function(e){ return project.path('../_images/' + e);}).map(
                function(e){ return Cv.MatIO.read(e); }
            )
            property var times:  [1.6, 6, 30]
            
            Cv.MatView{
                mat : modelData
            }
        }
    }         
    
    AlignMTB{
        id: alignMTB
        input: Cv.MatOp.createMatList(repeater.model)
    }
    
    CalibrateRobertson{ 
        id: calibrateRobertson
        input: alignMTB.output
        times: repeater.times
    }
    
    MergeRobertson{
        id: mergeRobertson
        input: alignMTB.output
        times: repeater.times
        response: calibrateRobertson.output
    }
    
    TonemapDrago{
        anchors.right: parent.right
        anchors.top: parent.top
        input: mergeRobertson.output
        params: {
            'gamma': 0.9,
            'saturation': 0.8
        }
    }
    
}
