import QtQuick 2.0
import timeline 1.0
import lcvcore 1.0
import live 1.0
import workspace 1.0 as Workspace
import fs 1.0 as Fs

Rectangle{
    width: 300
    height: 80
    color: '#03070a'

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
        text: 'Add video segment'
        font.family: 'Open Sans, Arial, sans-serif'
        font.pixelSize: 12
        font.weight: Font.Normal
        color: "#afafaf"
    }

    Workspace.PathInputBox{
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


    TextButton{
        anchors.right: parent.right
        anchors.rightMargin: 10
        radius: 5
        width: 30
        height: 30
        text: 'X'
        onClicked: {
            parent.cancelled()
        }
    }

}
