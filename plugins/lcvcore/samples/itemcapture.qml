import QtQuick 2.3
import lcvcore 1.0

Item{
    // This sample shows the usage of the ItemCapture element
    // The capture() method is called whenever we want to 
    // capture the screen into the ItemCapture's Mat
    
    anchors.fill: parent
    
    Item {
        width: 200
        height: 100
        
        Row {
            opacity: 0.5
            Item {
                id: foo
                width: 100; height: 100
                Rectangle { id: r1; x: 5; y: 5; width: 60; height: 60; color: "red" }
                Rectangle { x: 20; y: 20; width: 60; height: 60; color: "orange" }
                Rectangle { x: 35; y: 35; width: 60; height: 60; color: "yellow" }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        r1.color = "white"
                        itemCapture.capture() // capture the change
                    }
                }
            }
            
            ShaderEffectSource {
                id: shaderEffectSource
                width: foo.width 
                height: foo.height
                sourceItem: foo
            }
            
        }
    }
    
    ItemCapture{
        id: itemCapture
        anchors.right: parent.right
        captureSource : shaderEffectSource
        Component.onCompleted : {
            capture()
        }
    }
    
    
}

