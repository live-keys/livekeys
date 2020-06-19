import QtQuick 2.3
import workspace 1.0 as Workspace

Item{
    id: root
    width: 180
    height: 30

    property QtObject textDefaultStyle: Workspace.TextStyle{}

    property QtObject textStyle: parent && parent.style ? parent.style.itemStyle : textDefaultStyle
    property QtObject textHoverStyle: parent && parent.style ? parent.style.itemHoverStyle : textDefaultStyle

    signal clicked()
    property string text : ""

    Workspace.Label{
        id: paneButtonText
        anchors.right: parent.right

        textStyle: paneButtonarea.containsMouse ? root.textHoverStyle : root.textStyle

        text: root.text
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }
    MouseArea{
        id : paneButtonarea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
