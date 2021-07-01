import QtQuick 2.0
import timeline 1.0
import lcvcore 1.0
import live 1.0
import fs 1.0 as Fs
import visual.input 1.0 as Input

Rectangle{
    width: 300
    height: 80
    color: '#141b20'
    border.width: 1
    border.color: '#232b30'

    signal segmentCreated(Segment segment)
    signal cancelled()

    function createSegment(options, track){
        //TODO
//        return segmentFactory.createObject()
    }

    Text{
        anchors.top: parent.top
        anchors.topMargin: 14
        anchors.left: parent.left
        anchors.leftMargin: 14
        text: 'Add script video segment'
        font.family: 'Open Sans, Arial, sans-serif'
        font.pixelSize: 12
        font.weight: Font.Normal
        color: "#afafaf"
    }

    Input.PathInputBox{
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 50
        width: parent.width - 80
        onPathSelected: {
            var objectPathUrl = Fs.UrlInfo.urlFromLocalFile(path)
            var objectComponent = Qt.createComponent(objectPathUrl);
            if ( objectComponent.status === Component.Error ){
                throw linkError(new Error(objectComponent.errorString()), this)
            }

            var newSegment = objectComponent.createObject();
            if ( newSegment.length === 0 ){
                newSegment.length = 1
            }
            newSegment.scriptPath = path
            parent.segmentCreated(newSegment)
        }
    }

    Input.TextButton{
        anchors.right: parent.right
        anchors.rightMargin: 10
        radius: 5
        width: 30
        height: 30
        text: 'X'
        style:  Input.TextButtonStyle{
            backgroundColor: '#3f444d'
            backgroundHoverColor: Qt.lighter('#3f444d', 1.2)
            borderColor: '#575b63'
        }

        onClicked: {
            parent.cancelled()
        }
    }

}
