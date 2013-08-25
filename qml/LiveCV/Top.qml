import QtQuick 2.0

Rectangle {
    width: 100
    height: 35
    color : "#1f262f"

    // Logo

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 7
        height : parent.height
        Image{
            anchors.verticalCenter: parent.verticalCenter
            source : "../LiveCVImages/logo.png"
        }
    }

    // New, Open, Save

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 165
        height : parent.height
        Image{
            anchors.verticalCenter: parent.verticalCenter
            source : "../LiveCVImages/new.png"
        }
    }
    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 195
        height : parent.height
        Image{
            anchors.verticalCenter: parent.verticalCenter
            source : "../LiveCVImages/save.png"
        }
    }
    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 225
        height : parent.height
        Image{
            anchors.verticalCenter: parent.verticalCenter
            source : "../LiveCVImages/open.png"
        }
    }

    // Font Size


    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 320
        height : parent.height
        Text{
            anchors.verticalCenter: parent.verticalCenter
            text : "-"
            color : "#bec7ce"
            font.pixelSize: 24
            font.family: "Arial"
        }
    }
    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 344
        height : parent.height
        Text{
            anchors.verticalCenter: parent.verticalCenter
            text : "+"
            color : "#bec7ce"
            font.pixelSize: 24
            font.family: "Arial"
        }
    }


}
