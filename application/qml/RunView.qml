import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import base 1.0
import visual.shapes 1.0
import workspace 1.0

Pane{
    id: root

    objectName : 'runView'
    paneType: 'runView'
    paneState : { return {} }
    color: '#000509'

    property var panes: null
    property Theme currentTheme : lk.layers.workspace ? lk.layers.workspace.themes.current : null

    property color topColor: currentTheme ? currentTheme.paneTopBackground : 'black'

    paneClone: function(){
        return root.panes.createPane('runView', paneState, [root.width, root.height])
    }

    property var currentRunnable : null

    Rectangle{
        id: paneHeader
        anchors.left: parent.left
        anchors.top: parent.top

        width: parent.width
        height: 30
        color: root.topColor

        Rectangle {
            id: modeWrapper
            anchors.left: parent.left
            anchors.leftMargin: 20
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
                    width: 130
                    text : {
                        if (!root.currentRunnable)
                            return "";

                        return root.currentRunnable.name
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
                onClicked: {
                    if ( root.currentRunnable )
                        root.currentRunnable.run()
                }
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

            //TODO
//            RunTriggerTypeSelector{
//                id: runTypeTriggerSelector
//                anchors.right: parent.right
//                height: 30
//                trigger: lk.layers.workspace.project.active ? lk.layers.workspace.project.active.runTrigger : Document.RunManual

//                onTriggerSelected: {
//                    if ( lk.layers.workspace.project.active ){
//                        lk.layers.workspace.project.active.runTrigger = trigger
//                    }
//                }
//            }

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
                    paneMenu.visible = !paneMenu.visible
                }
            }
        }
    }

    Rectangle{
        id: paneMenu
        visible: false
        anchors.right: root.right
        anchors.top: root.top
        anchors.topMargin: 30
        property int buttonHeight: 30
        property int buttonWidth: 180
        opacity: visible ? 1.0 : 0
        z: 1000

        Behavior on opacity{ NumberAnimation{ duration: 200 } }

        Rectangle{
            id: addHorizontalPaneButton
            anchors.top: parent.top
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: addHorizontalPaneText
                text: qsTr("Split horizontally")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: addHorizontalPaneArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : addHorizontalPaneArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    paneMenu.visible = false
                    var clone = root.paneClone()
                    var index = root.parentSplitViewIndex()
                    root.panes.splitPaneHorizontallyWith(root.parentSplitView, index, clone)
                }
            }
        }

        Rectangle{
            id: addVerticalPaneButton
            anchors.top: addHorizontalPaneButton.bottom
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: addVerticalPaneText
                text: qsTr("Split vertically")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: addVerticalPaneArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : addVerticalPaneArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    paneMenu.visible = false
                    var clone = root.paneClone()
                    var index = root.parentSplitViewIndex()
                    root.panes.splitPaneVerticallyWith(root.parentSplitView, index, clone)
                }
            }
        }

        Rectangle{
            id: newWindowPaneButton
            anchors.top: addVerticalPaneButton.bottom
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: newWindowPaneText
                text: qsTr("Move to new window")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: newWindowPaneArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : newWindowPaneArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    paneMenu.visible = false
                    root.panes.movePaneToNewWindow(root)
                }
            }
        }

        Rectangle{
            id: removePaneButton
            anchors.top: newWindowPaneButton.bottom
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: removePaneText
                text: qsTr("Remove Pane")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: removePaneArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : removePaneArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    paneMenu.visible = false
                    root.panes.removePane(root)
                }
            }
        }
    }

    Item{
        id: viewer
        anchors.top: paneHeader.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    RunnablesMenu{
        id: runnablesMenu
        anchors.top: paneHeader.bottom
        anchors.left: parent.left
        anchors.leftMargin: 15
        onRunnableSelected: {
            if (root.currentRunnable){
                root.currentRunnable.setRunSpace(null)
            }

            var r = lk.layers.workspace.project.openRunnable(path)
            if (r){
                if ( !r.runSpace() )
                    r.setRunSpace(viewer)
                root.currentRunnable = r
            }
        }
    }

}
