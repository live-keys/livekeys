import QtQuick 2.11

Item {
    id: root
    property color hueColor : "blue"
    property double saturation : pickerCursor.x / (width - 2 * r)
    property double brightness : 1 - pickerCursor.y / (height - 2 * r)
    property int r : 8

    property color color: '#fff'
    onColorChanged: {
        pickerCursor.x = Qt.binding(function(){ return color.hslSaturation * (width - 2 * r) })
        pickerCursor.y = Qt.binding(function(){
            return (1 - color.hsvValue) * (height - 2 * r)
        })
    }

    signal valueSelected()

    Rectangle {
        x : r
        y : r + parent.height - 2 * r
        
        rotation: -90
        transformOrigin: Item.TopLeft
        
        width: parent.height - 2 * r
        height: parent.width - 2 * r
        
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FFFFFF" }
            GradientStop { position: 1.0; color: root.hueColor }
        }
    }
    Rectangle {
        x: r
        y: r
        width: parent.width - 2 * r
        height: parent.height - 2 *  r
        gradient: Gradient {
            GradientStop { position: 1.0; color: "#FF000000" }
            GradientStop { position: 0.0; color: "#00000000" }
        }
    }

    Item {
        id: pickerCursor

        Rectangle {
            width: r * 2
            height: r * 2
            radius: r
            border.color: "#111";
            border.width: 1
            color: "transparent"
            Rectangle {
                anchors.fill: parent; anchors.margins: 2;
                border.color: "white"; border.width: 1
                radius: width / 2
                color: "transparent"
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        x: r
        y: r
        onPositionChanged: updateMouse(mouse)
        onPressed: updateMouse(mouse)

        function updateMouse(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                pickerCursor.x = Math.max(0, Math.min(width,  mouse.x) - r * 2)
                pickerCursor.y = Math.max(0, Math.min(height, mouse.y) - r * 2)
                root.valueSelected()
            }
        }
    }
}

