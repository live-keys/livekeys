import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0
import lcvcore 1.0
import timeline 1.0

CodePalette{
    id: palette

    type: "qml/timeline#Timeline"

    item: Item {
        anchors.fill: parent

        width: videoSurface.width
        height: videoSurface.height + timelineArea.height

        VideoSurface{
            id: videoSurface
            width: 500
            height: 400
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TimelineView{
            id: timelineArea
            anchors.top: parent.top
            anchors.topMargin: 400
            width : parent.width
            height: 200
            surface: videoSurface
            focus : true            
        }
    }

    onInit: {
        timelineArea.timeline = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){}
    }
}
