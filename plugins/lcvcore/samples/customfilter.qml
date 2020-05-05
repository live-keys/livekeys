import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Row{
    
    // This sample shows how to iterate or 
    // access values from an image directly
    // from the qml file and create a custom filter.
    
    ImRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }
    
    ChannelSelect{
        input : src.output
        width : src.width
        height : src.height
        
        onOutputChanged : {
            // Versions of Qt >= 5.8 have raw buffer processing
            // which is a lot faster than the current method
            // If you're using a higher version, you can replace the 
            // 5.7 lines with 5.8
            
            var dim   = output.dimensions()
            var uview = cv.matToArray(output) // 5.7
            //var buffer = output.buffer() // 5.8
            //var uview = new Uint8Array(buffer) // 5.8

            for ( var i = 0; i < dim.height; ++i ){
                for ( var j = 0; j < dim.width; ++j ){
                    var val = uview[i * dim.width + j]
                    if ( val < 100 )
                        uview[i * dim.width + j] = 0
                }
            }
            
            cv.assignArrayToMat(uview, output) // 5.7
        }
    }
    
}
