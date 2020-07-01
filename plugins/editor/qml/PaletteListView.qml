import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import base 1.0
import editor.private 1.0
import workspace 1.0 as Workspace

Workspace.SelectableListView{
    id: root

    signal paletteSelected(int index)
    onTriggered: paletteSelected(index)

    delegate: Component{
        Rectangle{
            width : root.width
            height : 25
            color : ListView.isCurrentItem ? root.style.selectionBackgroundColor : "transparent"
            Workspace.Label{
                id: label
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                textStyle: root.style.labelStyle
                text: model.name
            }

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: root.currentIndex = index
                onClicked: root.triggered(index)
            }
        }
    }
}

