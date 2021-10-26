import QtQuick 2.3
import lcvcore 1.0 as Cv
import lcvphoto 1.0
import base 1.0

Item{
    // This sample shows the usage of the hdr module.
    // Images are first aligned using AlignMTB, then calibrated and merged
    // using either Robertson or Debevec, then tonemapping is applied, which 
    // also converts the resulting image to 8 bit channels.
    
    
    
    Row{
        id: row1
        Repeater{
            id: repeater
            model: ['coast_1.6.jpg', 'coast_6.jpg', 'coast_30.jpg'].map(
                function(e){ return project.path('../../../samples/_images/' + e);}).map(
                function(e){ return Cv.MatIO.read(e); }
            )
            property var times:  [1.6, 6, 30]
            
            Cv.ImageView{
                image : modelData
            }
        }
    }         
    
    AlignMTB{
        id: alignMTB
        input: Cv.MatOp.createMatList(repeater.model)
    }

    CalibrateRobertson {
        id: cd
        input: alignMTB.result
        times: repeater.times
    }
    
    MergeRobertson {
        id: md
        input: alignMTB.result
        times: repeater.times
        response: cd.result
    }
    

    TonemapMantiuk {
        id: tonemap
        input: md.result
    }
    
    Cv.ImageView {
        anchors.top: row1.bottom
        image: Cv.MatOp.convertTo8U(tonemap.result)
    }
    
    
}
