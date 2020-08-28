import QtQuick 2.3
import fs 1.0

Item{
    anchors.fill: parent
    
    SelectableFolderView{
        width: 300
        height: 200
        cwd: project.dir() + '/..'
    }
}