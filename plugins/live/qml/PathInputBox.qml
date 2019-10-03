import QtQuick 2.3
import fs 1.0 as Fs

Item{
    id: root
    height: 30
    width: parent.width - 20

    signal pathSelected(string path)

    property alias path: pathInput.text

    InputBox{
        id: pathInput
        anchors.left: parent.left
        width: parent.width - 31
        radius: 5
        height: 30
        font.family: "Open Sans, sans-serif"
        textColor: '#afafaf'

        onKeyPressed: {
            if ( event.key === Qt.Key_Return ){
                root.pathSelected(pathInput.text)
            }
        }
    }

    TextButton{
        anchors.right: parent.right
        radius: 5
        width: 30
        height: 30
        text: '/'
        onClicked: {
            lk.layers.window.dialogs.openFile({}, function(url){
                pathInput.text = Fs.Path.toLocalFile(url)
                root.pathSelected(pathInput.text)
            })
        }
    }
}
