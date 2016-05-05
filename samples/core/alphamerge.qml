import lcvcore 1.0

Grid{
    
    // This sample shows how to use a mask to create
    // an alpha channel for an image.
    
    columns : 2
    
    ImRead{
       id : src
       file : codeDocument.path + '/../_images/buildings_0246.jpg'
    }
    
    MatEmpty{
        id : nullmask
        matSize : src.output.dataSize()
    }
    
    MatDraw{
        id : mask
        input : nullmask.output
        onInputChanged : {
            circle( Qt.point(
                input.dataSize().width / 2, 
                input.dataSize().height / 2),
                input.dataSize().height / 4, 
                "#fff", -1);
        }
    }
    
    AlphaMerge{
        input : src.output
        mask : mask.output
    }
    
}
