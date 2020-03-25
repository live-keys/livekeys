import QtQuick 2.3
import QtQuick.Controls 2.1

import workspace.quickqanava 2.0 as Qan

Qan.NodeItem{
    id: root
    
    width: 180
    height: wrapper.height
    
    property string label: ''
    property alias propertyContainer: propertyContainer
    
    Rectangle{
        id: wrapper
        width: parent.width
        height: nodeTitle.height + propertyContainer.height + 10
        color: "#112"
        radius: 5
        border.color: "#555"
        border.width: 1
        
        Rectangle{
            id: nodeTitle
            width: parent.width
            height: 30
            anchors.top: parent.top
            color: '#666'
            radius: 5
            
            Text{
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 15
                color: 'white'
                text: root.label
            }
        }
        
        Column{
            id: propertyContainer
            spacing: 5
            anchors.top: parent.top
            anchors.topMargin: 35
            anchors.left: parent.left
            anchors.leftMargin: 3
        }
    }
}
