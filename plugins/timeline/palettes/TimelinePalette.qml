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
    property QtObject theme: lk.layers.workspace.themes.current
    property QtObject defaultTimelineStyle: TimelineStyle{}

    item: Item{
        width : 500
        height: 200

        TimelineView{
            id: timelineArea
            anchors.fill: parent
            focus : true
            timelineStyle: theme.timelineStyle

            onSegmentDoubleClicked: {
                if ( segment instanceof Keyframe ){
                    var objectPath = lk.layers.workspace.pluginsPath() + '/timeline/KeyframeEditor.qml'

                    var objectComponent = Qt.createComponent(objectPath);
                    if ( objectComponent.status === Component.Error ){
                        throw linkError(new Error(objectComponent.errorString()), timelineArea)
                    }

                    var coords = delegate.mapToItem(lk.layers.workspace.panes.container, 0, 0)

                    var object = objectComponent.createObject()
                    object.currentKeyframe = segment
                    var overlay = lk.layers.window.dialogs.overlayBox(object)
                    overlay.centerBox = false
                    overlay.backgroundVisible = true
                    overlay.box.visible = true
                    overlay.boxX = coords.x
                    overlay.boxY = coords.y

                    object.inputBox.forceFocus()

                    object.ready.connect(function(){
                        overlay.closeBox()
                        object.destroy()
                    })
                }
            }
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
