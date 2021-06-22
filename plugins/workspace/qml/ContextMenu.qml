import QtQuick 2.0
import QtQuick.Controls 2.10
import workspace 1.0

Menu{
    id: contextMenu
    property Component menuItemFactory: MenuItem{
        leftPadding: 5
        background: Rectangle{
            implicitWidth: 190
            color: parent.down || parent.hovered ? contextMenu.style.highlightBackgroundColor : contextMenu.style.backgroundColor
        }
        contentItem: Text{
            text: parent.text
            verticalAlignment: Text.AlignVCenter

            font.family: parent && (parent.down || parent.hovered) ? contextMenu.style.highlightTextStyle.font.family : contextMenu.style.textStyle.font.family
            font.pixelSize: parent && (parent.down || parent.hovered) ? contextMenu.style.highlightTextStyle.font.pixelSize : contextMenu.style.textStyle.font.pixelSize
            font.weight: parent && (parent.down || parent.hovered) ? contextMenu.style.highlightTextStyle.font.weight : contextMenu.style.textStyle.font.weight
            color: parent && (parent.down || parent.hovered) ? contextMenu.style.highlightTextStyle.color : contextMenu.style.textStyle.color
        }
    }
    property QtObject style: PopupMenuStyle{}

    topPadding: style.borderWidth
    bottomPadding: style.borderWidth
    leftPadding: style.borderWidth
    rightPadding: style.borderWidth

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 30
        color: style.backgroundColor
        border.color: style.borderColor
        border.width: style.borderWidth
        radius: 2
    }

    function clearItems(){
        while (count)
            takeItem(0)
    }

    function show(model){
        for ( var i = 0; i < model.length; ++i ){
            var menuItem = menuItemFactory.createObject()
            menuItem.text = model[i].name
            // menuItem.enabled = true
            menuItem.triggered.connect(model[i].action)
            // menuItem.shortcut = model[i].shortcut ? model[i].shortcut : ''
            contextMenu.insertItem(i, menuItem)
        }

        contextMenu.popup()
    }

    onAboutToHide: {
        clearItems()
    }

}
