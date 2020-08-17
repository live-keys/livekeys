import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0
import lcvphoto 1.0

Grid{
    id: grid

    ImageFile{
        id: imageFile
        source: project.dir() + '/../../../../samples/_images/object_101_piano_query.jpg'
    }

    TransformImage{
        id: transformImage
        input: imageFile.output
    }
    
    ImageView{
        image: transformImage.result
    }

}
