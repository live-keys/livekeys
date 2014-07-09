import "lcvcore" 1.0
import "lcvimgproc" 1.0

Row{
    
    ImRead{
       id : src
       file : todo
       Component.onCompleted : {
           width = width / 4
           height = height / 4
       }
    }
    
    ChannelSelect{
        input : src.output
        width : src.width
        height : src.height
        onInputChanged : {
            var data = input.data()
            var rows = 1800
            var cols = 1326
            var channels = data.channels()
            var it   = 0;
            
            var vals = data.values()
            
            for ( var i = 0; i < rows; ++i ){
                for ( var j = 0; j < cols; ++j ){
                    if ( vals[i][j * channels] > 100 ){
                        vals[i][j * channels + 0] = 200
                        vals[i][j * channels + 1] = 200
                        vals[i][j * channels + 2] = 200
                    }
                }
            }
            
            
            data.setValues(vals)
            
            console.log("It : " + it);
            console.log(data.ellapsed())
        }
    }
    
}
