import QtQuick 2.3
import QtQuick.Controls 2.2
import workspace.icons 1.0 as Icons
import visual.shapes 1.0
import visual.input 1.0 as Input

ScrollView{
    id: connectionListScroll

    contentWidth: controlledWidth

    property alias count: connectionList.count
    property alias model: connectionList.model
    property alias currentIndex: connectionList.currentIndex

    property alias listHeight: connectionList.contentHeight

    property double controlledWidth: connectionList.contentWidth

    property color iconColor: 'white'
    property color backgroundColor: '#000'
    property color connectionRunnableColor: '#111'
    property color selectionColor: '#999'
    property QtObject labelStyle: Input.TextStyle{}
    property QtObject fileIcon: null
    property QtObject componentIcon: null
    property QtObject watcherIcon: null
    property QtObject stackIcon: null


    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    ScrollBar.vertical.contentItem: Input.ScrollbarHandle{
        color: "#1f2227"
        visible: connectionListScroll.contentHeight > connectionListScroll.height
    }
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
    ScrollBar.horizontal.contentItem: Input.ScrollbarHandle{
        color: "#1f2227"
        visible: connectionListScroll.contentWidth > connectionListScroll.width
    }

    ListView{
        id : connectionList
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        visible: true

        currentIndex: 0
        onCountChanged: currentIndex = 0

        boundsBehavior : Flickable.StopAtBounds
        highlightMoveDuration: 100

        delegate: Component{

            Item{
                id: connectionView
                width: labelBox.width + 20
                height : 25

                property var modelPath : model.path
                property bool isEnabled: connectionList.model.selectedIndex === index
                property bool isSync: model.isInSync

                Component.onCompleted: {
                    if ( width > connectionListScroll.controlledWidth )
                        connectionListScroll.controlledWidth = width
                }

                Rectangle{
                    id: labelBox
                    width: label.width + 25
                    height: parent.height - 2
                    color: connectionListScroll.connectionRunnableColor
                    anchors.verticalCenter: parent.verticalCenter

                    Input.Label{
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        textStyle: connectionListScroll.labelStyle
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
                    width: bindingPathView.width
                    clip: true
                    anchors.left: labelBox.right
                    anchors.leftMargin: 30

                    BindingPathView{
                        id: bindingPathView
                        model: connectionView.modelPath
                        iconColor: connectionListScroll.iconColor
                        textStyle: connectionListScroll.labelStyle
                        fileIcon: connectionListScroll.fileIcon
                        componentIcon: connectionListScroll.componentIcon
                        watcherIcon: connectionListScroll.watcherIcon
                        stackIcon: connectionListScroll.stackIcon
                    }
                }

                Rectangle{
                    height: 20
                    width: 20
                    color: checkArea.containsMouse && !isEnabled ? '#141b20' : 'transparent'
                    visible: connectionView.isSync
                    border.width: 1
                    border.color: connectionListScroll.connectionRunnableColor
                    anchors.top: parent.top
                    anchors.topMargin: 3
                    anchors.left: parent.left
                    anchors.leftMargin: connectionList.width - width - 10 + connectionListScroll.contentX

                    property bool isEnabled: parent.isEnabled

                    Icons.CheckMarkIcon{
                        width: 10
                        height: 10
                        strokeWidth: 1.5
                        anchors.centerIn: parent
                        color: connectionListScroll.labelStyle.color
                        visible: parent.isEnabled
                    }

                    MouseArea{
                        id: checkArea
                        width: 20
                        height: parent.height
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        hoverEnabled: true
                        onClicked: {
                            connectionList.model.selectChannel(index)
                        }
                    }
                }


                Rectangle{
                    height: 1
                    width: connectionList.width > connectionListScroll.controlledWidth  ? connectionList.width - 10 - anchors.leftMargin : connectionListScroll.controlledWidth - 10 - anchors.leftMargin
                    anchors.left: parent.left
                    anchors.leftMargin: labelBox.width + 5
                    anchors.bottom: parent.bottom
                    color: ListView.isCurrentItem ? connectionListScroll.selectionColor : "#24282e"
                }

            }
        }
    }
}
