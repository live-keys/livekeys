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
    type : "qml/lcvcore#GalleryFileSelector"

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject style: QtObject{}

    item: Item{
        id: root
        width: 450
        height: 200

        property QtObject galleryFileSelector: null

        GridView {
            id: gridView
            width: 450
            height: 200
            cellWidth: 140
            cellHeight: 160
            property var path: {
                if (!parent.galleryFileSelector)
                    return ''
                if (!Fs.Path.exists(parent.galleryFileSelector.workingDirectory))
                    return project.dir()
                else
                    return parent.galleryFileSelector.workingDirectory
            }
            model: {
                var res = Fs.Dir.listDetail(gridView.path)
                if (res)
                    res = res.filter(item => item.isDir === false)
                return res
            }
            delegate: Rectangle {
                width: 140
                height: 160
                color: 'transparent'
                property bool isImage: modelData.name.toLowerCase().endsWith('.jpg')
                                    || modelData.name.toLowerCase().endsWith('.png')
                                    || modelData.name.toLowerCase().endsWith('.bmp')
                                    || modelData.name.toLowerCase().endsWith('.jpeg')


                Rectangle {
                    id: rec
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    width: 110
                    height: 110
                    color: "transparent"
                    anchors.horizontalCenter: parent.horizontalCenter

                    Loader {
                        property var imageDelegate: Component {
                            Item {
                                width: imageView.width
                                height: imageView.height
                                Cv.ImageRead {
                                    id: imRead
                                    file: gridView.path + "/" + modelData.name
                                }

                                Resize {
                                    id: resize
                                    input: imRead.result
                                    size: {
                                        var dims  = input ? input.dimensions() : Qt.size(100, 100)
                                        if (dims.width > dims.height){
                                            return Qt.size(100.0, 100.0 * dims.height / dims.width)
                                        } else {
                                            return Qt.size(100.0 * dims.width / dims.height, 100.0)
                                        }
                                    }
                                }

                                Cv.ImageView{
                                    id: imageView
                                    image: resize.result
                                    width: image ? image.dimensions().width: 100
                                    height: image ? image.dimensions().height: 100

                                    linearFilter: false
                                }

                            }
                        }
                        property var nonImageDelegate: Component {
                            Icons.FileIcon{
                                width: 50
                                height: 50
                            }
                        }
                        sourceComponent: isImage ? imageDelegate : nonImageDelegate
                        anchors.centerIn: parent

                    }
                }
                Rectangle {
                    width: text.width
                    height: text.height
                    color: "transparent"
                    anchors.top: rec.bottom
                    anchors.topMargin: 5
                    Text {
                        id: text
                        text: modelData.name
                        color: "white"
                        height: 20
                        width: 120
                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignHCenter
                    }
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                MouseArea {
                    id: ma
                    anchors.fill: parent
                    onClicked: {
                        gridView.currentIndex = index
                        if (root.galleryFileSelector)
                            root.galleryFileSelector.highlightedFile = gridView.path + "/" + modelData.name
                    }
                    onDoubleClicked: {
                        if (root.galleryFileSelector && parent.isImage){
                            root.galleryFileSelector.selectedFile = gridView.path + "/" + modelData.name
                        }
                    }
                }


            }

            highlight: Rectangle {
                color: "#66cccccc"
            }
            highlightFollowsCurrentItem: true
            focus: true
            clip: true

        }

    }

    onInit: {
        palette.item.galleryFileSelector = value
    }

}


