import QtQuick 2.3
import lcvcore 1.0 as Cv
import lcvphoto 1.0
import base 1.0

Item{
    // This sample shows the usage of the hdr module.
    // Images are first aligned using AlignMTB, then calibrated and merged
    // using either Robertson or Debevec, then tonemapping is applied, which 
    // also converts the resulting image to 8 bit channels.
    
    height: loader.height
    
    Column{
        id: loader
        spacing: 5
        
        Cv.ImRead{
            file: project.path('../_images/coast_1.6.jpg')
            onOutputChanged: {loader.images[0] = output; initInputList();}
        }
        Cv.ImRead{
            file: project.path('../_images/coast_6.jpg')
            onOutputChanged: {loader.images[1] = output; initInputList();}
        }
        Cv.ImRead{
            file: project.path('../_images/coast_30.jpg')
            onOutputChanged: {loader.images[2] = output; initInputList();}
        }
        
        property var times:  [1.6, 6, 30]
        property var images: [0, 0, 0]
    }
    

    function initInputList(){
        if ( loader.images[0] !== 0 &&  loader.images[1] !== 0 && loader.images[2] !== 0) {
            alignMTB.input = Cv.MatOp.createMatList(loader.images)
        }
    }
    
    AlignMTB{
        id: alignMTB
        input: null
        output: Cv.MatOp.createMatList()
    }
    
    CalibrateRobertson{ 
        id: calibrateRobertson
        input: alignMTB.output
        times: loader.times
    }
    
    MergeRobertson{
        id: mergeRobertson
        input: alignMTB.output
        times: loader.times
        response: calibrateRobertson.output
    }
    
    TonemapDurand{
        anchors.left: loader.right
        anchors.leftMargin: 15
        anchors.verticalCenter: parent.verticalCenter
        input: mergeRobertson.output
        params: {
            'gamma': 0.9,
            'contrast': 2.6,
            'saturation': 0.8
        }
    }
    
}
