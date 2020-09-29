import QtQuick 2.0
import live 1.0
import editor 1.0
import editor.private 1.0
import workspace 1.0 as Workspace

Pane{
    id : viewer
    objectName: "viewer"
    paneType: "viewer"
    color: 'transparent'
    property var runSpace: runSpace
    property var error: error
    property QtObject panes: null
    property alias modeContainer: modeContainer
    property alias runnablesMenu: runnablesMenu

    property Theme currentTheme : lk.layers.workspace.themes.current

    Connections{
        target: lk
        onLayerReady: {
            if (!layer || layer.name !== 'workspace' ) return
            modeContainer.liveImage.source = currentTheme.topLiveModeIcon
            modeContainer.onSaveImage.source = currentTheme.topOnSaveModeIcon
            modeContainer.disabledImage.source = currentTheme.topDisabledModeIcon
        }
    }

    Rectangle {
        id: header
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right

        height: 30
        color: currentTheme ? currentTheme.paneTopBackground : 'black'

        PaneDragItem{
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
                        if (!project.active)
                            return "";

                        return project.active.name
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
                onClicked: { project.run() }
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

            Item{
                width: modeImage.width + modeImage.anchors.rightMargin + 10
                height: parent.height
                anchors.right: parent.right

                Item{
                    width: 25
                    height: 25
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 20

                    Image{
                        id : modeImage
                        anchors.centerIn: parent
                        source: project.runTrigger === Project.RunOnChange
                            ? modeContainer.liveImage.source
                            : project.runTrigger === Project.RunOnSave
                                ? modeContainer.onSaveImage.source
                                : modeContainer.disabledImage.source
                    }
                }


                Triangle{
                    anchors.right: parent.right
                    anchors.rightMargin: 7
                    anchors.verticalCenter: parent.verticalCenter
                    width: 9
                    height: 5
                    color: openStatesDropdown.containsMouse ? "#9b6804" : "#bcbdc1"
                    rotation: Triangle.Bottom
                }

                MouseArea{
                    id : openStatesDropdown
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: modeContainer.visible = !modeContainer.visible
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
        target: project.active
        onObjectReady : { error.text = '' }
        onRunError : {
            var errorMessage = error.wrapMessage(errors)
            error.text = errorMessage.rich
            console.error(errorMessage.log)
        }
    }

    ErrorContainer{
        id: error
        anchors.bottom: parent.bottom
        width : parent.width
        color : root.style.errorBackgroundColor
        font: root.style.errorFont
    }

    RunnablesMenu{
        id: runnablesMenu
        anchors.top: header.bottom
        onRunnableSelected: {
            if ( project.active )
                project.active.setRunSpace(null)

            project.setActive(path)
        }
        x: 130
    }

    ModeContainer {
        id: modeContainer
        onRunTriggerSelected: project.runTrigger = trigger
        anchors.top: header.bottom
        x: 200
    }

    Rectangle{
        id: projectMenu
        visible: false
        anchors.right: parent.right
        anchors.top: header.bottom
        property int buttonHeight: 30
        property int buttonWidth: 180
        opacity: visible ? 1.0 : 0
        z: 1000
        color : "#03070b"

        Behavior on opacity{ NumberAnimation{ duration: 200 } }

        Rectangle{
            id: newWindowEditorButton
            anchors.top: parent.top
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: newWindowEditorText
                text: qsTr("Move to new window")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: newWindowEditorArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : newWindowEditorArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    projectMenu.visible = false
                    viewer.panes.movePaneToNewWindow(viewer)
                }
            }
        }

        Rectangle{
            id: removeProjectViewButton
            anchors.top: newWindowEditorButton.bottom
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text{
                text: qsTr("Remove Project View")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: removeProjectViewArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : removeProjectViewArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    projectMenu.visible = false
                    viewer.panes.removePane(viewer)
                }
            }
        }
    }

}
