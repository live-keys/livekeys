import QtQuick 2.3
import lcvcore 1.0
import lcvcontrols 1.0
import lcvfeatures2d 1.0

Column{
    
    Row{
        
        id: row
        
    property string imagePath   : '/home/dinu/Work/livecv/samples/_images/'
    property string trainImage  : imagePath + 'object_101_piano_train1.jpg'
    property string trainImage2 : imagePath + 'caltech_buildings_DSCN0246_small.JPG'
    
    MatList{
        id : objectList
    }
    
    Repeater{
        id : trainImages

        property int     selectedIndex : 0
                        
        model : objectList
        
        delegate : MatView{
            mat : modelData
        }
        
    }
        
        
    }
    
    
    SelectionWindow{
        id : selectionWindow
        onRegionSelected : {
            objectList.appendMat(mat)
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
