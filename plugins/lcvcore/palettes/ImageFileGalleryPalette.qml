import QtQuick 2.3
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons
import editor 1.0
import fs 1.0 as Fs
import lcvcore 1.0 as Cv
import lcvimgproc 1.0
import QtQuick.Controls.Styles 1.2

CodePalette{
    id: palette
    type : "qml/lcvcore#ImageFileGallery"

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject style: QtObject{}

    item: Item{
        id: root
        width: 450
        height: 200

        property QtObject imageFileGallery: null

        Cv.ImageGalleryView{
            anchors.fill: parent
            files: imageFileGallery ? imageFileGallery.run(imageFileGallery.path, imageFileGallery.extensions) : []
        }
    }

    onInit: {
        palette.item.imageFileGallery = value
    }

}


