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
                contentBox = null
            }
        }
    }

    property bool active: false
    onActiveChanged: {
        if ( active ){
            var theme = lk.layers.workspace.themes.current
            var windowContent = lk.layers.window.window()

            contentBox = content.createObject(windowContent)
            contentBox.color = theme.tooltip.backgroundColor
            contentBox.border.color = theme.tooltip.borderColor
            contentBox.border.width = theme.tooltip.borderWidth
            contentBox.radius = theme.tooltip.radius
            contentBox.opacity = theme.tooltip.opacity
            contentBox.labelStyle = theme.tooltip.labelStyle
            contentBox.z = 20000

            var coords = tooltip.getCoords()
            contentBox.x = coords.x
            contentBox.y = coords.y
        }
    }

    property QtObject contentBox: null
    property Component content: Rectangle{
        width: label.width + 10
        height: label.height + 10
        color: 'black'
        border.width: 1
        border.color: "white"
        radius: 3

        property alias labelStyle: label.textStyle

        Workspace.Label{
            id: label
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5
            text: tooltip.text
            width: implicitWidth > 200 ? 200 : contentWidth
            wrapMode: Text.WordWrap
        }
    }
}
