import QtQuick 2.3

Rectangle{
    id: root
    opacity: 0.2
    color: "transparent"
    visible: false

    property color highlightColor: "#eee"

    signal dropped(var data, int location)

    property int topPosition : 0
    property int rightPosition: 1
    property int bottomPosition: 2
    property int leftPosition: 3

    property Item currentPane : null
    property var paneFactory: null

    property alias model: repeater.model

    Repeater{
        id: repeater
        anchors.top: parent.top

        delegate: Item{
            x: modelData.x
            y: modelData.y
            width: modelData.width
            height: modelData.height

            property Item pane : modelData.pane

            Rectangle{
                x: 0
                y: 0
                width: parent.width
                height: pane === root.currentPane ? modelData.height : modelData.height / 2
                color: dropAreaTop.containsDrag ? root.highlightColor : "transparent"
            }

            DropArea {
                id: dropAreaTop
                x: 0
                y: 0
                width: parent.width
                height: pane === root.currentPane ? modelData.height : modelData.height / 4
                keys: ["text/plain"]
                onDropped: {
                    drag.accepted = true
                    root.dropped(modelData, root.topPosition)
                }
            }

            Rectangle{
                x: 0
                y: 0
                visible: pane !== root.currentPane
                width: parent.width / 2
                height: modelData.height
                color: dropAreaLeft.containsDrag ? root.highlightColor : "transparent"
            }

            DropArea {
                id: dropAreaLeft
                x: 0
                y: parent.height / 4
                visible: pane !== root.currentPane
                width: parent.width / 2
                height: modelData.height / 2
                keys: ["text/plain"]
                onDropped: {
                    drag.accepted = true
                    root.dropped(modelData, root.leftPosition)
                }
            }

            Rectangle{
                x: 0
                y: (parent.height / 4) * 2
                visible: pane !== root.currentPane
                width: parent.width
                height: modelData.height / 2
                color: dropAreaBottom.containsDrag ? root.highlightColor : "transparent"
            }

            DropArea {
                id: dropAreaBottom
                x: 0
                y: (parent.height / 4) * 3
                visible: pane !== root.currentPane
                width: parent.width
                height: modelData.height / 4
                keys: ["text/plain"]
                onDropped: {
                    drag.accepted = true
                    root.dropped(modelData, root.bottomPosition)
                }
            }

            Rectangle{
                x: parent.width / 2
                y: 0
                visible: pane !== root.currentPane
                width: parent.width / 2
                height: modelData.height
                color: dropAreaRight.containsDrag ? root.highlightColor : "transparent"
            }

            DropArea {
                id: dropAreaRight
                x: parent.width / 2
                y: parent.height / 4
                visible: pane !== root.currentPane
                width: parent.width / 2
                height: modelData.height / 2
                keys: ["text/plain"]
                onDropped: {
                    drag.accepted = true
                    root.dropped(modelData, root.rightPosition)
                }
            }
        }
    }
}
