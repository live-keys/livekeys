import QtQuick 2.3
import workspace 1.0
import visual.input 1.0 as Input

Rectangle{
    id: root

    property QtObject style: Input.TextButtonStyle{}

    signal clicked()

    color : buttonMouseArea.containsMouse ? style.backgroundHoverColor : style.backgroundColor
    radius: style.radius
    border.width: style.borderThickness
    border.color: buttonMouseArea.containsMouse ? style.borderHoverColor : style.borderColor
    clip: true

    property alias containsMouse: buttonMouseArea.containsMouse
    property alias label: label

    property string text: 'Button'

    gradient: buttonMouseArea.containsMouse ? style.backgroundHoverGradient : style.backgroundGradient

    Input.Label{
        id: label
        textStyle: buttonMouseArea.containsMouse ? root.style.hoverTextStyle : root.style.textStyle
        text: root.text
        anchors.centerIn: parent
    }

    MouseArea{
        id : buttonMouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
