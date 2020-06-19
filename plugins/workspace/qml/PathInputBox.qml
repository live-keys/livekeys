import QtQuick 2.3
import fs 1.0 as Fs
import workspace 1.0

Item{
    id: root
    height: 30
    width: 240

    signal pathSelected(string path)

    property alias path: pathInput.text
    property alias font: pathInput.font

    property QtObject defaultStyle: QtObject{

        property QtObject inputBoxStyle: QtObject{
            property QtObject textStyle: TextStyle{}
            property QtObject hintTextStyle: TextStyle{}
            property color backgroundColor: '#070b0f'
            property color borderColor: '#323232'
            property double borderThickness: 1
            property color textSelectionColor: '#3d4856'
            property double radius: 3
        }

        property QtObject buttonStyle: QtObject{
            property QtObject textStyle: TextStyle{}
            property QtObject hoverTextStyle: TextStyle{}
            property color backgroundColor: '#070b0f'
            property color backgroundHoverColor: '#213355'
            property color borderColor: '#323232'
            property color borderHoverColor: '#323232'
            property double borderThickness: 1
            property double radius: 3
        }
    }

    property QtObject style: defaultStyle

    InputBox{
        id: pathInput
        anchors.left: parent.left
        width: parent.width - 31
        height: root.height

        style: root.style.inputBoxStyle

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
        height: root.height
        style: root.style.buttonStyle
        text: '/'
        onClicked: {
            lk.layers.window.dialogs.openFile({}, function(url){
                pathInput.text = Fs.Path.toLocalFile(url)
                root.pathSelected(pathInput.text)
            })
        }
    }
}
