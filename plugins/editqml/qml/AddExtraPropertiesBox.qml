import QtQuick 2.3
import QtQuick.Controls 2.2
import base 1.0
import editor.private 1.0
import visual.input 1.0 as Input
import workspace.icons 1.0 as Icons

Item{
    id: root
    width: listView.width
    height: listView.height + 85 + addEntry.height

    property QtObject theme : lk.layers.workspace.themes.current

    function addEntry(type, name){
        var model = listView.model
        for ( var i = 0; i < model.length; ++i ){
            if ( model[i].name === name )
                return
        }

        model.push({type: type, name: name})
        listView.model = model
    }

    property string type: ''

    signal cancel()
    signal ready(var data)

    Rectangle{
        width: parent.width
        height: 25
        color: root.theme.colorScheme.middlegroundOverlay

        Item{
            anchors.right: parent.right
            anchors.rightMargin: 2
            width: parent.height
            height: parent.height

            Icons.XIcon{
                anchors.centerIn: parent
                width: 8
                height: 8
                strokeWidth: 1
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    root.cancel()
                }
            }
        }

        Input.Label{
            id: label1
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 10
            textStyle: root.theme.inputLabelStyle.textStyle
            text: root.type
        }
    }

    Input.Label{
        id: label2
        anchors.top: parent.top
        anchors.topMargin: 32
        anchors.left: parent.left
        anchors.leftMargin: 10
        textStyle: root.theme.inputLabelStyle.textStyle
        text: "Properties"
    }

    Input.SelectableListView{
        id: listView
        anchors.top: parent.top
        anchors.topMargin: 55
        width: 200

        model: []

        delegate: Component{
            Item{
                height: 25
                width: listView.width

                Rectangle{
                    width : listView.width
                    height : 24
                    color : root.theme.colorScheme.middleground
                    Input.Label{
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        textStyle: listView.style.labelStyle
                        text: modelData.name
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
                                var idx = index
                                var model = listView.model
                                model.splice(idx, 1)
                                listView.model = model
                            }
                        }
                    }
                }

            }
        }
    }

    Input.Button{
        id: showAddEntry
        anchors.top: parent.top
        anchors.topMargin: 28
        anchors.right: parent.right
        anchors.rightMargin: 10
        visible: !addEntry.visible
        width: 25
        height: 25
        content: Rectangle{
            width: 25
            height: 25
            color: showAddImportsButtonArea.containsMouse
                ? root.theme.colorScheme.middlegroundOverlayBorder
                : root.theme.colorScheme.middlegroundOverlay
            radius: 15

            Icons.PlusIcon{
                anchors.centerIn: parent
                width: 8
                height: 8
                strokeWidth: 1
                color: root.theme.colorScheme.foregroundFaded
            }

            MouseArea{
                id : showAddImportsButtonArea
                hoverEnabled: true
                anchors.fill: parent
                onClicked: {
                    addEntry.visible = true
                    entryInput.forceFocus()
                }
            }
        }
    }

    Item{
        id: addEntry
        height: visible ? 30 : 0
        width: parent.width
        visible: false
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30

        Item{
            id: addImport
            anchors.left: parent.left
            anchors.leftMargin: 0
            width: parent.width - 55
            height : visible ? 25 : 0

            Input.InputBox{
                id: entryInput
                anchors.left: parent.left
                anchors.leftMargin: 3
                margins: 4

                width: parent.width
                height: 25
                textHint: ''

                onKeyPressed: {
                    if ( event.key === Qt.Key_Return ){
                        var value = entryInput.text

                        entryInput.text = ''
                        entryInput.focus = false
                        addEntry.visible = false

                        root.addEntry('var', value)
                    }
                }

                style: theme.inputStyle
            }
        }

        Input.Button{
            id: commitButton
            anchors.right: parent.right
            anchors.rightMargin: 25
            width: 25
            height: 25
            content: root.theme.buttons.apply
            onClicked: {
                var value = entryInput.text

                entryInput.text = ''
                entryInput.focus = false
                addEntry.visible = false

                root.addEntry('var', value)
            }
        }
        Input.Button{
            id: cancelButton
            anchors.right: parent.right
            width: 25
            height: 25
            content: root.theme.buttons.cancel
            onClicked: {
                entryInput.text = ''
                entryInput.focus = false
                addEntry.visible = false
            }
        }

    }


    Input.TextButton{
        id: updateButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 0
        anchors.bottom: parent.bottom
        height: 25
        width: 70
        style: root.theme.formButtonStyle

        text: "Done"
        onClicked: {
            root.ready(listView.model)
        }
    }
}

