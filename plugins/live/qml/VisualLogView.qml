import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

ScrollView{
    id: root
    clip: true

    property alias model: logView.model
    property color backgroundColor: "#081017"

    signal itemAdded()

    style: ScrollViewStyle {
        transientScrollBars: false
        handle: Item {
            implicitWidth: 10
            implicitHeight: 10
            Rectangle {
                color: "#1f2227"
                anchors.fill: parent
            }
        }
        scrollBarBackground: Item{
            implicitWidth: 10
            implicitHeight: 10
            Rectangle{
                anchors.fill: parent
                color: root.backgroundColor
            }
        }
        decrementControl: null
        incrementControl: null
        frame: Rectangle{color: root.backgroundColor}
        corner: Rectangle{color: root.backgroundColor}
    }

    ListView{
        id: logView
        anchors.fill: parent
        delegate: Item{
            width: logView.model.width
            height: msg.height + 2
            children: [msg]
        }

        onAddChanged: {
            if ( logScroll.flickableItem.contentHeight > logScroll.height )
                logScroll.flickableItem.contentY = logScroll.flickableItem.contentHeight - logScroll.height
            root.itemAdded()
        }
        onWidthChanged: logView.model.width = width
    }

}
