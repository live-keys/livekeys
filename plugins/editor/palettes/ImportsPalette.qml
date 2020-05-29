import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0

CodePalette{
    id: palette
    type : "qml/import"

    item: Item{

        id: importsContainer
        visible: true
        objectName: "importsContainer"
        width: 280
        height: itemList.height + addImport.height + 5

        property alias model: itemList.model
        property bool addImportVisible: true
        property var editor: null

        ListView{
            id : itemList

            width: 280
            height: model ? (model.count  * 25) : 50

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

                    Item{
                        anchors.left: parent.left
                        anchors.leftMargin: 12

                        anchors.top: parent.top
                        anchors.topMargin: 5

                        width: 250
                        height: 20
                        Text{
                            id: moduleText
                            anchors.fill: parent

                            font.family: itemList.fontFamily
                            font.pixelSize: itemList.fontSize
                            font.weight: Font.Light

                            color: "#fafafa"
                            text: model.module + ' ' + model.version + (model.qualifier !== '' ? ' as ' + model.qualifier : '')
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
            visible: importsContainer.addImportVisible || !importsContainer.model || (importsContainer.model && importsContainer.model.count === 0)
            width : itemList.width
            height : visible ? 20 : 0

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5

            InputBox{
                id: moduleInput

                anchors.left: parent.left
                anchors.leftMargin: 10
                margins: 2
                radius: 3
                color: 'transparent'

                hintTextColor: "#494949"
                border.color : "#232427"

                anchors.top: parent.top
                anchors.topMargin: 5

                font.family: "Open Sans"
                font.pixelSize: 12

                width: 140
                height: 20
                textHint: 'Path...'
            }


            InputBox{
                id: versionInput
                anchors.left: parent.left
                anchors.leftMargin: 152
                margins: 2
                radius: 3
                color: 'transparent'

                hintTextColor: "#494949"
                border.color : "#232427"
                anchors.top: parent.top
                anchors.topMargin: 5

                font.family: "Open Sans"
                font.pixelSize: 12

                width: 40
                height: 20
                textHint: '0.0'
            }

            InputBox{
                id: qualifierInput
                anchors.left: parent.left
                anchors.leftMargin: 194
                margins: 2
                radius: 3
                color: 'transparent'

                hintTextColor: "#494949"

                border.color : "#232427"
                anchors.top: parent.top
                anchors.topMargin: 5

                font.family: "Open Sans"
                font.pixelSize: 12

                width: 40
                height: 20
                textHint: 'As...'
            }

            Item{
                id: commitImportButton
                anchors.right: parent.right
                anchors.rightMargin: 15
                anchors.top: parent.top
                anchors.topMargin: 7
                width: 15
                height: 15

                Text{
                    anchors.centerIn: parent
                    text: '+'
                    color: "white"
                    font.pixelSize: 15
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        importsContainer.model.commit(moduleInput.text, versionInput.text, qualifierInput.text)
                        moduleInput.text = ""
                        versionInput.text = ""
                        qualifierInput.text = ""
                    }
                }
            }
        }

    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }

    onInit: {
        intSlider.value = Math.floor(value)
        fractionalSlider.value = value - intSlider.value
    }
}
