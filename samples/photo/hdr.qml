import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Item{
    // This sample shows the usage of the hdr module.
    // Images are first aligned using AlignMTB, then calibrated and merged
    // using either Robertson or Debevec, then tonemapping is applied, which 
    // also converts the resulting image to 8 bit channels.
    
    height: loader.height
    
    Column{
        id: loader
        spacing: 5
        
        ImRead{
            file: project.path('../_images/coast_1.6.jpg')
            onOutputChanged: {loader.images[0] = output; inputList.initList();}
        }
        ImRead{
            file: project.path('../_images/coast_6.jpg')
            onOutputChanged: {loader.images[1] = output; inputList.initList();}
        }
        ImRead{
            file: project.path('../_images/coast_30.jpg')
            onOutputChanged: {loader.images[2] = output; inputList.initList();}
        }
        
        property var times:  [1.6, 6, 30]
        property var images: [0, 0, 0]
    }
    
    MatList{
        id: inputList
        function initList(){ fromArray(loader.images)}
        onEntriesAdded: if ( size() === 3 ) alignMTB.input = inputList
    }
    
    AlignMTB{
        id: alignMTB
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
