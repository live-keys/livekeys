import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Row{

    // This sample shows the usage of the CopyMakeBorder element

    ImageRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }

    CopyMakeBorder{
        id: cmb
        input : src.result
        color : "steelblue"
        top : 100
        bottom : 10
        left : 10
        right : 10
    }
    
    ImageView {
        image: cmb.result
    }
}
