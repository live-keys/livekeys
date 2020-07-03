import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Item{
    
    ImageFile{
        id: imageFile
        visible: false
        source: project.path('../../../../samples/_images/coast_30.jpg')
    }
    
    TransformImage{
        id: transformImage
        input: imageFile.output
        
        Crop{ region: "0, 0, 150x200" }
        Resize{ size: "100x100" }
        Scale{ factor: "2.5x2" }
        Rotate{ degrees: 10 }
    }
    
    ImageView{
        id: imageView
        image: transformImage.result
    }
    
}