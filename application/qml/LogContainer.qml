import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import live 1.0

Rectangle{
    id: root

    color: '#050a0f'

    signal itemAdded()

    property bool isInWindow : false

    property alias prefixWidth: prefixSplitDragHandle.x
    onPrefixWidthChanged: logList.model.width = logList.width - root.prefixWidth - 5

    property int prefixPadding : 10

    property int fontSize: 12

    property bool prefixVisible : false
    function toggleLogPrefix(){
        if ( !prefixVisible ){
            prefixVisible = true
        } else {
            prefixVisible = false
        }
    }

    property Component usedDelegate : prefixVisible ? prefixDelegate : noPrefixDelegate

    property Component noPrefixDelegate : Item{
        width: logList.model.width
        height: msg.height + 2
        children: [msg]
    }

    property Component prefixDelegate : Item{
        width: logList.model.width
        height: msg.height + 2

        Text{
            y: 5
            visible: true
            color: '#ccc'
            font.family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier'
            font.pixelSize: 12
            text: prefix
            width: root.prefixWidth
            wrapMode: Text.NoWrap
            clip: true
        }

        Item{
            children: [msg]
            x: root.prefixWidth + root.prefixPadding
        }
    }

    Rectangle{
        id: logViewHeader
        width: parent.width
        height: 30
        color: root.color

        InputBox{
            id: prefixSearchBox
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5
            visible: root.prefixVisible
            anchors.fill: undefined
            width: prefixSplitDragHandle.x - root.prefixPadding - 5
            border.width: 0
            text: ''
            color: root.color
            onTextChanged: searchTimer.restart()
            height: 25

            Rectangle{
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: "#0c1c2c"
            }
        }

        Rectangle{
            id: prefixSplitDragHandle
            color: '#0c1c2c'
            width: 3
            visible: root.prefixVisible
            x: 350
            anchors.top: parent.top
            anchors.topMargin: 3
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 3

            MouseArea{
                id: prefixSplit
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: 20
                drag.maximumX: root.width - 125
                cursorShape: Qt.SplitHCursor
            }
        }

        InputBox{
            id: logSearchBox
            anchors.left: parent.left
            anchors.leftMargin: prefixSearchBox.visible ? root.prefixWidth + 5 : 5
            anchors.right: tagSearchBox.visible ? tagSearchBox.left : parent.right
            anchors.rightMargin: tagSearchBox.visible ? 10 : 110
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.fill: undefined
            text: ''
            onTextChanged : searchTimer.restart()
            color: root.color
            height: 25
            border.width: 0
            Rectangle{
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: "#0c1c2c"
            }
        }

        InputBox{
            id: tagSearchBox
            anchors.right: parent.right
            anchors.rightMargin: 110
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.fill: undefined
            width: 100
            visible: false
            text: ''
            onTextChanged : searchTimer.restart()
            border.width: 0
            color: root.color
            height: 25
            Rectangle{
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: "#273057"
            }
        }

        Timer{
            id: searchTimer
            interval: 1000
            running: false
            repeat: false
            onTriggered: {
                var logFilterActive = prefixSearchBox.text !== '' || logSearchBox.text !== '' || tagSearchBox.text
                if ( logFilterActive ){
                    logFilter.source = livecv.log

                    var search = logSearchBox.text
                    if ( search.length > 1 && search.startsWith('/') && search.endsWith('/') ){
                        search = new RegExp(search.substring(1, search.length - 1))
                    }

                    var prefixSearch = prefixSearchBox.text
                    if ( prefixSearch.length > 1 && prefixSearch.startsWith('/') && prefixSearch.endsWith('/') ){
                        prefixSearch = new RegExp(prefixSearch.substring(1, prefixSearch.length - 1))
                    }

                    logFilter.search = search
                    logFilter.prefix = prefixSearch
                    logFilter.tag    = tagSearchBox.text
                    logList.model = logFilter
                } else {
                    logList.model = livecv.log
                    logFilter.source = null
                }
            }
        }

        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            height : logWindowMouseArea.containsMouse ? parent.height : parent.height - 3
            width : 25
            color : "transparent"
            Image{
                id : newImage
                anchors.centerIn: parent
                source : "qrc:/images/log-toggle-window.png"
            }
            MouseArea{
                id : logWindowMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.isInWindow
                             ? livecv.commands.execute("window.openLogInEditor")
                             : livecv.commands.execute("window.openLogInWindow")
            }
            Behavior on height{ NumberAnimation{  duration: 100 } }
        }

        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 50
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            height : prefixMouseArea.containsMouse ? parent.height : parent.height - 3
            width : 25
            color : "transparent"
            Image{
                id : prefixImage
                anchors.centerIn: parent
                source : "qrc:/images/log-view-prefix.png"
            }
            MouseArea{
                id : prefixMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.toggleLogPrefix()
            }
            Behavior on height{ NumberAnimation{  duration: 100 } }
        }

        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 80
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            height : tagSearchMouseArea.containsMouse ? parent.height : parent.height - 3
            width : 25
            color : "transparent"
            Image{
                id : tagSearchImage
                anchors.centerIn: parent
                source : "qrc:/images/log-search-tag.png"
            }
            MouseArea{
                id : tagSearchMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: tagSearchBox.visible = !tagSearchBox.visible
            }
            Behavior on height{ NumberAnimation{  duration: 100 } }
        }

        VisualLogFilter{
            id: logFilter
        }
    }

    ScrollView{
        id: logScroll
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: logViewHeader.bottom
        anchors.bottom: parent.bottom

        anchors.margins: 5
        clip: true

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
            frame: Rectangle{color: root.color}
            corner: Rectangle{color: root.color}
        }

        ListView{
            id: logList
            model: livecv.log
            anchors.fill: parent
            delegate: root.usedDelegate

            onWidthChanged: logList.model.width = width - root.prefixWidth - root.prefixPadding

            Connections{
                target: livecv.log
                onRowsInserted : {
                    if ( root.visible && logScroll.flickableItem.contentHeight > logScroll.height )
                        logScroll.flickableItem.contentY = logScroll.flickableItem.contentHeight - logScroll.height
                    root.itemAdded()
                }
            }
        }
    }

}
