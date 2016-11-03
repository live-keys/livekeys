import QtQuick 2.5
import QtQuick.Controls 1.4
import Cv 1.0

Rectangle{
    id: root
    color : "#0b1924"
    gradient: Gradient{
        GradientStop { position: 0.0;  color: "#08141f" }
        GradientStop { position: 0.01; color: "#0b1924" }
    }

//    TreeView {
//        id: view
//        anchors.fill: parent
//        anchors.margins: 2 * 12 + row.height
//        model: projectFileModel

//        property var dropEntry: null
//        property var dragEntry: null

//        property var contextDelegate : null

//        function setContextDelegate(delegate){
//            if ( contextDelegate !== null )
//                contextDelegate.editMode = false
//            contextDelegate = delegate
//        }

//        TableViewColumn {
//            title: "Title"
//            role: "fileName"
//            resizable: true
//        }

//        headerVisible: false

//        itemDelegate: Item{
//            id: entryDelegate

//            property bool editMode: false

//            function focusText(){
//                entryData.selectAll()
//                entryData.forceActiveFocus()
//            }
//            function unfocusText(){
//                entryData.activeFocus = false
//            }
//            function openExternally(){
//                Qt.openUrlExternally(styleData.value.path)
//            }

//            width: 100
//            height: 25
//            Rectangle{
//                id: boundingRect
//                anchors.top: parent.top
//                anchors.left: parent.left
//                width: 100
//                height: 20
//                color: entryDelegate.editMode ? "#ddaaff" : "#ddddff"
//                TextInput{
//                    id: entryData
//                    anchors.verticalCenter: parent.verticalCenter
//                    color: styleData.value === view.dropEntry ? "#ff0000" : "#000"// styleData.textColor
//                    text: {
//                        styleData.value ? styleData.index + styleData.value.name : ''
//                    }
//                    readOnly: !entryDelegate.editMode
//                    Keys.onReturnPressed: {
//                        //TODO: Implement rename
//                        console.log('enter pressed')
//                    }
//                    Keys.onEscapePressed: {

//                    }
//                }
//                MouseArea {
//                    id: dragArea
//                    anchors.fill: parent
//                    acceptedButtons: Qt.LeftButton | Qt.RightButton
//                    drag.target: parent
//                    drag.onActiveChanged: {
//                        if ( drag.active )
//                            view.dragEntry = styleData.value
//                        boundingRect.Drag.drop()
//                    }
//                    onClicked: {
//                        if( mouse.button === Qt.RightButton ){
//                            if ( styleData.value.isFile ){
//                                view.setContextDelegate(entryDelegate)
//                                fileContextMenu.popup()
//                            } else {
//                                view.setContextDelegate(entryDelegate)
//                                dirContextMenu.popup()
//                            }
//                        } else if ( mouse.button === Qt.LeftButton ) {
//                            if ( entryDelegate.editMode ){
////                                entryData.
//                                entryData.forceActiveFocus()
//                            }
//                            if ( view.contextDelegate )
//                                view.contextDelegate.editMode = false
//                        }
//                    }
//                    onDoubleClicked: {
//                        //TODO
//                        //project.open(styleData.value)
//                    }
//                }

//                states: [
//                    State {
//                        when: boundingRect.Drag.active
//                        AnchorChanges {
//                            target: boundingRect
//                            anchors.top: undefined
//                            anchors.left: undefined
//                        }
//                        ParentChange {
//                            target: boundingRect
//                            parent: view
//                        }
//                    }
//                ]


//                Drag.hotSpot.x: width / 2
//                Drag.hotSpot.y: height / 2
//                Drag.active: dragArea.drag.active
//            }
//        }
//        rowDelegate: Item{}

//        DropArea{
//            anchors.fill: parent
//            onPositionChanged: {
//                view.dropEntry = null
//                var index = view.indexAt(view.flickableItem.contentX + drag.x, view.flickableItem.contentY + drag.y)
//                var item = projectFileModel.itemAt(index)
//                if ( item && item !== view.dragEntry && !item.isFile )
//                    view.dropEntry = item
//            }
//            onDropped: {
//                if ( view.dropEntry !== null ){
//                    projectFileModel.moveEntry(view.dragEntry, view.dropEntry)
//                    view.dragEntry = null
//                    view.dropEntry = null
//                }
//            }
//        }

////        onActivated : {
////            var url = projectFileModel.data(index, projectFileModel.UrlStringRole)
////            Qt.openUrlExternally(url)
////        }

//        Connections{
//            target: projectFileModel
//            onProjectNodeChanged : {
//                view.expand(index)
//            }
//        }

//        Menu {
//            id: fileContextMenu
////            onAboutToHide: view.contextDelegate = null

//            MenuItem{
//                text: "Open File"
//                //TODO: Implemented using project.openFile()
//            }
//            MenuItem{
//                text: "Set As Active"
//                //TODO: Implemented using project.setActive()
//            }
//            MenuItem {
//                text: "Rename"
//                onTriggered: {
//                    view.contextDelegate.editMode = true
//                    view.contextDelegate.focusText()
//                }
//            }
//            MenuItem {
//                text: "Delete"
//            }

//        }

//        Menu {
//            id: dirContextMenu

//            MenuItem{
//                text: "Show in Explorer"
//                onTriggered: {
//                    view.contextDelegate.openExternally()
//                }
//            }
//            MenuItem {
//                text: "Rename"
//                onTriggered: {
//                    view.contextDelegate.editMode = true
//                    view.contextDelegate.focusText()
//                }
//            }
//            MenuItem {
//                text: "Add File"
//            }
//            MenuItem {
//                text: "Add Directory"
//            }
//        }
//    }
}

