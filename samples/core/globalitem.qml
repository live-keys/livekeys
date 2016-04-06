import lcvcore 1.0

Column{

    GlobalItem{
        stateId : "state1"
    
        source : Rectangle{
            width : 100
            height : 100
            color : "#ff0000"
            MouseArea{
                anchors.fill : parent
                onClicked : {
                    parent.color = "#ff00ff"
                    parent.height = 100
                }
            }
        }
    }
    
    Rectangle{
            width : 100
            height : 100
            color : "#fff000"
            MouseArea{
                anchors.fill : parent
                onClicked : {
                    parent.color = "#ff00ff"
                    parent.height = 100
                }
            }
        }
}