import QtQuick 2.11

QtObject{
    property QtObject inputBox: InputBoxStyle{}
    
    property Component incrementButton: Rectangle{
        
        signal clicked()
        
        color: "#444"
        
        Text{
            anchors.centerIn: parent
            color: 'white'
            text: '+'
        }
        MouseArea{
            anchors.fill: parent
            onClicked: parent.clicked()
        }
    }
    
    property Component decrementButton: Rectangle{
        
        signal clicked()
        
        color: "#444"
        
        Text{
            anchors.centerIn: parent
            color: 'white'
            text: '-'
        }

        MouseArea{
            anchors.fill: parent
            onClicked: parent.clicked()
        }
    }
}
