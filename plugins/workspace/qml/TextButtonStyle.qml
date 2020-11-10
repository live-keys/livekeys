import QtQuick 2.3

QtObject{
    property QtObject textStyle: TextStyle{}
    property QtObject hoverTextStyle: TextStyle{}

    property color backgroundColor: '#070b0f'
    property Gradient backgroundGradient: null

    property color backgroundHoverColor: '#213355'
    property Gradient backgroundHoverGradient: null

    property color borderColor: '#323232'
    property color borderHoverColor: '#323232'
    property double borderThickness: 1
    property double radius: 3
}
