import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {

    id: importsContainer
    visible: true
    objectName: "importsContainer"
    width: 360
    height: itemList.height + addImport.height
    color: "black"
    border.width: 3
    border.color: "#141c25"

    property alias model: itemList.model
    property bool addImportVisible: false

    ListView{
        id : itemList

        width: 310
        height: model ? (model.count  * 28) : 50

        anchors.rightMargin: 2
        anchors.bottomMargin: 5
        anchors.topMargin: 0
        visible: true
        property var fontSize: 12
        property var fontFamily: "Open Sans, sans-serif"

        model: null

        currentIndex: 0
        onCountChanged: currentIndex = 0

        boundsBehavior : Flickable.StopAtBounds
        highlightMoveDuration: 100

        delegate: Component{

            Item{
                width : itemList.width
                height : 25

                Rectangle {
                    border.width: 1
                    border.color: "#141c25"
                    color: "black"
                    anchors.left: parent.left
                    anchors.leftMargin: 10

                    anchors.top: parent.top
                    anchors.topMargin: 5

                    width: 160
                    height: 20
                    Text{
                        id: moduleText
                        anchors.fill: parent
                        anchors.leftMargin: 10

                        font.family: itemList.fontFamily
                        font.pixelSize: itemList.fontSize
                        font.weight: Font.Light

                        color: "#fafafa"
                        text: model.module

                    }
                }


                Rectangle {
                    border.width: 1
                    border.color: "#141c25"
                    color: "black"
                    anchors.left: parent.left
                    anchors.leftMargin: 180

                    anchors.top: parent.top
                    anchors.topMargin: 5

                    width: 40
                    height: 20
                    Text{
                        id: versionText
                        anchors.fill: parent
                        anchors.leftMargin: 10

                        font.family: itemList.fontFamily
                        font.pixelSize: itemList.fontSize
                        font.weight: Font.Light

                        color: "#fafafa"
                        text: model.version

                    }
                }



                Rectangle {
                    border.width: 1
                    border.color: "#141c25"
                    color: "black"
                    anchors.left: parent.left
                    anchors.leftMargin: 230

                    anchors.top: parent.top
                    anchors.topMargin: 5

                    width: 40
                    height: 20
                    Text{
                        id: qualifierInput
                        anchors.fill: parent
                        anchors.leftMargin: 10

                        font.family: itemList.fontFamily
                        font.pixelSize: itemList.fontSize
                        font.weight: Font.Light

                        color: "#fafafa"
                        text: model.qualifier

                    }
                }

                Image{
                    anchors.right: parent.right
                    anchors.rightMargin: 15
                    anchors.top: parent.top
                    anchors.topMargin: 7
                    source : "qrc:/images/palette-erase-object.png"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            itemList.model.erase(model.line)
                        }
                    }
                }
            }


        }

    }


    Item{
        id: addImport
        visible: addImportVisible
        width : itemList.width
        height : visible ? 25 : 0

        anchors.bottom: parent.bottom


        Rectangle {
            border.width: 1
            border.color: "white"
            color: "black"
            anchors.left: parent.left
            anchors.leftMargin: 10

            anchors.top: parent.top
            anchors.topMargin: 5

            width: 160
            height: 20
            TextInput{
                id: moduleInput
                anchors.fill: parent
                anchors.leftMargin: 10

                font.family: itemList.fontFamily
                font.pixelSize: itemList.fontSize
                font.weight: Font.Light
                selectByMouse: true
                color: "white"
            }
        }


        Rectangle {
            border.width: 1
            border.color: "white"
            color: "black"
            anchors.left: parent.left
            anchors.leftMargin: 180

            anchors.top: parent.top
            anchors.topMargin: 5

            width: 40
            height: 20
            TextInput{
                id: versionInput
                anchors.fill: parent
                anchors.leftMargin: 10
                selectByMouse: true

                font.family: itemList.fontFamily
                font.pixelSize: itemList.fontSize
                font.weight: Font.Light

                color: "white"

            }
        }



        Rectangle {
            border.width: 1
            border.color: "white"
            color: "black"
            anchors.left: parent.left
            anchors.leftMargin: 230

            anchors.top: parent.top
            anchors.topMargin: 5

            width: 40
            height: 20
            TextInput{
                id: qualifierInput
                anchors.fill: parent
                anchors.leftMargin: 10
                selectByMouse: true

                font.family: itemList.fontFamily
                font.pixelSize: itemList.fontSize
                font.weight: Font.Light

                color: "white"

            }
        }

        Rectangle{
            id: commitImportButton
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.top: parent.top
            anchors.topMargin: 7
            width: 15
            height: 15
            color: "red"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    model.commit(moduleInput.text, versionInput.text, qualifierInput.text)
                    addImportVisible = false
                }
            }
        }
    }

    Image{
        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 25
        source : "qrc:/images/palette-add-property.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                addImportVisible = true
            }
        }
    }


    Text {
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 10

        text: "x"
        color: "white"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                importsContainer.destroy()
            }
        }
    }
}
