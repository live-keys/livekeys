import QtQuick 2.0

ListView{
    id: textEditNodeView
    width: parent? parent.width: 0
    height: parent? parent.height: 0
    clip: true
    currentIndex : 0
    onCountChanged: currentIndex = 0

    boundsBehavior : Flickable.StopAtBounds
    highlightMoveDuration: 100

    property int delegateHeight : 25

    delegate: Rectangle {

        height: delegateHeight
        width: parent.width
        z: 400

        color: {
            if (model.hiddenByPalette) return "#805100"
            if (model.hiddenByCollapse) return "#7d0011"
            return "black"
        }

        Text {
            height: parent.height
            width: 20
            anchors.left: parent.left
            anchors.leftMargin: 10
            text: model.blockNumber
            color: "white"

            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 15
            font.weight: Font.Light
        }

        Rectangle {
            anchors.left: parent.left
            anchors.leftMargin: 30
            height: parent.height
            width: 1
            color: "#555555"
        }

        Text {
            height: parent.height
            width: 40
            anchors.left: parent.left
            anchors.leftMargin: 40
            text: model.lineText
            color: "white"

            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 15
            font.weight: Font.Light
        }

        Rectangle {
            anchors.left: parent.left
            anchors.leftMargin: 80
            height: parent.height
            width: 1
            color: "#555555"
        }

        Text {
            height: parent.height
            width: parent.width - 130
            clip: true
            anchors.left: parent.left
            anchors.leftMargin: 90
            text: model.blockText
            color: "white"

            elide: Text.ElideRight
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 15
            font.weight: Font.Light
        }

        Rectangle {
            anchors.right: parent.right
            anchors.rightMargin: 40
            height: parent.height
            width: 1
            color: "#555555"
        }

        Text {
            height: parent.height
            width: 30
            clip: true
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.leftMargin: 10
            text: model.offset
            color: "white"

            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 15
            font.weight: Font.Light
        }
    }
}

