import QtQuick 2.3
import QtQuick.Controls 2.2
import visual.input 1.0 as Input

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

    signal runnableSelected(string path)

    width: 220
    height: 400
    z: 400

    function show(){
        runnablesMenuInput.forceActiveFocus()
    }

    function close(){
        runnablesView.currentIndex = 0
        runnablesMenuInput.text = ''
        visible = false
    }

    Rectangle{
        id: runnablesMenuInputBox
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: 30

        color: "#05090e"

        border.color: "#0c1117"
        border.width: 1

        TextInput{
            id : runnablesMenuInput
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
                lk.layers.workspace.project.runnables.setFilter(text)
            }

            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: Qt.IBeamCursor
            }

            z: 300
            Keys.onPressed: {
                if ( event.key === Qt.Key_Down ){
                    runnablesView.highlightNext()
                    event.accepted = true
                } else if ( event.key === Qt.Key_Up ){
                    runnablesView.highlightPrev()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageDown ){
                    runnablesView.highlightNextPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageUp ){
                    runnablesView.highlightPrevPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_K && ( event.modifiers & Qt.ControlModifier) ){
                    root.close()
                    root.cancel()
                    event.accepted = true
                }
            }
            Keys.onReturnPressed: {
                if (runnablesView.currentItem) {
                    root.runnableSelected(runnablesView.currentItem.path)
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

    ScrollView{
        id: scrollView
        z: 300
        width: parent.width
        height: runnablesView.count * runnablesView.delegateHeight > parent.height - runnablesMenuInputBox.height ?
                    parent.height - runnablesMenuInputBox.height :
                    runnablesView.count * runnablesView.delegateHeight
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: runnablesMenuInputBox.height

        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.contentItem: Input.ScrollbarHandle{
            color: "#1f2227"
            visible: scrollView.contentHeight > scrollView.height
        }
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.horizontal.contentItem: Input.ScrollbarHandle{
            color: "#1f2227"
            visible: scrollView.contentWidth > scrollView.width
        }

        ListView{
            id: runnablesView
            model : lk.layers.workspace.project.runnables
            width: parent.width
            height: parent.height
            clip: true
            currentIndex : 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            property int delegateHeight : 20

            function highlightNext(){
                if ( runnablesView.currentIndex + 1 <  runnablesView.count ){
                    runnablesView.currentIndex++;
                } else {
                    runnablesView.currentIndex = 0;
                }
            }

            function highlightPrev(){
                if ( runnablesView.currentIndex > 0 ){
                    runnablesView.currentIndex--;
                } else {
                    runnablesView.currentIndex = runnablesView.count - 1;
                }
            }

            function highlightNextPage(){
                var noItems = Math.floor(runnablesView.height / runnablesView.delegateHeight)
                if ( runnablesView.currentIndex + noItems < runnablesView.count ){
                    runnablesView.currentIndex += noItems;
                } else {
                    runnablesView.currentIndex = runnablesView.count - 1;
                }
            }
            function highlightPrevPage(){
                var noItems = Math.floor(runnablesView.height / runnablesView.delegateHeight)
                if ( runnablesView.currentIndex - noItems >= 0 ){
                    runnablesView.currentIndex -= noItems;
                } else {
                    runnablesView.currentIndex = 0;
                }
            }

            delegate: Rectangle{
                z: 400
                color: ListView.isCurrentItem ? "#091927" : "#0b0e11"
                width: root.width
                height: runnablesView.delegateHeight

                property string path : model.path

                Text{
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    text: model.name
                    color: "#ebebeb"

                    font.family: "Open Sans, sans-serif"
                    font.pixelSize: 12
                    font.weight: Font.Light
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.runnableSelected(model.path)
                        root.close()
                    }
                }
            }
        }
    }

    Rectangle{
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: runnablesMenuInputBox.height
        visible: !runnablesView.currentItem
        z: 400
        color: "#0a131a"
        width: root.width
        height: runnablesView.delegateHeight
        Text{
            anchors.left: parent.left
            anchors.leftMargin: 20
            text: "No runnables available."
            color: "#ebebeb"

            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            font.weight: Font.Light
            font.italic: true
        }
    }
}
