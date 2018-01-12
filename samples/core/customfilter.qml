import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Row{
    
    // This sample shows how to iterate or 
    // access values from an image directly
    // from the qml file and create a custom filter.
    
    ImRead{
       id : src
       file : project.dir() + '/../_images/buildings_0246.jpg'
    }
    
    ChannelSelect{
        input : src.output
        width : src.width
        height : src.height
        
        onOutputChanged : {
            var dim   = output.dimensions()
            var buffer = output.buffer()
            var uview = new Uint8Array(buffer)

            for ( var i = 0; i < dim.height; ++i ){
                for ( var j = 0; j < dim.width; ++j ){
                    var val = uview[i * dim.width + j]
                    if ( val < 100 )
                        uview[i * dim.width + j] = 0
                }
            }
        }
    }
    
}
