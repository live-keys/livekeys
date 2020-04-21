import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import base 1.0
import timeline 1.0

Rectangle{
    id: root

    width: 180
    height: segmentTypeList.contentHeight > maxHeight ? maxHeight : segmentTypeList.contentHeight + 5
    color: "#03070a"
    opacity: 0.95

    property color selectionColor: "#091927"
    property alias suggestionCount : segmentTypeList.count

    property var segmentSelection : {
        return {}
    }

    property Track currentTrack : null
    visible: currentTrack ? true : false

    property alias model : segmentTypeList.model
    property string fontFamily: 'Courier New, Courier'
    property int maxHeight: 160
    property int fontSize: 12
    property int smallFontSize: 9

    signal segmentSelected(int index)
    signal cancelled()

    function highlightNext(){
        if ( segmentTypeList.currentIndex + 1 <  segmentTypeList.count ){
            segmentTypeList.currentIndex++;
        } else {
            segmentTypeList.currentIndex = 0;
        }
    }
    function highlightPrev(){
        if ( segmentTypeList.currentIndex > 0 ){
            segmentTypeList.currentIndex--;
        } else {
            segmentTypeList.currentIndex = segmentTypeList.count - 1;
        }
    }

    function highlightNextPage(){
        var noItems = Math.floor(segmentTypeList.height / 25)
        if ( segmentTypeList.currentIndex + noItems < segmentTypeList.count ){
            segmentTypeList.currentIndex += noItems;
        } else {
            segmentTypeList.currentIndex = segmentTypeList.count - 1;
        }
    }
    function highlightPrevPage(){
        var noItems = Math.floor(segmentTypeList.height / 25)
        if ( segmentTypeList.currentIndex - noItems >= 0 ){
            segmentTypeList.currentIndex -= noItems;
        } else {
            segmentTypeList.currentIndex = 0;
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
            root.segmentSelected(segmentTypeList.currentIndex)
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
            id : segmentTypeList
            anchors.fill: parent
            anchors.rightMargin: 2
            anchors.bottomMargin: 5
            anchors.topMargin: 0
            visible: true
            opacity: root.opacity

            model: ['asd', 'asd', '123']

            currentIndex: 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            delegate: Component{

                Rectangle{
                    width : segmentTypeList.width
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
                        onEntered: segmentTypeList.currentIndex = index
                        onClicked: root.segmentSelected(index)
                    }
                }
            }

        }
    }
}

