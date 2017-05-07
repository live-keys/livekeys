import QtQuick 2.3
import live 1.0
import lcvcore 1.0

Column{
    spacing: 10
    
    // This sample shows how to use the ImWrite component
    // to write an image to a file
    
    ImRead{
       id : src
       file : project.dir() + '/../_images/buildings_0246.jpg'
    }

    MatRoi{
        id : matRoi
        input : src.output
        regionWidth: 100
        regionHeight: 100
    }
    
    ImWrite{
        id: imWrite        
    }
    
    Rectangle{
        width: 400
        height: 30
        InputBox{
            id: pathInput
            clip: true
            text: project.dir() + '/../_images/buildings_0246_roi.jpg'
            anchors.rightMargin: 50
            border.width: 1
            border.color: "#061a29"
        }
        TextButton{
            width: 50
            height: parent.height
            anchors.right: parent.right
            text: 'Save'
            onClicked: {
                if ( imWrite.saveImage(pathInput.text, matRoi.output) )
                    console.log("Image saved succesfully")
            }
        }
    }
}
