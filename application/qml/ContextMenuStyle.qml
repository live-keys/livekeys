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
            color: styleData.enabled ? "#9babb8" : "#555"
            anchors.centerIn: parent
            text: styleData.text
            font.family: 'Open Sans, Arial, sans-serif'
            font.pixelSize: 12
            font.weight: Font.Light
        }
    }
    itemDelegate.shortcut: Rectangle{
        width: dirShortCutText.width
        height: dirShortCutText.height + 6
        color: 'transparent'
        Text{
            id: dirShortCutText
            color: "#7b8b98"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            text: styleData.shortcut
            font.family: 'Open Sans, Arial, sans-serif'
            font.pixelSize: 9
            font.weight: Font.Light
        }
    }
    itemDelegate.background: Rectangle{
        color: styleData.selected && styleData.enabled ? "#092235" : "transparent"
    }
}
