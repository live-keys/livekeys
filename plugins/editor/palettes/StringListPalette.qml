import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import workspace 1.0 as Workspace

CodePalette{
    id: palette
    type : "qml/QStringList"


    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    item: Item{

        id: argsContainer
        visible: true
        objectName: "argsContainer"
        width: 220
        height: itemList.height + addArg.height + 5

        property alias model: itemList.model
        property bool addImportVisible: true
        property var editor: null

        ListView{
            id : itemList

            width: 220
            height: model ? (model.count  * 25) : 50

            anchors.rightMargin: 2
            anchors.bottomMargin: 5
            anchors.topMargin: 0
            visible: true
            property var fontSize: 12
            property var fontFamily: "Open Sans, sans-serif"

            model: ListModel {}

            currentIndex: 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            function modelToArray(){
                var result = []
                for (var i = 0; i < itemList.model.count; ++i)
                    result.push(itemList.model.get(i).value)
                return result
            }

            delegate: Component{

                Item{
                    width : itemList.width
                    height : 25

                    Item{
                        anchors.left: parent.left
                        anchors.leftMargin: 12

                        anchors.top: parent.top
                        anchors.topMargin: 5

                        width: 170
                        height: 20
                        Text{
                            id: argText
                            anchors.fill: parent

                            font.family: itemList.fontFamily
                            font.pixelSize: itemList.fontSize
                            font.weight: Font.Light

                            color: "#fafafa"
                            text: model.value

                            elide: Text.ElideRight
                            clip: true
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

                                var idx = model.index
                                itemList.model.remove(idx)
                                extension.write(itemList.modelToArray())
                            }
                        }
                    }
                }


            }

        }

        Item{
            id: addArg
            visible: argsContainer.addImportVisible || !argsContainer.model || (argsContainer.model && argsContainer.model.count === 0)
            width : itemList.width
            height : visible ? 20 : 0

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5

            Workspace.InputBox{
                id: inputBox
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 5
                margins: 2

                width: 170
                height: 20
                textHint: 'Arg...'

                style: paletteStyle ? paletteStyle.inputStyle : defaultStyle
            }

            Item{
                id: commitArgButton
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
                        argsContainer.model.append({'value': inputBox.text})
                        extension.write(itemList.modelToArray())
                        inputBox.text = ""
                    }
                }
            }
        }

    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(itemList.modelToArray())
        }
    }

    onInit: {
        for (var i=0; i<value.length; ++i)
            argsContainer.model.append({'value': value[i]})
    }
}
