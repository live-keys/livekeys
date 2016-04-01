import QtQuick 2.3
import lcvcore 1.0
import lcvcontrols 1.0

Column{
    
    SelectionWindow{
        id : selectionWindow
        onRegionSelected : {
            console.log("Region selected" + x + y + width + height)
            
        }
    }

    ImRead{
        id : imgSource
        file : "/home/dinu/Work/livecv/samples/_images/object_101_piano_query.jpg"
    }

    Rectangle{
        width : 100
        height : 100
        MouseArea{
            anchors.fill : parent
            onClicked : {
                selectionWindow.show()
                selectionWindow.mat = imgSource.output
            }
        }
    }

}
