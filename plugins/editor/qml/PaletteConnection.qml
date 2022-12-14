import QtQuick 2.3
import base 1.0
import editor 1.0
import editor.private 1.0
import workspace.icons 1.0 as Icons
import visual.shapes 1.0
import visual.input 1.0 as Input

Rectangle{
    id: root

    width: 400
    height: connectionListScroll.listHeight + 80 > maxHeight
        ? maxHeight
        : connectionListScroll.listHeight + 80
    color:  theme.colorScheme.backgroundOverlay
    border.width: 1
    border.color: theme.colorScheme.middlegroundOverlayBorder
    z: 3000

    focus: false
    property int maxHeight: 160


    property QtObject editFragment: null

    property QtObject theme: lk.layers.workspace.themes.current

    property color connectionRunnableColor: theme.colorScheme.middlegroundOverlay
    property color selectionColor: theme.colorScheme.middleground
    property alias connectionCount : connectionListScroll.count
    property alias model : connectionListScroll.model
    property Input.TextStyle labelStyle: theme.inputLabelStyle.textStyle
    property Input.TextStyle smallLabelStyle: theme.smallLabelStyle


    signal paletteSelected(int index)
    signal cancelled()

    function highlightNext(){
        if ( connectionListScroll.currentIndex + 1 <  connectionListScroll.count ){
            connectionListScroll.currentIndex++;
        } else {
            connectionListScroll.currentIndex = 0;
        }
    }
    function highlightPrev(){
        if ( connectionListScroll.currentIndex > 0 ){
            connectionListScroll.currentIndex--;
        } else {
            connectionListScroll.currentIndex = connectionListScroll.count - 1;
        }
    }

    function highlightNextPage(){
        var noItems = Math.floor(connectionListScroll.height / 25)
        if ( connectionListScroll.currentIndex + noItems < connectionListScroll.count ){
            connectionListScroll.currentIndex += noItems;
        } else {
            connectionListScroll.currentIndex = connectionListScroll.count - 1;
        }
    }
    function highlightPrevPage(){
        var noItems = Math.floor(connectionListScroll.height / 25)
        if ( connectionListScroll.currentIndex - noItems >= 0 ){
            connectionListScroll.currentIndex -= noItems;
        } else {
            connectionListScroll.currentIndex = 0;
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
            root.paletteSelected(connectionListScroll.currentIndex)
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

        visible: root.editFragment ? true : false

        Input.Label{
            id: componentLabel
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 15
            textStyle: root.labelStyle
            text: root.editFragment ? root.editFragment.typeName() : ''
        }
    }

    Item{
        height: 23
        width: parent.width - 30

        anchors.top: parent.top
        anchors.topMargin: 27
        anchors.left: parent.left
        anchors.leftMargin: 17
        clip: true

        visible: root.editFragment ? true : false

        Row{
            id: modelSplitterRow
            spacing: 5
            Repeater{
                id: modelSplitter
                model: editFragment ? editFragment.bindingPath() : []

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
            text: connectionListScroll.model && connectionListScroll.model.document() ? connectionListScroll.model.document().fileName() : ''
        }
    }

    BindingChannelListView{
        id: connectionListScroll
        anchors.top : parent.top
        anchors.topMargin: 2 + 75
        anchors.left: parent.left
        anchors.leftMargin: 2

        height : root.height - 79
        width: root.width - 4

        iconColor: theme.colorScheme.foregroundFaded
        backgroundColor: root.color
        connectionRunnableColor: root.connectionRunnableColor
        labelStyle: root.labelStyle
        selectionColor: root.selectionColor
        fileIcon: theme.fileIcon
        componentIcon: theme.componentIcon
        watcherIcon: theme.watcherIcon
        stackIcon: theme.stackIcon
    }

}

