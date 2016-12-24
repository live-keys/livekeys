import QtQuick 2.3
import lcvcore 1.0
import lcvcontrols 1.0

Column{
    spacing: 10
    
    // This sample shows how to use the ImWrite component
    // to write an image to a file
    
    ImRead{
       id : src
       file : project.dir() + '/../_images/buildings_0246.jpg'
    }

    ChannelSelect{
        id : channelSelect
        input : src.output
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
            text: project.dir() + '/../_images/buildings_0246_grey.jpg'
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
                imWrite.saveImage(pathInput.text, channelSelect.output)
            }
        }
    }
}
