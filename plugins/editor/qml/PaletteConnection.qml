import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import base 1.0
import live 1.0
import editor 1.0
import editor.private 1.0
import workspace.icons 1.0 as Icons
import visual.shapes 1.0
import visual.input 1.0 as Input

Rectangle{
    id: root

    width: 400
    height: connectionList.contentHeight + 80 > maxHeight
        ? maxHeight
        : connectionList.contentHeight + 80
    color:  theme.colorScheme.backgroundOverlay
    border.width: 1
    border.color: theme.colorScheme.middlegroundOverlayBorder
    z: 3000

    focus: false

    property QtObject theme: lk.layers.workspace.themes.current

    property color connectionRunnableColor: theme.colorScheme.middlegroundOverlay
    property color selectionColor: theme.colorScheme.middleground
    property alias connectionCount : connectionList.count
    property alias model : connectionList.model

    property QtObject editingFragment: null

    property Input.TextStyle labelStyle: theme.inputLabelStyle.textStyle
    property Input.TextStyle smallLabelStyle: theme.smallLabelStyle

    property int maxHeight: 160

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

    Rectangle{
        id: componentLabelBox
        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.left: parent.left
        anchors.leftMargin: 2
        height: 23
        width: parent.width - 5
        color: root.theme.colorScheme.middleground
        Input.Label{
            id: componentLabel
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 15
            textStyle: root.labelStyle
            text: root.editingFragment ? root.editingFragment.typeName() : ''
        }
    }

    Item{
        height: parent.height
        width: parent.width - 30

        anchors.top: parent.top
        anchors.topMargin: 27
        anchors.left: parent.left
        anchors.leftMargin: 17
        clip: true

        Row{
            id: modelSplitterRow
            spacing: 5
            Repeater{
                id: modelSplitter
                model: editingFragment ? editingFragment.bindingPath() : []

                Item{
                    anchors.top: parent.top
                    height: 25
                    width: pathIconLoader.width + pathText.width + pathSplitter.width + (pathIconLoader.visible ? 18 : 8 )

                    property int type: 0

                    Loader{
                        id: pathIconLoader
                        anchors.verticalCenter: parent.verticalCenter

                        sourceComponent: {
                            if ( modelData[0] === 'F' )
                                return theme.fileIcon
                            else if ( modelData[0] === 'C' )
                                return theme.componentIcon
                            else if ( modelData[0] === 'W' ){
                                return theme.watcherIcon
                            } else if ( modelData[0] === 'I' ){
                                return theme.stackIcon
                            }

                            return null
                        }
                        onItemChanged: {
                            if ( item.width > 12 )
                                item.width = 12
                            if ( item.height > 12 )
                                item.height = 12
                            if ( item.color )
                                item.color = theme.colorScheme.foregroundFaded
                        }
                    }

                    Input.Label{
                        id: pathText
                        anchors.left: pathIconLoader.right
                        anchors.leftMargin: pathIconLoader.visible ? 8 : 0
                        anchors.verticalCenter: parent.verticalCenter
                        textStyle: root.labelStyle
                        text: modelData.substr(2)
                    }

                    Text{
                        id: pathSplitter
                        anchors.verticalCenter: parent.verticalCenter
                        font.family: 'Open Sans'
                        font.pixelSize: root.labelStyle.fontSize + 2
                        font.weight: Font.Bold
                        color: root.labelStyle.color
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
        id: fileLabelBox
        anchors.top: parent.top
        anchors.topMargin: 52
        anchors.left: parent.left
        anchors.leftMargin: 2
        height: 23
        width: parent.width - 5
        color: root.theme.colorScheme.middleground
        Input.Label{
            id: fileLabel
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 15
            textStyle: root.labelStyle
            text: connectionList.model && connectionList.model.document() ? connectionList.model.document().file.name : ''
        }
    }

    ScrollView{
        id: connectionListScroll
        anchors.top : parent.top
        anchors.topMargin: 2 + 75
        anchors.left: parent.left
        anchors.leftMargin: 2

        height : root.height - 79
        width: root.width - 4
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
                        color: root.connectionRunnableColor
                        anchors.verticalCenter: parent.verticalCenter

                        Input.Label{
                            id: label
                            anchors.left: parent.left
                            anchors.leftMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                            textStyle: root.labelStyle
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
                                    width: pathIconLoader.width + pathText.width + pathSplitter.width + (pathIconLoader.visible ? 18 : 8 )

                                    property int type: 0


                                    Loader{
                                        id: pathIconLoader
                                        anchors.verticalCenter: parent.verticalCenter

                                        sourceComponent: {
                                            if ( modelData[0] === 'F' )
                                                return theme.fileIcon
                                            else if ( modelData[0] === 'C' )
                                                return theme.componentIcon
                                            else if ( modelData[0] === 'W' ){
                                                return theme.watcherIcon
                                            } else if ( modelData[0] === 'I' ){
                                                return theme.stackIcon
                                            }

                                            return ""
                                        }
                                        onItemChanged: {
                                            if ( item.width > 12 )
                                                item.width = 12
                                            if ( item.height > 12 )
                                                item.height = 12
                                            if ( item.color )
                                                item.color = theme.colorScheme.foregroundFaded
                                        }
                                    }

                                    Input.Label{
                                        id: pathText
                                        anchors.left: pathIconLoader.right
                                        anchors.leftMargin: pathIconLoader.visible ? 8 : 0
                                        anchors.verticalCenter: parent.verticalCenter
                                        textStyle: root.labelStyle
                                        text: modelData.substr(2)
                                    }

                                    Text{
                                        id: pathSplitter
                                        anchors.verticalCenter: parent.verticalCenter
                                        font.family: 'Open Sans'
                                        font.pixelSize: root.labelStyle.fontSize + 2
                                        font.weight: Font.Bold
                                        color: root.labelStyle.color
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
                        height: 20
                        width: 20
                        color: checkArea.containsMouse && !isEnabled ? '#141b20' : 'transparent'
                        visible: connectionView.isSync
                        border.width: 1
                        border.color: root.connectionRunnableColor
                        anchors.top: parent.top
                        anchors.topMargin: 3
                        anchors.left: parent.left
                        anchors.leftMargin: connectionList.width - width - 10 + connectionListScroll.flickableItem.contentX

                        property bool isEnabled: parent.isEnabled

                        Icons.CheckMarkIcon{
                            width: 10
                            height: 10
                            strokeWidth: 1.5
                            anchors.centerIn: parent
                            color: root.labelStyle.color
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
                        color: ListView.isCurrentItem ? root.selectionColor : "#24282e"
                    }

                }
            }
        }
    }
}

