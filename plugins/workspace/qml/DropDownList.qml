import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import base 1.0
import editor.private 1.0
import visual.input 1.0 as Input
import workspace.icons 1.0 as Icons

Item{
    id: root
    width: listView.width
    height: listView.height + 30 + (updateButton.visible ? 30 : 0)
    
    property QtObject theme : lk.layers.workspace.themes.current
    
    property var whenEntrySelected: function(index, value){
        var model = root.model
        model.push(value)
        root.model = model
    }
    property var whenEntryDeleted: function(index){
        var model = root.model
        model.splice(index, 1)
        root.model = model
    }
    
    property var selectionModel: []
    property alias model: listView.model
    property bool isDirty: false

    signal ready(var data)

    Input.SelectableListView{
        id: listView
        anchors.top: parent.top
        anchors.topMargin: 5
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
                        text: modelData
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
                                root.isDirty = true
                                root.whenEntryDeleted(index)
                            }
                        }
                    }
                }
                
            }
        }
    }
    
    Input.Button{
        id: showAddEntry
        anchors.bottom: parent.bottom
        anchors.bottomMargin: updateButton.visible ? 30 : 0
        anchors.right: parent.right
        anchors.rightMargin: 10
        visible: !addEntry.visible
        width: parent.wi
        height: 25
        content: Rectangle{
            width: 25
            height: 25
            color: showAddImportsButtonArea.containsMouse
                ? root.theme.colorScheme.middlegroundOverlayBorder
                : root.theme.colorScheme.middlegroundOverlay
            radius: 12

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
                }
            }
        }
    }
    
    Input.Label{
        id: label2
        
        anchors.bottom: parent.bottom
        anchors.bottomMargin: updateButton.visible ? 35 : 5
        anchors.left: parent.left
        anchors.leftMargin: 10
        
        textStyle: root.theme.inputLabelStyle.textStyle
        text: listView.model.length + (listView.model.length === 1 ? " entry" : " entries")
    }
    
    Item{
        id: addEntry
        
        anchors.bottom: parent.bottom
        anchors.bottomMargin: updateButton.visible ? 30 : 0
        
        height: visible ? 30 : 0
        width: parent.width
        visible: false
        
        Item{
            id: entryInput
            anchors.left: parent.left
            anchors.leftMargin: 0
            width: parent.width - 55
            height : visible ? 25 : 0
            
            Input.DropDown{
                id: entryInputDropDown
                anchors.left: parent.left
                anchors.leftMargin: 3
                width: parent.width
                style: root.theme.dropDownStyle
        
                model: root.selectionModel
            }
        }
        
        Input.Button{
            id: commitButton
            anchors.right: parent.right
            anchors.rightMargin: 25
            width: 25
            height: 25
            content: root.theme.buttons.apply
            visible: root.selectionModel.length
            onClicked: {
                var value = entryInputDropDown.currentValue
                var index = entryInputDropDown.currentIndex
                
                root.whenEntrySelected(index, value)
                addEntry.visible = false
                root.isDirty = true
            }
        }
        Input.Button{
            id: cancelButton
            anchors.right: parent.right
            width: 25
            height: 25
            content: root.theme.buttons.cancel
            onClicked: {
                addEntry.visible = false
            }
        }
    }
    
    Input.TextButton{
        id: updateButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 0
        anchors.bottom: parent.bottom
        height: visible ? 25 : 0
        width: 70
        style: root.theme.formButtonStyle

        visible: root.isDirty

        text: "Update"
        onClicked: {
            root.isDirty = false
            root.ready(listView.model)
        }
    }
}

