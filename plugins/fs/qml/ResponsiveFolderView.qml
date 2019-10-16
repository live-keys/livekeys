import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import base 1.0
import live 1.0
import editor.private 1.0
import fs 1.0 as Fs

ResponsiveListView{
    id: root

    property var selectedIndexes : []
    property color selectedIndexColor: "#252830"

    property string cwd: ''
    onCwdChanged: {
        var wdlist = [{name: '..', isDir: true, size: 0}].concat(Fs.Dir.listDetail(cwd))
        model = wdlist
    }

    onItemSelected: {
        var item = model[index]
        if ( item.isDir ){
            cwd = item.path ? item.path : Fs.Path.join(cwd, item.name)
        }
    }
    
    Keys.onPressed: {
        if ( event.key === Qt.Key_C && (event.modifiers & Qt.ControlModifier) ){
            var m = root.model
            var wd = root.cwd
            var selectedFiles = root.selectedIndexes
                .map(function(i){ return m[i]; })
                .map(function(d){ return d.path ? d.path : Fs.Path.join(wd, d.name) })
            lk.layers.window.clipboard.setPaths(selectedFiles)
        }
    }

    delegate: Component{
        Rectangle{
            width : root.width
            height : 25
            color : {
                if ( ListView.isCurrentItem )
                    return root.selectionColor
                
                if ( root.selectedIndexes.indexOf(index) != -1 )
                    return selectedIndexColor
                
                return 'transparent'
            }
            Text{
                id: label
                anchors.left: parent.left
                anchors.leftMargin: 25
                anchors.verticalCenter: parent.verticalCenter

                font.family: root.fontFamily
                font.pixelSize: root.fontSize

                color: "#fafafa"
                text: modelData.name
            }
            
            Image{
                source: modelData.isDir ? "qrc:/images/project-directory.png" : "qrc:/images/project-file.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 5
            }

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: root.currentIndex = index
                onClicked: {
                    if (mouse.modifiers & Qt.ControlModifier){
                        if ( root.selectedIndexes.indexOf(index) == -1 )
                            root.selectedIndexes.push(index)
                    } else if (mouse.modifiers & Qt.ShiftModifier){
                        if ( root.selectedIndexes.length > 0 ){
                            var last = root.selectedIndexes[root.selectedIndexes.length - 1]
                            var from = index > last ? last : index
                            var to   = index > last ? index : last
                            
                            var newIndexes = root.selectedIndexes
                            
                            for ( var i = from; i < to; ++i ){
                                if ( root.selectedIndexes.indexOf(i) == -1 )
                                    newIndexes.push(i)                 
                            }
                            root.selectedIndexes = newIndexes
                        }
                    } else {
                        root.selectedIndexes = [index]
                    }
                    root.forceActiveFocus()
                }
                onDoubleClicked: root.itemSelected(index)
            }
        }
    }
}

