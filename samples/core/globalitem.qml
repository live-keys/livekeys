import lcvcore 1.0

Column{

    GlobalItem{
        id : globalItem
        stateId : "state1"
     
        source : Rectangle{
            width : 100
            height : 100
            color : "#ff0000"
        }   
    }
    
    Rectangle{
        width : 100
        height : 100
        color : "#fff000"
        MouseArea{
            anchors.fill : parent
            onClicked : {
                globalItem.item.color = "#ff00ff"
                parent.color = "#ff00ff"
            }
        }
    }
}
