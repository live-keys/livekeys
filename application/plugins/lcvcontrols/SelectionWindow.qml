import QtQuick 2.3
import QtQuick.Window 2.2
import lcvcore 1.0
import lcvcontrols 1.0

Window{
    id : selectionWindow

    color: "#05111b"

    width : matView.width
    height : matView.height + 30

    property alias mat : matView.mat
    signal regionSelected(Mat region, int x, int y, int width, int height)

    MatView{
        id : matView

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
        input : matView.mat
        visible : false
        regionX : 0
        regionY : 0
        regionWidth : selectionArea.width
        regionHeight : selectionArea.height
    }
    
    Rectangle{
        width : 100
        height : 30
        color : "#ff0000"
        anchors.bottom: parent.bottom
        MouseArea{
            anchors.fill : parent
            onClicked : {
                selectionWindow.regionSelected(
                    selectedRegion.output.createOwnedObject(),
                    selectedRegion.regionX,
                    selectedRegion.regionY,
                    selectedRegion.regionWidth,
                    selectedRegion.regionHeight
                )
                selectionWindow.close()
            }
        }
    }
}
