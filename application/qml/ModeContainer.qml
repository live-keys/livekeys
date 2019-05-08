import QtQuick 2.3

Rectangle {
    id: modeContainer

    property var modeWrapper: null
    property var setLiveCodingMode: null
    property var setOnSaveCodingMode: null
    property var setDisabledCodingMode: null

    property alias liveImage: liveImage
    property alias onSaveImage: onSaveImage
    property alias disabledImage: disabledImage

    visible: false
    anchors.left: modeWrapper.left
    anchors.leftMargin: 770
    anchors.top: modeWrapper.bottom
    property int buttonHeight: 30
    property int buttonWidth: 120
    opacity: visible ? 1.0 : 0
    z: 1000

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
        Image{
            id : liveImage
            anchors.right: parent.right
            anchors.rightMargin: 25
            anchors.verticalCenter: parent.verticalCenter
            source : "qrc:/images/live.png"
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
        Image{
            id : onSaveImage
            anchors.right: parent.right
            anchors.rightMargin: 25
            anchors.verticalCenter: parent.verticalCenter
            source : "qrc:/images/onsave.png"
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
        Image{
            id : disabledImage
            anchors.right: parent.right
            anchors.rightMargin: 28
            anchors.verticalCenter: parent.verticalCenter
            source : "qrc:/images/disabled.png"
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
