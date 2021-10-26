import QtQuick 2.3
import base 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img
import fs 1.0 as Fs
import workspace.icons 1.0 as Icons
import visual.input 1.0 as Input

GridView{
    id: gridView

    width: 400
    height: 300

    cellHeight: 140
    cellWidth: 140

    clip: true

    property var files: null
    property var images: null
    property bool labelVisible: false
    property Input.TextStyle labelStyle: Input.TextStyle{}
    property var imageExtensions: ['jpg', 'png', 'bmp', 'jpeg']


    model: images ? images : files
    delegate: images ? __imageModelDelegate : __fileModelDelegate

    property Component __imageDelegate: Item{
        anchors.fill: parent

        Cv.ImageRead {
            id: imRead
            file: parent.parent.path
        }

        Act{
            id: resize
            property Cv.Mat input: imRead.result
            args: ['$input']
            returns: 'qml/object'

            run: function(input){
                if ( !input )
                    return null
                var imageSize = input.dimensions()
                if ( imageSize.width === 0 || imageSize.height === 0 )
                    return input


                var scale = 0
                var wScale = parent.width / imageSize.width
                scale = wScale

                var hScale = parent.height / imageSize.height
                scale = scale === 0
                            ? hScale
                            : scale < hScale ? hScale : scale

                if ( scale < 1 )
                    return Img.Geometry.scale(input, scale, scale, Img.Geometry.INTER_LINEAR)

                return input
            }
        }

        Cv.ImageView{
            id: imageView
            image: resize.result
            linearFilter: false
        }
    }

    property Component __fileDelegate: Item{
        anchors.fill: parent
        Icons.FileIcon{
            anchors.centerIn: parent
            width: 50
            height: 50
        }
    }

    property Component __fileModelDelegate: Rectangle{
        width: gridView.cellWidth
        height: gridView.cellHeight
        color: 'transparent'
        clip: true

        property string path: modelData
        property string name: Fs.Path.name(modelData)
        property string extension: Fs.Path.suffix(modelData)

        Loader{
            x: 1
            width: parent.width - 2
            height: parent.height - (gridView.labelVisible ? 20 : 1)
            clip: true
            sourceComponent: {
                return gridView.imageExtensions.includes(parent.extension)
                    ? gridView.__imageDelegate
                    : gridView.__fileDelegate
            }
        }

        Input.Label{
            visible: gridView.labelVisible
            textStyle: gridView.labelStyle
            text: parent.name
            width: parent.width - 10
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
        }
    }

    property Component __imageModelDelegate: Rectangle{
        width: gridView.cellWidth
        height: gridView.cellHeight
        color: 'transparent'
        clip: true

        Cv.ImageView{
            id: imageView
            image: modelData
            linearFilter: false
        }
    }

}
