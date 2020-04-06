import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import base 1.0
import live 1.0
import editor 1.0
import editor.private 1.0

Rectangle{
    id: root

    width: 400
    height: connectionList.contentHeight > maxHeight
        ? maxHeight
        : connectionList.contentHeight + 5
    color: "#03070a"
    border.width: 1
    border.color: "#333"
    z: 3000

    focus: false

    property color connectionRunnableColor: "#1b1e1f"
    property color selectionColor: "#094966"
    property alias connectionCount : connectionList.count
    property alias model : connectionList.model
    property string fontFamily: 'Open Sans, sans-serif'
    property int maxHeight: 160
    property int fontSize: 12
    property int smallFontSize: 9

    signal paletteSelected(int index)
    signal cancelled()

    function highlightNext(){
        if ( connectionList.currentIndex + 1 <  connectionList.count ){
            connectionList.currentIndex++;
        } else {
            connectionList.currentIndex = 0;
        }
    }
    function highlightPrev(){
        if ( connectionList.currentIndex > 0 ){
            connectionList.currentIndex--;
        } else {
            connectionList.currentIndex = connectionList.count - 1;
        }
    }

    function highlightNextPage(){
        var noItems = Math.floor(connectionList.height / 25)
        if ( connectionList.currentIndex + noItems < connectionList.count ){
            connectionList.currentIndex += noItems;
        } else {
            connectionList.currentIndex = connectionList.count - 1;
        }
    }
    function highlightPrevPage(){
        var noItems = Math.floor(connectionList.height / 25)
        if ( connectionList.currentIndex - noItems >= 0 ){
            connectionList.currentIndex -= noItems;
        } else {
            connectionList.currentIndex = 0;
        }
    }

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
            root.paletteSelected(connectionList.currentIndex)
            event.accepted = true
        } else if ( event.key === Qt.Key_Escape ){
            root.cancelled()
            event.accepted = true
        }
    }

    ScrollView{
        id: connectionListScroll
        anchors.top : parent.top

        height : root.height - 2
        width: root.width - 2
        flickableItem.contentWidth: controlledWidth

        property double controlledWidth: connectionList.contentWidth

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
            id : connectionList
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            visible: true
            opacity: root.opacity

            currentIndex: 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            delegate: Component{

                Item{
                    id: connectionView
                    width: labelBox.width + modelSplitterRow.width + 80
                    height : 25

                    property var modelPath : model.path
                    property bool isEnabled: connectionList.model.inputPathIndex === index

                    Component.onCompleted: {
                        if ( width > connectionListScroll.controlledWidth )
                            connectionListScroll.controlledWidth = width
                    }

                    Rectangle{
                        width: label.width + 25
                        height: parent.height - 5
                        color: root.connectionRunnableColor
                        anchors.verticalCenter: parent.verticalCenter
                        id: labelBox

                        Text{
                            id: label
                            anchors.left: parent.left
                            anchors.leftMargin: 15
                            anchors.verticalCenter: parent.verticalCenter

                            font.family: root.fontFamily
                            font.pixelSize: root.fontSize

                            color: "#fafafa"
                            text: model.runnableName
                        }
                    }

                    Triangle{
                        width: 10
                        color: labelBox.color
                        height: labelBox.height
                        rotation: Triangle.Right
                        anchors.left: labelBox.right
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Item{
                        height: parent.height
                        width: modelSplitterRow.width
                        clip: true
                        anchors.left: labelBox.right
                        anchors.leftMargin: 30

                        Row{
                            id: modelSplitterRow
                            spacing: 5
                            Repeater{
                                id: modelSplitter
                                model: connectionView.modelPath

                                Item{
                                    anchors.top: parent.top
                                    height: 25
                                    width: pathIcon.width + pathText.width + pathSplitter.width + (pathIcon.visible ? 18 : 8 )

                                    property int type: 0

                                    Image{
                                        id: pathIcon
                                        anchors.verticalCenter: parent.verticalCenter
                                        visible: modelData[0] === 'F' || modelData[0] === 'C'
                                        source: modelData[0] === 'F'
                                            ? "qrc:/images/palette-connection-file.png"
                                            : modelData[0] === 'C'
                                                ? "qrc:/images/palette-connection-component.png"
                                                : ""
                                    }

                                    Text{
                                        id: pathText
                                        anchors.left: pathIcon.right
                                        anchors.leftMargin: pathIcon.visible ? 8 : 0
                                        anchors.verticalCenter: parent.verticalCenter
                                        font.family: root.fontFamily
                                        font.pixelSize: root.fontSize
                                        color: "#83878b"
                                        text: modelData.substr(2)
                                    }

                                    Text{
                                        id: pathSplitter
                                        anchors.verticalCenter: parent.verticalCenter
                                        font.family: root.fontFamily
                                        font.pixelSize: root.fontSize + 2
                                        font.weight: Font.Bold
                                        color: "#83878b"
                                        anchors.left: pathText.right
                                        anchors.leftMargin: 8
                                        text: '>'
                                        visible: modelSplitter.model.length !== index + 1
                                    }
                                }
                            }
                        }
                    }

                    Rectangle{
                        height: parent.height
                        width: 25
                        color: root.color
                        anchors.top: parent.top
                        anchors.topMargin: 1
                        anchors.left: parent.left
                        anchors.leftMargin: connectionList.width - width + connectionListScroll.flickableItem.contentX

                        property bool isEnabled: parent.isEnabled

                        Rectangle{
                            width: parent.isEnabled ? 7 : 2
                            height: parent.isEnabled ? 7 : 2
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            radius: 3
                            anchors.rightMargin: parent.isEnabled ? 12 : 15
                            color: "#878e99"
                        }

                        MouseArea{
                            width: 20
                            height: parent.height
                            anchors.right: parent.right
                            anchors.rightMargin: 5
                            onClicked: {
                                connectionList.model.connectPathAtIndex(index)
                            }
                        }
                    }


                    Rectangle{
                        height: 1
                        width: connectionList.width > connectionListScroll.controlledWidth  ? connectionList.width - 10 : connectionListScroll.controlledWidth - 10
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.bottom: parent.bottom
                        color: ListView.isCurrentItem ? root.selectionColor : "#24282e"
                    }

                }
            }
        }
    }
}

