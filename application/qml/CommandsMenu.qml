import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle{
    id: root
    visible: false
    color: "transparent"

    onVisibleChanged: {
        if ( visible ){
            show()
        } else {
            close()
        }

    }

    width: 400
    height: 400

    function show(){
        commandsMenuInput.forceActiveFocus()
    }

    function close(){
        commandsView.currentIndex = 0
        commandsMenuInput.text = ''
        visible = false
    }

    Rectangle{
        id: commandsMenuInputBox
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: 30

        color: "#050d13"

        border.color: "#0d1f2d"
        border.width: 1

        TextInput{
            id : commandsMenuInput
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 6
            anchors.right: parent.right
            anchors.rightMargin: 6

            clip: true

            width: parent.width

            color : "#afafaf"
            font.family: "Source Code Pro, Ubuntu Mono, Courier New, Courier"
            font.pixelSize: 12
            font.weight: Font.Light

            selectByMouse: true

            text: ""
            onTextChanged: {
                livecv.commands.model.setFilter(text)
            }

            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: Qt.IBeamCursor
            }

            z: 300
            Keys.onPressed: {
                if ( event.key === Qt.Key_Down ){
                    commandsView.highlightNext()
                    event.accepted = true
                } else if ( event.key === Qt.Key_Up ){
                    commandsView.highlightPrev()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageDown ){
                    commandsView.highlightNextPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageUp ){
                    commandsView.highlightPrevPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_K && ( event.modifiers & Qt.ControlModifier) ){
                    root.close()
                    root.cancel()
                    event.accepted = true
                }
            }
            Keys.onReturnPressed: {
                if (commandsView.currentItem) {
                    livecv.commands.execute(commandsView.currentItem.command)
                    root.close()
                }
                event.accepted = true
            }
            Keys.onEscapePressed: {
                root.close()
                event.accepted = true
            }
        }
    }

    /*Rectangle{
        z: 300
        anchors.fill: parent
        anchors.topMargin: 30
        color: "#030609"
        opacity: root.visible ? 0.92 : 0
        Behavior on opacity{ NumberAnimation{ duration: 250} }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                root.visible = false
                mouse.accepted = true;
            }
            onPressed: mouse.accepted = true;
            onReleased: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true;
            onPositionChanged: mouse.accepted = true;
            onPressAndHold: mouse.accepted = true;
            onWheel: wheel.accepted = true
        }
    }*/

    ScrollView{
        z: 300
        width: parent.width
        height: commandsView.count * commandsView.delegateHeight > parent.height - commandsMenuInputBox.height ?
                    parent.height - commandsMenuInputBox.height :
                    commandsView.count * commandsView.delegateHeight
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: commandsMenuInputBox.height

        style: ScrollViewStyle {
            transientScrollBars: false
            handle: Item {
                implicitWidth: 10
                implicitHeight: 10
                Rectangle {
                    color: "#0b1f2e"
                    anchors.fill: parent
                }
            }
            scrollBarBackground: Item{
                implicitWidth: 10
                implicitHeight: 10
                Rectangle{
                    anchors.fill: parent
                    color: "#091823"
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Rectangle{color: "transparent"}
            corner: Rectangle{color: "#091823"}
        }

        ListView{
            id: commandsView
            model : livecv.commands.model
            width: parent.width
            height: parent.height
            clip: true
            currentIndex : 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            property int delegateHeight : 20

            function highlightNext(){
                if ( commandsView.currentIndex + 1 <  commandsView.count ){
                    commandsView.currentIndex++;
                } else {
                    commandsView.currentIndex = 0;
                }
            }

            function highlightPrev(){
                if ( commandsView.currentIndex > 0 ){
                    commandsView.currentIndex--;
                } else {
                    commandsView.currentIndex = commandsView.count - 1;
                }
            }

            function highlightNextPage(){
                var noItems = Math.floor(commandsView.height / commandsView.delegateHeight)
                if ( commandsView.currentIndex + noItems < commandsView.count ){
                    commandsView.currentIndex += noItems;
                } else {
                    commandsView.currentIndex = commandsView.count - 1;
                }
            }
            function highlightPrevPage(){
                var noItems = Math.floor(commandsView.height / commandsView.delegateHeight)
                if ( commandsView.currentIndex - noItems >= 0 ){
                    commandsView.currentIndex -= noItems;
                } else {
                    commandsView.currentIndex = 0;
                }
            }


            delegate: Rectangle{

                property string command: model.command

                z: 400
                color: ListView.isCurrentItem ? "#11202d" : "#0a131a"
                width: root.width
                height: commandsView.delegateHeight
                Text{
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    text: model.description
                    color: "#ebebeb"

                    font.family: "Open Sans, sans-serif"
                    font.pixelSize: 12
                    font.weight: Font.Light
                }

                Text{
                    anchors.right: parent.right
                    anchors.rightMargin: 20

                    text: model.shortcuts
                    color: "#aaa5a5"

                    font.family: "Open Sans, sans-serif"
                    font.pixelSize: 12
                    font.weight: Font.Light
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        livecv.commands.execute(model.command)
                        root.close()
                    }
                }
            }
        }
    }

    Rectangle{
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: commandsMenuInputBox.height
        visible: !commandsView.currentItem
        z: 400
        color: "#0a131a"
        width: root.width
        height: commandsView.delegateHeight
        Text{
            anchors.left: parent.left
            anchors.leftMargin: 20
            text: "No available commands"
            color: "#ebebeb"

            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            font.weight: Font.Light
            font.italic: true
        }

    }
}
