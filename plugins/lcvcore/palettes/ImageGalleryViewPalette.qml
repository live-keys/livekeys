import QtQuick 2.3
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons
import editor 1.0
import fs 1.0 as Fs
import lcvcore 1.0 as Cv
import lcvimgproc 1.0
import live 1.0
import QtQuick.Controls.Styles 1.2

CodePalette{
    id: palette
    type : "qml/lcvcore#ImageGalleryView"

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject style: QtObject{}

    item: Item{
        id: root
        width: 450
        height: 200

        property QtObject imageGalleryView: null

        Cv.ImageGalleryView{
            anchors.fill: parent
            files: root.imageGalleryView ? root.imageGalleryView.files : []
            images: root.imageGalleryView ? root.imageGalleryView.images : null
        }

        ResizeArea{
            minimumHeight: 80
            minimumWidth: 80
        }
    }

    onInit: {
        palette.item.imageGalleryView = value
    }

}


