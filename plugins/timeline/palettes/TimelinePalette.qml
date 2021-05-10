import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0
import lcvcore 1.0
import timeline 1.0
import workspace 1.0 as Workspace
import fs 1.0 as Fs
import visual.input 1.0 as Input

CodePalette{
    id: palette

    type: "qml/timeline#Timeline"
    property QtObject theme: lk.layers.workspace.themes.current
    property QtObject defaultTimelineStyle: TimelineStyle{}

    item: Item{
        id: paletteItem
        width : 500
        height: 200

        property Component timelineConfigComponent: Rectangle{
            anchors.fill: parent
            color: "#cc000000"

            Input.Label{
                textStyle: theme.inputStyle.textStyle
                text: "Setup Timeline"
                anchors.top: parent.top
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Item{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 50

                width: 180
                height: 30

                Input.Label{
                    textStyle: theme.inputStyle.textStyle
                    text: "Fps:"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Input.InputBox{
                    id: fpsInput
                    style: theme.inputStyle
                    text: "30"
                    width: 100

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 80
                }
            }

            Item{
                anchors.top: parent.top
                anchors.topMargin: 90
                anchors.horizontalCenter: parent.horizontalCenter
                width: 180
                height: 30

                Input.Label{
                    textStyle: theme.inputStyle.textStyle
                    text: "Length:"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Input.InputBox{
                    id: lengthInput
                    style: theme.inputStyle
                    text: "1000"
                    width: 100

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 80
                }
            }

            Workspace.TextButton{
                text: "Ready"
                style: theme.formButtonStyle
                anchors.top: parent.top
                anchors.topMargin: 160
                anchors.horizontalCenter: parent.horizontalCenter

                width: 100
                height: 25

                onClicked: {
                    timelineArea.timeline.fps = parseFloat(fpsInput.text)
                    timelineArea.timeline.contentLength = parseFloat(lengthInput.text)

                    editFragment.writeProperties({
                        'fps' : timelineArea.timeline.fps,
                        'contentLength' : timelineArea.timeline.contentLength
                    })

                    timelineConfig.sourceComponent = null
                }
            }
        }

        TimelineView{
            id: timelineArea
            anchors.fill: parent
            focus : true
            timelineStyle: theme.timelineStyle
            handleContextMenu: function(item){
                if ( item.objectName === 'timelineTrackTitle' ){
                    timelineArea.trackTitleRightClicked(item.trackIndex, item)
                } else if ( item.objectName === 'timelineOptions' ){
                    var pane = null
                    var p = parent
                    while ( p ){
                        if ( p.paneType !== undefined ){
                            break
                        }
                        p = p.parent
                    }

                    if ( p ){
                        lk.layers.workspace.panes.activateItem(item, p)
                        lk.layers.workspace.panes.openContextMenu(item, p)
                    }
                }
            }

            onSegmentRightClicked: {
                var pane = null
                var p = parent
                while ( p ){
                    if ( p.paneType !== undefined ){
                        break
                    }
                    p = p.parent
                }

                if ( p ){
                    lk.layers.workspace.panes.activateItem(delegate, p)
                    lk.layers.workspace.panes.openContextMenu(delegate, p)
                }
            }

            onTrackTitleRightClicked: {
                var pane = null
                var p = parent
                while ( p ){
                    if ( p.paneType !== undefined ){
                        break
                    }
                    p = p.parent
                }

                if ( p ){
                    lk.layers.workspace.panes.activateItem(delegate, p)
                    lk.layers.workspace.panes.openContextMenu(delegate, p)
                }
            }

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

        Loader{
            id: timelineConfig
            anchors.fill: parent
        }

        ResizeArea{
            minimumHeight: 200
            minimumWidth: 400
        }

    }

    onInit: {
        timelineArea.timeline = value
        if ( value ){
            if ( value.fps === 0 || value.contentLength === 0 ){
                timelineConfig.sourceComponent = paletteItem.timelineConfigComponent
            } else {
                timelineConfig.sourceComponent = null
            }
        } else {
            timelineConfig.sourceComponent = null
        }
    }
    onValueFromBindingChanged: {
        timelineArea.timeline = value
    }

    onEditFragmentChanged: {
        editFragment.whenBinding = function(){}
    }
}
