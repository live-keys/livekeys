import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import live 1.0
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons

CodePalette{
    id: palette
    type : "qml/import"

    property QtObject theme: lk.layers.workspace.themes.current

    function commitImports(){
        importsContainer.addImportVisible = false
        importsContainer.model.commit(moduleInput.text, versionInput.text, qualifierInput.text)
        moduleInput.text = ""
        versionInput.text = ""
        qualifierInput.text = ""
    }

    item: Rectangle{
        id: importsContainer
        visible: true
        objectName: "importsContainer"
        width: 280
        height: itemList.height + addImport.height + 5
        color: palette.theme.colorScheme.middleground

        property alias model: itemList.model
        property bool addImportVisible: false
        property var editor: null

        ListView{
            id : itemList

            width: parent.width - 6
            height: model ? (model.count  * 22) : 44

            anchors.top: parent.top
            anchors.topMargin: 2

            anchors.left: parent.left
            anchors.leftMargin: 3

            visible: true

            model: null

            currentIndex: 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            delegate: Item{
                width: itemList.width
                height: 22

                Rectangle{
                    width : itemList.width
                    height : 20
                    color: palette.theme.colorScheme.backgroundOverlay

                    Workspace.Label{
                        id: moduleText
                        anchors.top: parent.top
                        anchors.topMargin: 3
                        anchors.left: parent.left
                        anchors.leftMargin: 3
                        textStyle: palette.theme.selectableListView.labelStyle
                        text: model.module + ' ' + model.version + (model.qualifier !== '' ? ' as ' + model.qualifier : '')
                    }

                    Image{
                        anchors.left: moduleText.right
                        anchors.leftMargin: 15
                        anchors.top: parent.top
                        anchors.topMargin: 3

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
            anchors.bottomMargin: 2

            Workspace.InputBox{
                id: moduleInput
                anchors.left: parent.left
                anchors.leftMargin: 3
                anchors.top: parent.top
                anchors.topMargin: 2
                margins: 2

                width: 140
                height: 20
                textHint: 'Path...'
                prevFocusItem: qualifierInput
                nextFocusItem: versionInput

                onKeyPressed: {
                    if ( event.key === Qt.Key_Return ){
                        palette.commitImports()
                        event.accepted = true
                    }
                }

                style: theme.inputStyle
            }

            Workspace.InputBox{
                id: versionInput
                anchors.left: parent.left
                anchors.leftMargin: 149
                anchors.top: parent.top
                anchors.topMargin: 2
                margins: 2

                prevFocusItem: moduleInput
                nextFocusItem: qualifierInput

                width: 40
                height: 20
                textHint: '0.0'

                style: theme.inputStyle

                onKeyPressed: {
                    if ( event.key === Qt.Key_Return ){
                        palette.commitImports()
                        event.accepted = true
                    }
                }
            }

            Workspace.InputBox{
                id: qualifierInput
                anchors.left: parent.left
                anchors.leftMargin: 191
                anchors.top: parent.top
                anchors.topMargin: 2
                margins: 2

                prevFocusItem: versionInput
                nextFocusItem: moduleInput

                onKeyPressed: {
                    if ( event.key === Qt.Key_Return ){
                        palette.commitImports()
                        event.accepted = true
                    }
                }

                width: 40
                height: 20
                textHint: 'As...'

                style: theme.inputStyle
            }


            Workspace.Button{
                anchors.left: qualifierInput.right
                anchors.leftMargin: 5
                anchors.top: parent.top
                anchors.topMargin: 2
                width: 20
                height: 20

                content: theme.buttons.add
                onClicked: {
                    palette.commitImports()
                }
            }
        }

        Workspace.Button{
            id: showAddImportsButton
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.right: parent.right
            anchors.rightMargin: 10
            visible: !importsContainer.addImportVisible
            width: 30
            height: 30
            content: Rectangle{
                width: 30
                height: 30
                color: showAddImportsButtonArea.containsMouse
                    ? palette.theme.colorScheme.middlegroundOverlayBorder
                    : palette.theme.colorScheme.middlegroundOverlay
                radius: 15

                Icons.PlusIcon{
                    anchors.centerIn: parent
                    width: 8
                    height: 8
                    strokeWidth: 1
                    color: palette.theme.colorScheme.foregroundFaded
                }

                MouseArea{
                    id : showAddImportsButtonArea
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: {
                        importsContainer.addImportVisible = true
                        moduleInput.forceFocus()
                    }
                }
            }
        }

    }

    onEditFragmentChanged: {
        editFragment.whenBinding = function(){
            editFragment.write(palette.value)
        }
    }
}
