import QtQuick 2.3
import lcvcore 1.0

Row{

    // Sample showing how to use ColorHistograms for images
    property string imagePath : project.dir() + '/../_images/buildings_0246.jpg'

    ImRead{
        id : imgSource
        file : parent.imagePath
    }

    ColorHistogram{
        input: imgSource.output
    }

}
