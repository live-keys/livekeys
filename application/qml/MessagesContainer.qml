import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import workspace 1.0 as Workspace

Rectangle {
    id: root
    width: 400
    height: listView.height + footer.height
    visible: false
    color: "gray"
    Workspace.SelectableListView {
        anchors.top: parent.top
        id: listView
        width: parent.width
        maxHeight: 400
        style: lk.layers.workspace.themes.current.selectableListView
        model: lk.layers.workspace.messages

        delegate: Rectangle {
            width: listView.width
            height: msgText.height + 10 + 2*listView.style.borderWidth
            color: "transparent"

            Rectangle {
                anchors.top: parent.top
                height: listView.style.borderWidth
                width: parent.width
                color: listView.style.borderColor
            }

            Rectangle {

                width: parent.width - 10
                height: msgText.height
                color: "black"
                id: delegateContainer
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 5
                anchors.topMargin: 5 + listView.style.borderWidth
                Text {
                    id: msgText
                    width: listView.width - 30
                    text: model.message
                    wrapMode: Text.WordWrap
                    color: "white"
                }

                Text {
                    anchors.left: msgText.right
                    height: 10
                    text: 'x'
                    color: "white"
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            lk.layers.workspace.messages.removeAtIndex(index)
                        }
                    }
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                height: listView.style.borderWidth
                width: parent.width
                color: listView.style.borderColor
            }
        }
    }

    Rectangle {
        id: footer
        anchors.bottom: parent.bottom
        width: parent.width
        height: 30
        color: "transparent"

        Rectangle{
            id: minimize
            anchors.right: clear.left
            anchors.rightMargin: 5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            height : minimizeArea.containsMouse ? parent.height : parent.height - 3
            width : 25
            color : "transparent"
            Text{
                id : minimizeText
                anchors.centerIn: parent
                text: "-"
                font.pixelSize: 25
                color: "white"
            }
            MouseArea{
                id : minimizeArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    root.visible = false
                }
            }
            Behavior on height{ NumberAnimation{  duration: 100 } }
        }

        Rectangle{
            id: clear
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            height : clearMessages.containsMouse ? parent.height : parent.height - 3
            width : 25
            color : "transparent"
            Image{
                id : clearMessagesImage
                anchors.centerIn: parent
                source : "qrc:/images/log-clear.png"
            }
            MouseArea{
                id : clearMessages
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    lk.layers.workspace.messages.clear()
                    root.visible = false
                }
            }
            Behavior on height{ NumberAnimation{  duration: 100 } }
        }
    }

}

