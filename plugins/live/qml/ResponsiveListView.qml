import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import base 1.0
import editor.private 1.0

Item{
    id: root

    width: 180
    height: nestedList.contentHeight > maxHeight ? maxHeight : nestedList.contentHeight + 5

    property color selectionColor: "#091927"
    property alias suggestionCount : nestedList.count
    property alias model : nestedList.model
    property alias delegate: nestedList.delegate
    property alias currentIndex : nestedList.currentIndex
    property string fontFamily: 'Open Sans'
    property int maxHeight: 160
    property int fontSize: 12
    property int smallFontSize: 9

    signal itemSelected(int index)
    signal cancelled()

    function highlightNext(){
        if ( nestedList.currentIndex + 1 <  nestedList.count ){
            nestedList.currentIndex++;
        } else {
            nestedList.currentIndex = 0;
        }
    }
    function highlightPrev(){
        if ( nestedList.currentIndex > 0 ){
            nestedList.currentIndex--;
        } else {
            nestedList.currentIndex = nestedList.count - 1;
        }
    }

    function highlightNextPage(){
        var noItems = Math.floor(nestedList.height / 25)
        if ( nestedList.currentIndex + noItems < nestedList.count ){
            nestedList.currentIndex += noItems;
        } else {
            nestedList.currentIndex = nestedList.count - 1;
        }
    }
    function highlightPrevPage(){
        var noItems = Math.floor(nestedList.height / 25)
        if ( nestedList.currentIndex - noItems >= 0 ){
            nestedList.currentIndex -= noItems;
        } else {
            nestedList.currentIndex = 0;
        }
    }

    focus: false

    Keys.onPressed: {
        if ( event.key === Qt.Key_PageUp ){
            highlightPrevPage()
            event.accepted = true
        } else if ( event.key === Qt.Key_PageDown ){
            highlightNextPage()
            event.accepted = true
        } else if ( event.key === Qt.Key_Down ){
            highlightNext()
            event.accepted = true
        } else if ( event.key === Qt.Key_Up ){
            highlightPrev()
            event.accepted = true
        } else if ( event.key === Qt.Key_Return || event.key === Qt.Key_Enter ){
            root.paletteSelected(nestedList.currentIndex)
            event.accepted = true
        } else if ( event.key === Qt.Key_Escape ){
            root.cancelled()
            event.accepted = true
        }
    }

    ScrollView{
        anchors.top : parent.top

        height : root.height
        width: root.width

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
                    color: 'transparent'
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Rectangle{color: "transparent"}
            corner: Rectangle{color: "transparent"}
        }

        ListView{
            id : nestedList
            anchors.fill: parent
            anchors.rightMargin: 2
            anchors.bottomMargin: 5
            anchors.topMargin: 0
            visible: true
            opacity: root.opacity

            currentIndex: 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            delegate: Component{

                Rectangle{
                    width : nestedList.width
                    height : 25
                    color : ListView.isCurrentItem ? root.selectionColor : "transparent"
                    Text{
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        font.family: root.fontFamily
                        font.pixelSize: root.fontSize

                        color: "#fafafa"
                        text: modelData
                    }

                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: nestedList.currentIndex = index
                        onClicked: root.itemSelected(index)
                    }
                }
            }

        }
    }
}

