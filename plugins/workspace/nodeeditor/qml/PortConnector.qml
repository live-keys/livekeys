import QtQuick 2.3
import live 1.0
import visual.shapes 1.0

Rectangle{
    id: defaultConnectorItem
    anchors.fill: parent
    visible: false
    z: 15
    state: "NORMAL"
    color: "#364"
    smooth: true;
    antialiasing: true
    radius: width / 2
    
    border.width: 1
    border.color: "#253"
    
    rotation: Triangle.Right
    
    property real borderWidth: 5
    
    states: [
        State { name: "NORMAL";
            PropertyChanges { 
                target: defaultConnectorItem; 
                scale: 0.8
            }
        },
        State { name: "HILIGHT"
            PropertyChanges {
                target: defaultConnectorItem; 
                scale: 1.7 
            }
        }
    ]
    transitions: [
        Transition {
            from: "NORMAL"; to: "HILIGHT"; 
            PropertyAnimation { 
                target: defaultConnectorItem; 
                properties: "borderWidth, scale"; 
                duration: 100 
            }
        },
        Transition {
            from: "HILIGHT"; to: "NORMAL"; 
            PropertyAnimation { 
                target: defaultConnectorItem; 
                properties: "borderWidth, scale"; 
                duration: 150 
            }
        }
    ]
}
