import QtQuick 2.3
import workspace 1.0

Rectangle{
    id: root

    property QtObject defaultStyle : QtObject{
        property QtObject textStyle: TextStyle{}
        property QtObject hoverTextStyle: TextStyle{}
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

    property alias label: label

    property string text: 'Button'


    Label{
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
