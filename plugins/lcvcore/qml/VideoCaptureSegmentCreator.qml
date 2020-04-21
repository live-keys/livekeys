import QtQuick 2.0
import timeline 1.0
import lcvcore 1.0
import live 1.0

Rectangle{
    width: 300
    height: 80
    color: '#03070a'

    signal segmentCreated(Segment segment)
    signal cancelled()

    property Component segmentFactory : VideoSegment{

    }

    function createSegment(options, track){
        return segmentFactory.createObject()
        //TODO: Add serialization options
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

    PathInputBox{
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 50
        width: parent.width - 80
        onPathSelected: {
            var newSegment = segmentFactory.createObject()
            newSegment.file = path
            newSegment.length = 20
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
