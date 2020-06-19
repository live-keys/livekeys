import QtQuick 2.3
import workspace 1.0

Rectangle{
    id: root

    property QtObject defaultStyle : QtObject{
        property color backgroundColor: '#070b0f'
        property color backgroundHoverColor: '#213355'
        property color borderColor: '#323232'
        property color borderHoverColor: '#323232'
        property double borderThickness: 1
        property double radius: 3
    }
    property QtObject style: defaultStyle

    signal clicked()

    color : buttonMouseArea.containsMouse ? style.backgroundHoverColor : style.backgroundColor
    radius: style.radius
    border.width: style.borderThickness
    border.color: buttonMouseArea.containsMouse ? style.borderHoverColor : style.borderColor
    clip: true

    property string text: 'Button'

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
