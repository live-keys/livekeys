import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle{
    width: 1040
    height: 660
    color: "#141517"
    border.color: "#2d2d2e"

    property bool firstTime: !lk.layers.workspace.wasRecentsFileFound() && !noThanks
    property bool noThanks: false
    Item {
        id: closeButton
        x: 1000
        y: 10
        width: 25
        height: 25

        Text {
            text: "x"
            font.family: "Source Code Pro"
            font.pixelSize: 25
            color: "white"
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                lk.layers.workspace.panes.removeStartupBox()
            }
        }
    }

    Item {
        id: header
        width: parent.width
        x: 0
        y: 40

        Image {
            id: logoImage
            source: "qrc:/images/logo.png"
            x: 50
            y: 10
        }

        Text{
            color: "#f1f1f1"
            text: "WELCOME"
            x: 110
            y: 0
            font.family: "Source Code Pro"
            font.pixelSize: 30
            font.letterSpacing: 0.3
        }

        Item {
            x: 400
            y: 5
            visible: !firstTime
            width: 170
            height: 40

            MouseArea{
                id : newMArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: lk.layers.workspace.project.newProject()
            }

            Image {
                x: 15
                id: newProjectImage
                source: "qrc:/images/top-icon-new.png"
                y: 5
            }

            Text{
                x: 45
                y: 0
                color: "#f1f1f1"
                text: "New Project"
                font.family: "Open Sans"
                font.pixelSize: 18
                font.weight: Font.Light
            }


            Rectangle {
                width: parent.width
                height: 5
                color: !newMArea.containsMouse ? "#2b2b2b" : "#2b2b8b"
                anchors.bottom: parent.bottom
            }
        }

        Item {
            x: 600
            y: 5
            visible: !firstTime
            width: 180
            height : 40
            MouseArea{
                id : openProjectMArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: lk.layers.workspace.project.openProject()
            }

            Image {
                x: 15
                id: openFolderImage
                source: "qrc:/images/top-icon-openproject.png"
                y: 2
            }

            Text{
                x: 50
                y: 0
                color: "#f1f1f1"
                text: "Open Folder"
                font.family: "Open Sans"
                font.pixelSize: 18
                font.weight: Font.Light
            }


            Rectangle {
                width: parent.width
                height: 5
                color: !openProjectMArea.containsMouse ? "#2b2b2b" : "#2b2b8b"
                anchors.bottom: parent.bottom
            }
        }

        Item {
            x: 810
            y: 5
            visible: !firstTime
            width: 150
            height: 40

            Behavior on height{ NumberAnimation{  duration: 100 } }
            MouseArea{
                id : openMArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: lk.layers.workspace.project.openFileDialog()
            }

            Image {
                x: 15
                id: openFileImage
                source: "qrc:/images/top-icon-openfile.png"
                y: 2
            }

            Text{
                x: 50
                y: 0
                color: "#f1f1f1"
                text: "Open File"
                font.family: "Open Sans"
                font.pixelSize: 18
                font.weight: Font.Light
            }


            Rectangle {
                width: parent.width
                height: 5
                color: !openMArea.containsMouse ? "#2b2b2b" : "#2b2b8b"
                anchors.bottom: parent.bottom
            }
        }
    }


    Item {
        id: panesWrapper
        x: 40
        y: 180
        visible: !firstTime
        width: 960
        height: 430

        Rectangle {
            id: recentProjectsPane
            width: 280
            height: parent.height
            visible: recentView.model.rowCount() > 0
            Text {
                text: "Recent Projects"
                color: "#fff"
                font.family: "Open Sans"
                font.pixelSize: 18
                font.weight: Font.Bold
                font.letterSpacing: 0.02
            }

            ScrollView{
                anchors.top: parent.top
                anchors.topMargin: 60
                width: parent.width
                height: recentView.count * recentView.delegateHeight > parent.height - anchors.topMargin
                        ? parent.height - anchors.topMargin
                        : recentView.count * recentView.delegateHeight

                style: ScrollViewStyle {
                    transientScrollBars: false
                    handle: Item {
                        implicitWidth: 6
                        implicitHeight: 6
                        Rectangle {
                            color: "#3c3c3d"
                            anchors.fill: parent
                        }
                    }
                    scrollBarBackground: Item{
                        implicitWidth: 6
                        implicitHeight: 6
                        Rectangle{
                            anchors.left: parent.left
                            anchors.leftMargin: 3
                            width: 1
                            height: parent.height
                            color: "#3c3c3d"
                        }
                    }
                    decrementControl: null
                    incrementControl: null
                    frame: Item{}
                    corner: Rectangle{color: "#3c3c3d"}
                }



                ListView{
                    id: recentView
                    model: lk.layers.workspace.recents
                    width: parent.width - 30
                    height: parent.height
                    clip: true
                    currentIndex : 0
                    onCountChanged: currentIndex = 0

                    boundsBehavior : Flickable.StopAtBounds
                    highlightMoveDuration: 100

                    property int delegateHeight: 60

                    delegate: Rectangle {
                        width: 260
                        height: recentView.delegateHeight
                        color: recentViewMA.containsMouse ? "#2b2d31" : "transparent"

                        MouseArea {
                            id: recentViewMA
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                project.openProject(model.path)
                            }
                        }

                        Text {
                            height: 25
                            width: parent.width
                            anchors.top: parent.top
                            text: model.label
                            clip: true
                            elide: Text.ElideRight
                            font.family: "Open Sans"
                            font.pixelSize: 16
                            font.weight: Font.Light
                            color: "#00cbfe"
                        }

                        Text {
                            height: 22
                            width: parent.width
                            anchors.top: parent.top
                            anchors.topMargin: 27
                            text: model.path
                            clip: true
                            elide: Text.ElideRight
                            font.family: "Open Sans"
                            font.pixelSize: 14
                            font.weight: Font.Light
                            color: "#6d6d6d"
                        }

                    }

                }
            }

            color: "transparent"

        }

        Rectangle {
            id: learnPane
            x: recentProjectsPane.visible ? 340 : 0
            width: 280
            height: parent.height

            Text {
                text: "Learn"
                color: "#fff"
                font.family: "Open Sans"
                font.pixelSize: 18
                font.weight: Font.Bold
                font.letterSpacing: 0.02
            }

            ScrollView{
                anchors.top: parent.top
                anchors.topMargin: 50
                width: parent.width
                height: learnView.count * learnView.delegateHeight + learnView.model.numberOfTitles * 15 > parent.height - anchors.topMargin
                        ? parent.height - anchors.topMargin
                        : learnView.count * learnView.delegateHeight + learnView.model.numberOfTitles * 15

                style: ScrollViewStyle {
                    transientScrollBars: false
                    handle: Item {
                        implicitWidth: 6
                        implicitHeight: 6
                        Rectangle {
                            color: "#3c3c3d"
                            anchors.fill: parent
                        }
                    }
                    scrollBarBackground: Item{
                        implicitWidth: 6
                        implicitHeight: 6
                        Rectangle{
                            anchors.left: parent.left
                            anchors.leftMargin: 3
                            width: 1
                            height: parent.height
                            color: "#3c3c3d"
                        }
                    }
                    decrementControl: null
                    incrementControl: null
                    frame: Item{}
                    corner: Rectangle{color: "#3c3c3d"}
                }



                ListView{
                    id: learnView
                    model: lk.layers.workspace.tutorials
                    width: parent.width - 30
                    height: parent.height
                    clip: true
                    currentIndex : 0
                    onCountChanged: currentIndex = 0

                    boundsBehavior : Flickable.StopAtBounds
                    highlightMoveDuration: 100

                    property int delegateHeight: 35

                    delegate: Rectangle {
                        width: 240
                        height: learnView.delegateHeight + (model.isGroupTitle ? 15 : 0)
                        color: learnViewMA.containsMouse && !model.isGroupTitle ? "#2b2d31" : "transparent"

                        MouseArea {
                            id: learnViewMA
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                if (model.isGroupTitle) return
                                project.openProject(model.path)
                            }
                        }

                        Text {
                            width: parent.width - anchors.leftMargin
                            anchors.verticalCenter: parent.verticalCenter

                            anchors.left: parent.left
                            anchors.leftMargin: model.isGroupTitle? 0 : 15
                            text: model.label
                            clip: true
                            elide: Text.ElideRight
                            font.family: "Open Sans"
                            font.pixelSize: 16
                            font.weight: model.isGroupTitle ? Font.Bold : Font.Light
                            color: model.isGroupTitle ? "#cb6000" : "white"
                        }
                    }

                }
            }


            color: "transparent"
        }

        Rectangle {
            id: samplesPane
            x: recentProjectsPane.visible ? 680 : 340
            width: 280
            height: parent.height

            Text {
                text: "Samples"
                color: "#fff"
                font.family: "Open Sans"
                font.pixelSize: 18
                font.weight: Font.Bold
                font.letterSpacing: 0.02
            }

            ScrollView{
                anchors.top: parent.top
                anchors.topMargin: 50
                width: parent.width
                height: samplesView.count * samplesView.delegateHeight + samplesView.model.numberOfTitles * 15 > parent.height - anchors.topMargin
                        ? parent.height - anchors.topMargin
                        : samplesView.count * samplesView.delegateHeight + samplesView.model.numberOfTitles * 15

                style: ScrollViewStyle {
                    transientScrollBars: false
                    handle: Item {
                        implicitWidth: 6
                        implicitHeight: 6
                        Rectangle {
                            color: "#3c3c3d"
                            anchors.fill: parent
                        }
                    }
                    scrollBarBackground: Item{
                        implicitWidth: 6
                        implicitHeight: 6
                        Rectangle{
                            anchors.left: parent.left
                            anchors.leftMargin: 3
                            width: 1
                            height: parent.height
                            color: "#3c3c3d"
                        }
                    }
                    decrementControl: null
                    incrementControl: null
                    frame: Item{}
                    corner: Rectangle{color: "#3c3c3d"}
                }

                ListView{
                    id: samplesView
                    model: lk.layers.workspace.samples

                    width: parent.width - 30
                    height: parent.height
                    clip: true
                    currentIndex : 0
                    onCountChanged: currentIndex = 0

                    boundsBehavior : Flickable.StopAtBounds
                    highlightMoveDuration: 100

                    property int delegateHeight: 35

                    delegate: Rectangle {

                        width: 260
                        height: samplesView.delegateHeight + (model.isGroupTitle ? 15 : 0)
                        color: samplesViewMA.containsMouse && !model.isGroupTitle ? "#2b2d31" : "transparent"

                        MouseArea {
                            id: samplesViewMA
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                if (model.isGroupTitle)
                                    return
                                project.openProject(model.path)
                            }
                        }

                        Text {
                            width: parent.width - anchors.leftMargin
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: model.isGroupTitle? 0 : 15
                            text: model.name
                            clip: true
                            elide: Text.ElideRight
                            font.family: "Open Sans"
                            font.pixelSize: 16
                            font.weight: model.isGroupTitle ? Font.Bold : Font.Light
                            color: model.isGroupTitle ? "#5d59f7" : "white"
                        }
                    }

                }
            }


            color: "transparent"

        }
    }

    Item {
        id: firstTimeScreen
        x: 50
        y: 120
        visible: firstTime
        Rectangle {

            width: 940
            height: 480
            color: "#1d1e22"

            Text {
                text: "Looks like it's your first time starting Livekeys.\nWould you like a quick tour?"
                x: 60
                y: 60
                font.family: "Open Sans"
                font.pixelSize: 24
                color: "#fff"
            }

            Rectangle {
                id: button1
                x: 55
                y: 160
                width: 750
                height: 70

                Text {
                    x: 30
                    anchors.verticalCenter: parent.verticalCenter

                    text: "Yes, take me to the tour"
                    font.family: "Open Sans"
                    font.pixelSize: 24
                    color: "#fff"
                }

                MouseArea{
                    id : button1MA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        project.openProject(lk.layers.workspace.pluginsPath() + "/squareone/tutorials/workspace")
                    }
                }

                color: button1MA.containsMouse ? "#383c44" : "#2a303c"
            }

            Rectangle {
                id: button2
                x: 55
                y: 250
                width: 750
                height: 70

                Text {
                    x: 30
                    anchors.verticalCenter: parent.verticalCenter

                    text: "No, thanks"
                    font.family: "Open Sans"
                    font.pixelSize: 24
                    color: "#fff"
                }

                MouseArea{
                    id : button2MA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        noThanks = true
                    }
                }

                color: button2MA.containsMouse ? "#383c44" : "#2a303c"
            }

            Text {
                text: "You can also open up the tour later by going to the\nwelcome screen > learn > quickstart > workspace"
                x: 60
                y: 350
                font.family: "Open Sans"
                font.pixelSize: 18
                color: "#959595"
            }
        }


    }
}
