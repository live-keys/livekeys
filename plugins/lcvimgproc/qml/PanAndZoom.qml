import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Row{
    id: root

    property alias input: mv.mat
    property int regionWidth: 20
    property int regionHeight: 20
    property int regionX: 0
    property int regionY: 0
    property alias selectionColor: regionSelection.borderColor

    MatView{
        id: mv
        mat: Mat{}
        RegionSelection{
            id: regionSelection
            item : matRoi
            anchors.fill : parent
        }
    }

    Rectangle{
        width : matRoi.width
        height : matRoi.height
        border.width: 1
        border.color: regionSelection.borderColor


        MatRoi{
            id : matRoi
            width : imgval.width
            height : imgval.height
            regionWidth  : root.regionWidth
            regionHeight : root.regionHeight
            regionX : root.regionX
            regionY : root.regionY
            linearFilter: false
            input : mv.mat
        }

        MatRead{
            id : imgval
            input : matRoi.output
            opacity : 0.5
            squareCell : true
        }
    }
}
