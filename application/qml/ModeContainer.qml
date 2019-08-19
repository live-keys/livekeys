import QtQuick 2.3
import editor 1.0
import editor.private 1.0

Item{
    id: modeContainer

    signal runTriggerSelected(int trigger)

    function setLiveCodingMode(){
        modeContainer.visible = false
        modeContainer.runTriggerSelected(Project.RunOnChange)
    }

    function setOnSaveCodingMode(){
        modeContainer.visible = false
        modeContainer.runTriggerSelected(Project.RunOnSave)
    }

    function setDisabledCodingMode(){
        modeContainer.visible = false
        modeContainer.runTriggerSelected(Project.RunManual)
    }

    property alias liveImage: liveImage
    property alias onSaveImage: onSaveImage
    property alias disabledImage: disabledImage

    visible: false
    property int buttonHeight: 30
    property int buttonWidth: 120
    opacity: visible ? 1.0 : 0
    z: 1000

    width: buttonWidth
    height: buttonHeight * 3

    Behavior on opacity{ NumberAnimation{ duration: 200 } }

    Rectangle{
        id: liveButton
        anchors.top: parent.top
        anchors.right: parent.right
        width: parent.buttonWidth
        height: parent.buttonHeight
        color : "#03070b"
        Text {
            id: liveText
            text: qsTr("Live")
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            color: liveArea.containsMouse ? "#969aa1" : "#808691"
        }
        Item{
            width: 25
            height: 25
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20

            Image{
                id : liveImage
                anchors.centerIn: parent
            }
        }

        MouseArea{
            id : liveArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: function(){
                modeContainer.setLiveCodingMode()
            }
        }
    }

    Rectangle{
        id: onSaveButton
        anchors.top: liveButton.visible ? liveButton.bottom : parent.top
        anchors.right: parent.right
        width: parent.buttonWidth
        height: parent.buttonHeight
        color : "#03070b"
        Text {
            id: onSaveText
            text: qsTr("On Save")
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            color: onSaveArea.containsMouse ? "#969aa1" : "#808691"
        }

        Item{
            width: 25
            height: 25
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20

            Image{
                id : onSaveImage
                anchors.centerIn: parent
            }
        }
        MouseArea{
            id : onSaveArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: function(){
                modeContainer.setOnSaveCodingMode()
            }
        }
    }

    Rectangle{
        id: disabledButton
        anchors.top: onSaveButton.bottom
        anchors.right: parent.right
        width: parent.buttonWidth
        height: parent.buttonHeight
        color : "#03070b"
        Text {
            id: disabledText
            text: qsTr("Disabled")
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            color: disabledArea.containsMouse ? "#969aa1" : "#808691"
        }

        Item{
            width: 25
            height: 25
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20

            Image{
                id : disabledImage
                anchors.centerIn: parent
            }
        }

        MouseArea{
            id : disabledArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: function(){
                modeContainer.setDisabledCodingMode()
            }
        }
    }
}
