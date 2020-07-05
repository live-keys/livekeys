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
    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null
    property QtObject defaultTimelineStyle: TimelineStyle{}

    item: Item{
        width : 500
        height: 200

        TimelineView{
            id: timelineArea
            anchors.fill: parent
            focus : true
            timelineStyle: paletteStyle ? paletteStyle.timelineStyle : palette.defaultTimelineStyle
        }

        ResizeArea{
            minimumHeight: 200
            minimumWidth: 400
        }

    }

    onInit: {
        timelineArea.timeline = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){}
    }
}
