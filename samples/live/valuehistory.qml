import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import live 1.0

Column{
    spacing: 2
    
    Row{
        
        Slider{
            id: slider
            anchors.top: parent.top
            anchors.topMargin: 12
            width: 200
            height: 20
            minimumValue: -200
            maximumValue: 200
            stepSize: 1.0
            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 8
                    color: "#15202c"
                }
                handle: Rectangle{
                    width: 8
                    height: 18
                    radius: 5
                    border.width: 1
                    border.color: '#233569'
                    color: '#b2b2b2'
                }
            }
        }
        
        TextButton{
            text: "Update"
            onClicked: valueHistory.currentValue = slider.value
        }
        
    }
    
    ValueHistory{
        id: valueHistory
        width: 400
        height: 300
        Component.onCompleted: {
            currentValue = 0
            currentValue = 20
            currentValue = -100
            currentValue = 200
        }
    }
    
    
}