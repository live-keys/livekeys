import QtQuick 2.0
import QtQuick.Window 2.1
import QtQuick.Controls 1.0

Window {
    id : container
    width: 400
    height: 200
    title: "LCV Log"
    color : "#041725"

    property string text : ""

    ScrollView {
        id: logScroll
        anchors.fill: parent
        anchors.margins: 5
        clip: true

        TextEdit{
            id : logText
            anchors.top : parent.top
            anchors.topMargin: 0
            text : container.text
            textFormat: Text.RichText
            readOnly: true
            font.family: "Lucida Console, Courier New"
            font.pixelSize: 12
            color : "#eee"
        }

    }
}
