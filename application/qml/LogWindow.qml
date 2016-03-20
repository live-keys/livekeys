import QtQuick 2.0
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.2

Window {
    id : container
    width: 400
    height: 200
    title: "LCV Log"
    color : "#05111b"

    property string text : ""

    ScrollView {
        id: logScroll
        anchors.fill: parent
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
                    color: "#091a27"
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Rectangle{color: "#071723"}
            corner: Rectangle{color: "#071723"}
        }

        TextEdit{
            id : logText
            anchors.top : parent.top
            anchors.topMargin: 0
            text : container.text
            onTextChanged: {
                if ( logScroll.flickableItem.contentHeight > logScroll.height )
                    logScroll.flickableItem.contentY = logScroll.flickableItem.contentHeight - logScroll.height
            }
            textFormat: Text.RichText
            readOnly: true
            font.family: "Source Code Pro, Ubuntu Mono, Courier New, Courier"
            font.pixelSize: 12
            color : "#eee"
        }

    }
}
