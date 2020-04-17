import QtQuick 2.3
import QtQuick.Controls 2.1

import workspace.quickqanava 2.0 as Qan

Qan.NodeItem{
    id: root
    
    width: 350
    height: wrapper.height
    
    property string label: ''
    property var properties: []
    property var propertyNames: []
    property alias propertyContainer: propertyContainer
    property var nodeParent: null
    property var editingFragment: null
    property var removeNode: null
    property var addSubobject: null
    property string id: ""

    Rectangle{
        id: wrapper
        width: parent.width
        height: nodeTitle.height + propertyContainer.height + 40
        color: "#112"
        radius: 15
        border.color: "#555"
        border.width: 1
        
        Rectangle{
            id: nodeTitle
            width: parent.width
            height: 30
            anchors.top: parent.top
            color: '#666'
            radius: 5
            
            Text{
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 15
                color: 'white'
                text: root.label
            }

            Item{
                id: eraseButton
                anchors.right: parent.right
                anchors.rightMargin: 40
                anchors.verticalCenter: parent.verticalCenter
                width: 15
                height: 20
                Image{
                    anchors.centerIn: parent
                    source: "qrc:/images/palette-erase-object.png"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                    }
                }
            }

            Item{
                id: composeButton
                anchors.right: parent.right
                anchors.rightMargin: 22
                anchors.verticalCenter: parent.verticalCenter
                width: 15
                height: 20
                Image{
                    anchors.centerIn: parent
                    source: "qrc:/images/palette-add-property.png"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                    }
                }
            }

            Item{
                id: paletteAddButton
                anchors.right: parent.right
                anchors.rightMargin: 4
                anchors.verticalCenter: parent.verticalCenter
                width: 15
                height: 20
                Image{
                    anchors.centerIn: parent
                    source: "qrc:/images/palette-add.png"
                }
                MouseArea{
                    id: paletteAddMouse
                    anchors.fill: parent
                    onClicked: {
                    }
                }
            }
        }
        
        Column{
            id: propertyContainer
            spacing: 10
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.left: parent.left
            anchors.leftMargin: 5
        }
    }

    Connections {
        target: nodeParent
        ignoreUnknownSignals: true
        onFragmentChanged: {
            editingFragment = nodeParent.fragment
        }
    }

    Connections {
        target: editingFragment
        ignoreUnknownSignals: true
        onAboutToBeRemoved: {
            if (removeNode)
                removeNode(nodeParent)
        }
        onObjectAdded: {
            if (!addSubobject) returns
            var object = obj.objectInfo()
            addSubobject(nodeParent, object.name + (object.id ? ("#" + object.id) : ""), 0, object.connection)
        }
        onPropertyAdded: {
            var prop = ef.objectInfo()
            for (var i=0; i < propertyNames.length; ++i){
                if (!propertyNames[i].toString().localeCompare(prop.name.toString())) return
            }

            addSubobject(nodeParent, prop.name, (prop.isWritable?3:2), prop.connection)
        }

    }
}
