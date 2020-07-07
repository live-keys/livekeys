import QtQuick 2.3
import workspace 1.0 as Workspace

Item{
    id: tooltip

    property bool mouseOver: false
    property string text: ''
    property var getCoords: function(){
        var coords = tooltip.mapToItem(lk.layers.window.window().contentItem, 0, 0)
        return { x: coords.x, y: tooltip.parent.height + coords.y }
    }

    onMouseOverChanged: {
        if ( mouseOver ){
            lk.layers.workspace.triggerTooltip(this)
        } else {
            lk.layers.workspace.cancelTooltip(this)
            active = false
            if ( contentBox ){
                contentBox.destroy()
            }
        }
    }

    property bool active: false
    onActiveChanged: {
        if ( active ){
            var theme = lk.layers.workspace.themes.current

            var contentItem = content.createObject()
            contentItem.color = theme.tooltip.backgroundColor
            contentItem.border.color = theme.tooltip.borderColor
            contentItem.border.width = theme.tooltip.borderWidth
            contentItem.radius = theme.tooltip.radius
            contentItem.opacity = theme.tooltip.opacity
            contentItem.labelStyle = theme.tooltip.labelStyle

            contentBox = lk.layers.window.dialogs.overlayBox(contentItem)
            contentBox.box.visible = true
            contentBox.centerBox = false
            contentBox.backgroundVisible = false

            var coords = tooltip.getCoords()
            contentBox.boxX = coords.x
            contentBox.boxY = coords.y
        }
    }

    property QtObject contentBox: null
    property Component content: Rectangle{
        width: label.width + 10
        height: 20
        color: 'black'
        border.width: 1
        border.color: "white"
        radius: 3

        property alias labelStyle: label.textStyle

        Workspace.Label{
            id: label
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            text: tooltip.text
        }
    }
}
