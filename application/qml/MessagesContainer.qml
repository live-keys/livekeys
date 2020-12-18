import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons

Rectangle {
    id: root
    width: 400
    height: listView.height + header.height
    visible: false
    onVisibleChanged: {
        if ( !visible )
            messageState = 0
    }

    border.color: root.theme.colorScheme.middlegroundOverlayDominant
    border.width: 1
    radius: 2

    color: root.theme.colorScheme.background

    property QtObject theme: lk.layers.workspace.themes.current

    property int messageState: 0

    Rectangle {
        id: header
        anchors.top: parent.top
        width: parent.width
        height: 30
        color: "transparent"

        Rectangle{
            id: minimize
            anchors.right: parent.right
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
                color: root.theme.colorScheme.foregroundFaded
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
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            height : clearMessages.containsMouse ? parent.height : parent.height - 3
            width : 25
            color : "transparent"
            Image{
                id : clearMessagesImage
                anchors.centerIn: parent
                width: implicitWidth * 0.9
                height: implicitHeight * 0.9
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

    Connections{
        target: listView.model
        onMessageAdded:{
            if ( type > root.messageState && !root.visible )
                root.messageState = type
            if ( !root.visible )
                root.visible = true
        }
    }

    Workspace.SelectableListView {
        id: listView
        anchors.top: parent.top
        anchors.topMargin: 30

        width: parent.width
        maxHeight: 400

        style: root.theme.selectableListView
        model: lk.layers.workspace.messages

        delegate: Rectangle {
            width: listView.width
            height: msgText.height + 11

            border.width: 1
            border.color: root.theme.colorScheme.middlegroundOverlayDominant

            Rectangle{
                id: messageType
                visible: model.type !== 0
                height: 16
                width: 16
                radius: 8
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.left: parent.left
                anchors.leftMargin: 5

                border.width: 2
                border.color: model.type === 1 ? root.theme.colorScheme.warning : root.theme.colorScheme.error
                color: 'transparent'

                Text{
                    anchors.centerIn: parent
                    color: messageType.border.color
                    text: '!'
                    font.pixelSize: 12
                }
            }

            color: root.theme.colorScheme.middleground

            Item{
                id: delegateContainer
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 30
                anchors.topMargin: 5 + listView.style.borderWidth
                width: parent.width - 35
                height: msgText.height

                Workspace.Label{
                    id: msgText
                    width: listView.width - 55
                    text: model.message
                    wrapMode: Text.Wrap
                    textStyle: root.theme.inputLabelStyle.textStyle
                }

                Item{
                    anchors.right: parent.right
                    anchors.rightMargin: 3
                    anchors.top : parent.top
                    width: 20
                    height: 20

                    Icons.XIcon{
                        anchors.centerIn: parent
                        width: 8
                        height: 8
                        color: root.theme.colorScheme.foregroundFaded
                        strokeWidth: 1
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            lk.layers.workspace.messages.removeAt(index)
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        width: listView.width
        height: noMsgsText.height + 11
        anchors.top: parent.top
        anchors.topMargin: 30
        visible: listView.model.count === 0
        border.width: 1
        border.color: root.theme.colorScheme.middlegroundOverlayDominant

        color: root.theme.colorScheme.middleground

        Item{
            id: noMsgsTextContainer
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 30
            anchors.topMargin: 5 + listView.style.borderWidth
            width: parent.width - 35
            height: noMsgsText.height

            Workspace.Label{
                id: noMsgsText
                width: listView.width - 55
                font.italic: true
                text: "No messages"
                textStyle: root.theme.inputLabelStyle.textStyle
            }
        }
    }
}

