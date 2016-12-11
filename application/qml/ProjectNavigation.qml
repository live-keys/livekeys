import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import Cv 1.0

Rectangle{
    id: root
    color: "#000"
    visible: false
    opacity: 0.7

    onVisibleChanged: {
        if ( visible ){
            show()
        } else {
            close()
            cancel()
        }

    }

    signal cancel()
    signal open(string path)

    function show(){
        navigationInput.forceActiveFocus()
    }
    function close(){
        documentView.currentIndex = 0
        navigationInput.text = ''
        visible = false
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            root.cancel()
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

    Rectangle{
        id: navigationInputBox
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        color: "#333"
        height: 36

        border.color: "#666"
        border.width: 1

        TextInput{
            id : navigationInput
            anchors.fill: parent
            anchors.margins: 10

            color : "#fff"
            font.family: "Source Code Pro, Ubuntu Mono, Courier New, Courier"
            font.pixelSize: 13
            font.weight: Font.Light

            selectByMouse: true

            text: ""
            onTextChanged: {
                project.navigationModel.setFilter(text)
            }

            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: Qt.IBeamCursor
            }

            Keys.onPressed: {
                if ( event.key === Qt.Key_Down ){
                    documentView.highlightNext()
                    event.accepted = true
                } else if ( event.key === Qt.Key_Up ){
                    documentView.highlightPrev()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageDown ){
                    documentView.highlightNextPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageUp ){
                    documentView.highlightPrevPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_K && ( event.modifiers & Qt.ControlModifier) ){
                    root.close()
                    root.cancel()
                    event.accepted = true
                }
            }
            Keys.onReturnPressed: {
                root.open(documentView.currentItem.path)
                root.close()
                event.accepted = true
            }
            Keys.onEscapePressed: {
                root.close()
                root.cancel()
                event.accepted = true
            }
        }
    }

    ScrollView{
        width: parent.width
        height: parent.height - 35
        anchors.top: parent.top
        anchors.topMargin: navigationInputBox.height

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
            id: documentView
            model : navigationInput.text === '' ? project.documentModel : project.navigationModel
            width: parent.width
            height: parent.height
            clip: true
            currentIndex : 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            property int delegateHeight : 40

            function highlightNext(){
                if ( documentView.currentIndex + 1 <  documentView.count ){
                    documentView.currentIndex++;
                } else {
                    documentView.currentIndex = 0;
                }
            }
            function highlightPrev(){
                if ( documentView.currentIndex > 0 ){
                    documentView.currentIndex--;
                } else {
                    documentView.currentIndex = documentView.count - 1;
                }
            }

            function highlightNextPage(){
                var noItems = Math.floor(documentView.height / documentView.delegateHeight)
                if ( documentView.currentIndex + noItems < documentView.count ){
                    documentView.currentIndex += noItems;
                } else {
                    documentView.currentIndex = pluginList.count - 1;
                }
            }
            function highlightPrevPage(){
                var noItems = Math.floor(documentView.height / documentView.delegateHeight)
                if ( documentView.currentIndex - noItems >= 0 ){
                    documentView.currentIndex -= noItems;
                } else {
                    documentView.currentIndex = 0;
                }
            }

            delegate: Rectangle{

                property string path: model.path

                color: ListView.isCurrentItem ? "#224422" : "#332222"
                width: root.width
                height: documentView.delegateHeight
                Text{
                    text: model.name
                    color: "#fff"

                    font.family: "Open Sans, sans-serif"
                    font.pixelSize: 13
                    font.weight: model.isOpen ? Font.Bold : Font.Light
                }
                Text{
                    anchors.top: parent.top
                    anchors.topMargin: 20

                    text: model.path
                    color: "#aaa"

                    font.family: "Open Sans, sans-serif"
                    font.pixelSize: 13
                    font.weight: Font.Light
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.open(path)
                        root.close()
                    }
                }
            }
        }
    }

    Rectangle{
        anchors.centerIn: parent
        color: "#003300"
        width: 100
        height: 100
        visible: project.navigationModel.isIndexing
    }

}
