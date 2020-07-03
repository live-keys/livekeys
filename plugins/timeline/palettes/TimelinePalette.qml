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


    item: TimelineView{
        id: timelineArea
        width : 500
        height: 200
        focus : true
    }

    onInit: {
        timelineArea.timeline = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){}
    }
}
