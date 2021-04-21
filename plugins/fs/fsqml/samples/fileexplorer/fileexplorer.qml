import QtQuick 2.3
import fs 1.0
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    id: grid
    columns: 2

    VisualFileSelector{
        id: visualFileSelector
        workingDirectory: project.dir() + '../../../../../samples/_images/'
    }
    
    ExtensionPass{
        id: extensionPass
        file: visualFileSelector.selectedFile
        extensions: ['jpg', 'png']
    }
    
    ImageRead{
        id: imageFile
        file: extensionPass.result
    }
    
    NavigableImageView{
        id: navigableImageView
        image: imageFile.result
        maxWidth: 300
        maxHeight: 200
    }
    Sepia{
        id: sepia
        image: imageFile.result
    }
    Temperature{
        id: temperature
        value: 1
        image: imageFile.result
    }
    Temperature{
        id: temperature2
        value: 0
        image: imageFile.result
    }
    NavigableImageView{
        id: navigableImageView2
        image: sepia.result
        maxWidth: 300
        maxHeight: 200
    }
    NavigableImageView{
        id: navigableImageView3
        image: temperature.result
        maxWidth: 300
        maxHeight: 200
    }
    NavigableImageView{
        id: navigableImageView4
        image: temperature2.result
        maxWidth: 300
        maxHeight: 200
    }


}
