import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import base 1.0
import editor.private 1.0

Rectangle{
    id: root

    width: 180
    height: paletteList.contentHeight > maxHeight ? maxHeight : paletteList.contentHeight + 5
    color: "#03070a"
    opacity: 0.95

    property color selectionColor: "#091927"
    property alias suggestionCount : paletteList.count
    property alias model : paletteList.model
    property string fontFamily: 'Courier New, Courier'
    property int maxHeight: 160
    property int fontSize: 12
    property int smallFontSize: 9

    signal paletteSelected(int index)
    signal cancelled()

    function highlightNext(){
        if ( paletteList.currentIndex + 1 <  paletteList.count ){
            paletteList.currentIndex++;
        } else {
            paletteList.currentIndex = 0;
        }
    }
    function highlightPrev(){
        if ( paletteList.currentIndex > 0 ){
            paletteList.currentIndex--;
        } else {
            paletteList.currentIndex = paletteList.count - 1;
        }
    }

    function highlightNextPage(){
        var noItems = Math.floor(paletteList.height / 25)
        if ( paletteList.currentIndex + noItems < paletteList.count ){
            paletteList.currentIndex += noItems;
        } else {
            paletteList.currentIndex = paletteList.count - 1;
        }
    }
    function highlightPrevPage(){
        var noItems = Math.floor(paletteList.height / 25)
        if ( paletteList.currentIndex - noItems >= 0 ){
            paletteList.currentIndex -= noItems;
        } else {
            paletteList.currentIndex = 0;
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
            root.paletteSelected(paletteList.currentIndex)
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
                    color: root.color
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Rectangle{color: "transparent"}
            corner: Rectangle{color: root.color}
        }

        ListView{
            id : paletteList
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
                    width : paletteList.width
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
                        text: model.name
                    }

                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: paletteList.currentIndex = index
                        onClicked: root.paletteSelected(index)
                    }
                }
            }

        }
    }
}

