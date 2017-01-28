import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Row{

    // This sample shows the usage of the CopyMakeBorder element

    ImRead{
       id : src
       file : project.dir() + '/../_images/buildings_0246.jpg'
    }

    CopyMakeBorder{
        input : src.output
        borderType : CopyMakeBorder.BORDER_CONSTANT
        color : "steelblue"
        top : 10
        bottom : 10
        left : 10
        right : 10
    }
}
