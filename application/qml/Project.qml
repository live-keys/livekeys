import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Cv 1.0

Rectangle{
    id: root
    color : "#0b1924"
    gradient: Gradient{
        GradientStop { position: 0.0;  color: "#08141f" }
        GradientStop { position: 0.01; color: "#0b1924" }
    }

    TreeView {
        id: view
        model: project.fileModel
//        anchors.top: parent.top
//        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 10
        anchors.fill: parent
//        height: parent.height - 10


        style: TreeViewStyle{
            backgroundColor: "transparent"
            transientScrollBars: false
            handle: Item {
                implicitWidth: 10
                implicitHeight: 10
                Rectangle {
                    color: "#0b1f2e"
                    anchors.fill: parent
                }
            }
            scrollBarBackground: Item{
                implicitWidth: 10
                implicitHeight: 10
                Rectangle{
                    anchors.fill: parent
                    color: "#091823"
                }
            }
            textColor: '#9babb8'
            decrementControl: null
            incrementControl: null
            frame: Rectangle{color: "transparent"}
            corner: Rectangle{color: "#091823"}
        }

        property var dropEntry: null
        property var dragEntry: null

        property var contextDelegate : null

        function setContextDelegate(delegate){
            if ( contextDelegate !== null )
                contextDelegate.editMode = false
            contextDelegate = delegate
        }

        TableViewColumn {
            title: "Title"
            role: "fileName"
            resizable: true
        }

        headerVisible: false

        itemDelegate: Item{
            id: entryDelegate

            property bool editMode: false

            function focusText(){
                entryData.selectAll()
                entryData.forceActiveFocus()
            }
            function unfocusText(){
                entryData.activeFocus = false
            }
            function setActive(){
                project.setActive(styleData.value)
            }
            function openFile(){
                project.openFile(styleData.value)
            }

            function openExternally(){
                Qt.openUrlExternally(styleData.value.path)
            }

            Rectangle{
                id: boundingRect
                height: 22
                width: entryData.width > 70 ? entryData.width + 30 : 95
                color: entryDelegate.editMode ? "#ddaaff" : "transparent"
                Image{
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    source: {
                        if ( styleData.value.isFile ){
                            if (styleData.value.isActive)
                                return "qrc:/images/project-file-active.png"
                            else
                                return "qrc:/images/project-file.png"
                        } else
                            return "qrc:/images/project-directory.png"
                    }
                }

                TextInput{
                    id: entryData
                    anchors.left: parent.left
                    anchors.leftMargin: 25
                    anchors.verticalCenter: parent.verticalCenter
                    color: styleData.value === view.dropEntry ? "#ff0000" : styleData.textColor
                    text: {
                        styleData.value
                            ? styleData.value.name === ''
                            ? 'untitled' : styleData.value.name : ''
                    }
                    font.family: 'Open Sans, Arial, sans-serif'
                    font.pixelSize: 12
                    font.weight: styleData.value.isOpen ? Font.Bold : Font.Light
                    readOnly: !entryDelegate.editMode
                    Keys.onReturnPressed: {
                        //TODO: Implement rename
                        console.log('enter pressed')
                    }
                    Keys.onEscapePressed: {

                    }
                }
                MouseArea {
                    id: dragArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    drag.target: parent
                    drag.onActiveChanged: {
                        if ( drag.active )
                            view.dragEntry = styleData.value
                        boundingRect.Drag.drop()
                    }
                    onClicked: {
                        if( mouse.button === Qt.RightButton ){
                            if ( styleData.value.isFile ){
                                view.setContextDelegate(entryDelegate)
                                fileContextMenu.popup()
                            } else {
                                view.setContextDelegate(entryDelegate)
                                dirContextMenu.popup()
                            }
                        } else if ( mouse.button === Qt.LeftButton ) {
                            if ( entryDelegate.editMode ){
                                entryData.forceActiveFocus()
                            } else {
//                                view.currentIndex = styleData.index
                            }

                            if ( view.contextDelegate )
                                view.contextDelegate.editMode = false
                        }
                    }
                    onDoubleClicked: {
                        if ( styleData.value.isFile )
                            project.openFile(styleData.value)
                        else if (view.isExpanded(styleData.index))
                            view.collapse(styleData.index)
                        else
                            view.expand(styleData.index)
                    }
                }

                states: [
                    State {
                        when: boundingRect.Drag.active
                        AnchorChanges {
                            target: boundingRect
                            anchors.top: undefined
                            anchors.left: undefined
                        }
                        ParentChange {
                            target: boundingRect
                            parent: view
                        }
                    }
                ]


                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2
                Drag.active: dragArea.drag.active
            }
        }
        rowDelegate: Item{
            height: 22
        }

        DropArea{
            anchors.fill: parent
            onPositionChanged: {
                view.dropEntry = null
                var index = view.indexAt(view.flickableItem.contentX + drag.x, view.flickableItem.contentY + drag.y)
                var item = project.fileModel.itemAt(index)
                if ( item && item !== view.dragEntry && !item.isFile )
                    view.dropEntry = item
            }
            onDropped: {
                if ( view.dropEntry !== null ){
                    project.fileModel.moveEntry(view.dragEntry, view.dropEntry)
                    view.dragEntry = null
                    view.dropEntry = null
                }
            }
        }
        Connections{
            target: project.fileModel
            onProjectNodeChanged : {
                view.expand(index)
            }
        }

        Menu {
            id: fileContextMenu


            style: MenuStyle{
                frame: Rectangle{
                    color: "#071119"
                    opacity: 0.95
                }
                itemDelegate.label: Rectangle{
                    width: fileMenuLabel.width
                    height: fileMenuLabel.height + 6
                    color: 'transparent'
                    Text{
                        id: fileMenuLabel
                        color: "#9babb8"
                        anchors.centerIn: parent
                        text: styleData.text
                        font.family: 'Open Sans, Arial, sans-serif'
                        font.pixelSize: 12
                        font.weight: Font.Light
                    }
                }
                itemDelegate.background: Rectangle{
                    color: styleData.selected ? "#092235" : "transparent"
                }
            }
            MenuItem{
                text: "Open File"
                onTriggered: {
                    view.contextDelegate.openFile()
                }
            }
            MenuItem{
                text: "Set As Active"
                onTriggered: {
                    view.contextDelegate.setActive()
                }
            }
            MenuItem {
                text: "Rename"
                onTriggered: {
                    view.contextDelegate.editMode = true
                    view.contextDelegate.focusText()
                }
            }
            MenuItem {
                text: "Delete"
            }

        }

        Menu {
            id: dirContextMenu
            style: MenuStyle{
                frame: Rectangle{
                    color: "#071119"
                    opacity: 0.95
                }
                itemDelegate.label: Rectangle{
                    width: dirLabelMenu.width
                    height: dirLabelMenu.height + 6
                    color: 'transparent'
                    Text{
                        id: dirLabelMenu
                        color: "#9babb8"
                        anchors.centerIn: parent
                        text: styleData.text
                        font.family: 'Open Sans, Arial, sans-serif'
                        font.pixelSize: 12
                        font.weight: Font.Light
                    }
                }
                itemDelegate.background: Rectangle{
                    color: styleData.selected ? "#092235" : "transparent"
                }
            }
            MenuItem{
                text: "Show in Explorer"
                onTriggered: {
                    view.contextDelegate.openExternally()
                }
            }
            MenuItem {
                text: "Rename"
                onTriggered: {
                    view.contextDelegate.editMode = true
                    view.contextDelegate.focusText()
                }
            }
            MenuItem {
                text: "Add File"
            }
            MenuItem {
                text: "Add Directory"
            }
        }
    }
}

