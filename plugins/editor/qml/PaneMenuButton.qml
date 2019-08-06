import QtQuick 2.0

Item{
    id: root
    anchors.top: parent.top
    anchors.right: parent.right
    width: 180
    height: 30

    signal clicked()
    property string text : ""

    Text {
        id: paneButtonText
        text: root.text
        font.family: "Open Sans, sans-serif"
        font.pixelSize: 12
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        color: paneButtonarea.containsMouse ? "#969aa1" : "#808691"
    }
    MouseArea{
        id : paneButtonarea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
