import QtQuick 2.3
import workspace 1.0

Rectangle{
    id: root

    property QtObject defaultStyle : RectangleButtonStyle{}
    property QtObject style: defaultStyle

    signal clicked()

    color : buttonMouseArea.containsMouse ? style.backgroundHoverColor : style.backgroundColor
    radius: style.radius
    border.width: style.borderThickness
    border.color: buttonMouseArea.containsMouse ? style.borderHoverColor : style.borderColor
    clip: true

    property Component content: null

    Loader{
        id: loader
        anchors.centerIn: parent
        sourceComponent: root.content
    }

    MouseArea{
        id : buttonMouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
