import QtQuick 2.3
import QtQuick.Controls 2.2
import editor 1.0
import editor.private 1.0
import workspace 1.0 as Workspace
import visual.shapes 1.0
import visual.input 1.0 as Input

Workspace.Pane{
    id : viewer
    objectName: "viewer"
    paneType: "viewer"
    color: 'transparent'
    property var runSpace: runSpace
    property var error: error
    property QtObject panes: null
    property alias runnablesMenu: runnablesMenu

    property Theme currentTheme : lk.layers.workspace.themes.current

    Connections{
        target: lk
        function onLayerReady(layer){
            if (!layer || layer.name !== 'workspace' )
                return
            runTypeTriggerSelector.popupMenuStyle = currentTheme.popupMenuStyle
            runTypeTriggerSelector.runLiveTriggerIcon = currentTheme.runLiveTriggerIcon
            runTypeTriggerSelector.runSaveTriggerIcon = currentTheme.runSaveTriggerIcon
            runTypeTriggerSelector.runDisabledTriggerIcon = currentTheme.runDisabledTriggerIcon
        }
    }

    Rectangle {
        id: header
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right

        height: 30
        color: currentTheme ? currentTheme.paneTopBackground : 'black'

        Workspace.PaneDragItem{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            onDragStarted: viewer.panes.__dragStarted(viewer)
            onDragFinished: viewer.panes.__dragFinished(viewer)
            display: "Viewer"
        }

        Rectangle {
            id: modeWrapper
            anchors.left: parent.left
            anchors.leftMargin: 30
            width: 220
            height: 30
            color: 'transparent'

            Item{
                anchors.left: parent.left
                anchors.leftMargin: 35
                height : parent.height
                Text{
                    color :  "#969aa1"
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 12
                    font.family: 'Open Sans, Arial, sans-serif'
                    elide: Text.ElideRight
                    width: 110
                    text : {
                        if (!lk.layers.workspace.project.active)
                            return "";

                        return lk.layers.workspace.project.active.name
                    }
                }
            }

            Triangle{
                id: compileButtonShape
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                width: compileButton.containsMouse ? 8 : 7
                height: compileButton.containsMouse ? 13 : 12
                state : "Released"
                rotation: Triangle.Right

                Behavior on height{ NumberAnimation{ duration: 100 } }
                Behavior on width{ NumberAnimation{ duration: 100 } }

                states: [
                    State {
                        name: "Pressed"
                        PropertyChanges { target: compileButtonShape; color: "#487db9"}
                    },
                    State {
                        name: "Released"
                        PropertyChanges { target: compileButtonShape; color: compileButton.containsMouse ? "#768aca" : "#bcbdc1"}
                    }
                ]
                transitions: [
                    Transition {
                        from: "Pressed"
                        to: "Released"
                        ColorAnimation { target: compileButtonShape; duration: 100}
                    },
                    Transition {
                        from: "Released"
                        to: "Pressed"
                        ColorAnimation { target: compileButtonShape; duration: 100}
                    }
                ]
            }

            MouseArea{
                id : compileButton
                anchors.left: parent.left
                anchors.leftMargin: 0
                width: 50
                height: 30
                hoverEnabled: true
                onPressed: compileButtonShape.state = "Pressed"
                onReleased: compileButtonShape.state = "Released"
                onClicked: { lk.layers.workspace.project.run() }
            }
            Workspace.Tooltip{
                mouseOver: compileButton.containsMouse
                text: "Run active file"
            }

            Item{
                anchors.right: parent.right
                anchors.rightMargin: 50
                width: 30
                height: parent.height

                Image{
                    id: switchRunnableImage
                    anchors.centerIn: parent
                    source : "qrc:/images/switch-file.png"
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: runnablesMenu.visible = !runnablesMenu.visible
                }
            }

            RunTriggerTypeSelector{
                id: runTypeTriggerSelector
                anchors.right: parent.right
                height: 30
                trigger: lk.layers.workspace.project.active ? lk.layers.workspace.project.active.runTrigger : Document.RunManual

                onTriggerSelected: {
                    if ( lk.layers.workspace.project.active ){
                        lk.layers.workspace.project.active.runTrigger = trigger
                    }
                }
            }

            Rectangle{
                width: parent.width
                height: 1
                color: "#1a1f25"
                anchors.bottom: parent.bottom
            }
        }


        Item{
            anchors.right: parent.right
            width: 30
            height: parent.height

            Image{
                id : paneOptions
                anchors.centerIn: parent
                source : "qrc:/images/pane-menu.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    projectMenu.visible = !projectMenu.visible
                }
            }
        }
    }

    Rectangle{
        id: runSpace
        clip: true
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "black"
    }

    Connections{
        target: lk.layers.workspace.project.active
        function onObjectReady(){ error.text = '' }
        function onRunError(errors){
            var errorMessage = error.wrapMessage(errors)
            error.text = errorMessage.rich
            console.error(errorMessage.log)
        }
    }

    ErrorContainer{
        id: error
        anchors.bottom: parent.bottom
        width : parent.width
        color : "#000"
        font: Qt.font({
            family: "Source Code Pro, Ubuntu Mono, Courier New, Courier",
            pixelSize: 12,
            weight: Font.Normal
        })
    }

    RunnablesMenu{
        id: runnablesMenu
        anchors.top: header.bottom
        onRunnableSelected: {
            if ( lk.layers.workspace.project.active )
                lk.layers.workspace.project.active.setRunSpace(null)

            lk.layers.workspace.project.setActive(path)
        }
        x: 130
    }

    Workspace.PaneMenu{
        id: projectMenu
        visible: false
        anchors.right: parent.right
        anchors.topMargin: 30
        anchors.top: parent.top

        style: viewer.currentTheme.popupMenuStyle

        Workspace.PaneMenuItem{
            text: qsTr("Move to new window")
            onClicked: {
                projectMenu.visible = false
                viewer.panes.movePaneToNewWindow(viewer)
            }
        }
        Workspace.PaneMenuItem{
            text: qsTr("Remove View")
            onClicked: {
                projectMenu.visible = false
                viewer.panes.removePane(viewer)
            }
        }
    }

}
