import QtQuick 2.3
import workspace 1.0 as Workspace
import editor 1.0
import fs 1.0 as Fs
import lcvcore 1.0 as Cv


CodePalette{
    id: palette
    type : "qml/fs#VisualFileSelector"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    property QtObject style: QtObject{
    }

    item: Item{
        width: 250
        height: 200

        property QtObject visualFileSelector: null

        Fs.SelectableFolderView{
            id: selectableFolderView
            anchors.fill: parent
            cwd: parent.visualFileSelector ? parent.visualFileSelector.workingDirectory : ''
            style: palette.paletteStyle ? palette.paletteStyle.selectableListView : defaultStyle
            iconColor: palette.paletteStyle ? palette.paletteStyle.sectionHeaderFocusBackgroundColor : defaultStyle
            onItemSelected: {
                var item = model[index]

                if ( !item.isDir ){
                    var itemPath = Fs.Path.absolutePath(item.path ? item.path : Fs.Path.join(cwd, item.name))
                    parent.visualFileSelector.selectedFile = itemPath
                }

                //HERE:
                //ADD ExtensionPass Act
                //ADD Sepia filter, Warm filter and Dark filter
            }
        }

    }

    onInit: {
        palette.item.visualFileSelector = value
    }

}


