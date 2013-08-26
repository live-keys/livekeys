import QtQuick 2.0

Rectangle {
    id : container
    width: 100
    height: 35
    color : "#1f262f"

    signal openfile();
    signal savefile();
    signal newfile();

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

    // New

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 165
        height : parent.height
        width : newImage.width
        color : "transparent"
        Image{
            id : newImage
            anchors.verticalCenter: parent.verticalCenter
            source : "../LiveCVImages/new.png"
        }
        Rectangle{
            color : "#354253"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : newMArea.containsMouse
        }
        MouseArea{
            id : newMArea
            anchors.fill: parent
            hoverEnabled: true
        }
    }

    // Open

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 195
        color : "transparent"
        height : parent.height
        width : saveImage.width
        Image{
            id : saveImage
            anchors.verticalCenter: parent.verticalCenter
            source : "../LiveCVImages/save.png"
        }
        Rectangle{
            color : "#354253"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : saveMArea.containsMouse
        }
        MouseArea{
            id : saveMArea
            anchors.fill: parent
            hoverEnabled: true
        }
    }

    // Save

    Rectangle{
        anchors.left: parent.left
        anchors.leftMargin: 225
        color : "transparent"
        height : parent.height
        width : openImage.width
        Image{
            id : openImage
            anchors.verticalCenter: parent.verticalCenter
            source : "../LiveCVImages/open.png"
        }
        Rectangle{
            color : "#354253"
            width : parent.width
            height : 3
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible : openMArea.containsMouse
        }
        MouseArea{
            id : openMArea
            anchors.fill: parent
            hoverEnabled: true
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
