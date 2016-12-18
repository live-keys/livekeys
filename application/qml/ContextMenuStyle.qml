import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

MenuStyle{
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
