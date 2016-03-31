import lcvcore 1.0
import lcvcontrols 1.0
import QtQuick.Window 2.2

Column{


Window{
    width :100
    height : 100
    Component.onCompleted : {
//        show();
    }
}
        
    property alias input : matView.mat
    
    ImRead{
        id : imgSource
    }
    
    MatView{
        id : matView
        mat : imgSource.output
        
        SelectionArea{
            id : selectionArea
            anchors.fill: parent
            onSelected : {
                selectedRegion.regionX = x
                selectedRegion.regionY = y
                selectedRegion.regionWidth = width
                selectedRegion.regionHeight = height
            }
        }
    }
    
    MatRoi{
        id: selectedRegion
        input : imgSource.output
        regionX : 0
        regionY : 0
        regionWidth : selectionArea.width - 1
        regionHeight : selectionArea.height - 1
    }
}