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

    MatView{
        id: mv
        mat: Mat{}
        RegionSelection{
            item : matRoi
            anchors.fill : parent
        }
    }

    Rectangle{
        width : matRoi.width
        height : matRoi.height


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
