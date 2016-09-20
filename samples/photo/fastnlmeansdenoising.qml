import lcvcore 1.0
import lcvphoto 1.0

Grid{
    
    // Usage of FastNlMeansDenoising
    
    columns: 2
    
    ImRead{
       id : src
       file : codeDocument.path + '/../_images/object_101_piano_query.jpg'
    }
    
    FastNlMeansDenoising{
        input : src.output
        h : 10.0
    }
    
}

