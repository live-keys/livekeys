import QtQuick 2.3
import live 1.0

Rectangle{
    
    StaticLoader{
        id : staticLoader
     
        source : Rectangle{
            color: "#ff0000"
        }
        Component.onCompleted : {
            staticLoad("item1")
        }
    }
    
    Rectangle{
        width : 100
        height : 100
        color : staticLoader.item ? staticLoader.item.color: "transparent"
        MouseArea{
            anchors.fill : parent
            onClicked : {
                staticLoader.item.color = "#ff00ff"
            }
        }
    }
}
