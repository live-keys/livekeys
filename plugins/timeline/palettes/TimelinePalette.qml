import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0
import lcvcore 1.0
import timeline 1.0
import fs 1.0 as Fs

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
                var trackProperties = track.configuredProperties(segment)

                if ( trackProperties.editor ){
                    var objectPath = trackProperties.editor
                    var objectUrl = Fs.UrlInfo.urlFromLocalFile(objectPath)

                    var objectComponent = Qt.createComponent(objectUrl);
                    if ( objectComponent.status === Component.Error ){
                        throw linkError(new Error(objectComponent.errorString()), timelineArea)
                    }

                    var object = objectComponent.createObject()
                    object.currentSegment = segment
                    var overlay = lk.layers.window.dialogs.overlayBox(object)
                    overlay.centerBox = false
                    overlay.backgroundVisible = true
                    overlay.box.visible = true

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
    onValueFromBindingChanged: {
        timelineArea.timeline = value
    }

    onEditFragmentChanged: {
        editFragment.whenBinding = function(){}
    }
}
