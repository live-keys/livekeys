import "lcvcore" 1.0
import "lcvimgproc" 1.0

Grid{
    
    // Usage of the filter2D
    
    columns: 2
    
    ImRead{
       id : src
       file : codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
    }
    
    Mat2DArray{
        id : kernel
        type : Mat.CV32F
        width : 200
        height : 200
        linearFilter : false
        values : [
            [-0.1, 0.0, 0,1], 
            [-0.1, 0.0, 0.1], 
            [-0,1, 0.0, 0.1]]
    }
    
    Filter2D{
        input : src.output
        kernel : kernel.output
        borderType : CopyMakeBorder.BORDER_DEFAULT
    }
}
