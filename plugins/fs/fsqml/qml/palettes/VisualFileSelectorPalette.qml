import QtQuick 2.3
import workspace 1.0 as Workspace
import editor 1.0
import fs 1.0 as Fs
import lcvcore 1.0 as Cv


CodePalette{
    id: palette
    type : "qml/fs#VisualFileSelector"

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject style: QtObject{}

    item: Item{
        width: 250
        height: 200

        property QtObject visualFileSelector: null

        Fs.SelectableFolderView{
            id: selectableFolderView
            anchors.fill: parent
            cwd: {
                if (!parent.visualFileSelector)
                    return ''
                if (!Fs.Path.exists(parent.visualFileSelector.workingDirectory))
                    return project.dir()
                return parent.visualFileSelector.workingDirectory
            }
            style: palette.theme.selectableListView
            iconColor: palette.theme.colorScheme.middlegroundOverlayDominant
            onItemSelected: {
                var item = model[index]

                if ( !item.isDir ){
                    var itemPath = Fs.Path.absolutePath(item.path ? item.path : Fs.Path.join(cwd, item.name))
                    parent.visualFileSelector.selectedFile = itemPath
                }
            }
            onItemHighlighted: {
                var item = model[index]

                if ( !item.isDir ){
                    var itemPath = Fs.Path.absolutePath(item.path ? item.path : Fs.Path.join(cwd, item.name))
                    parent.visualFileSelector.highlightedFile = itemPath
                }
            }
        }

    }

    onInit: {
        palette.item.visualFileSelector = value
    }

}


