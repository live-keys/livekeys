import QtQuick 2.5

Rectangle{
    width: 38
    height: 18
    color: '#0c121d'
    radius: 5

    property alias text: labelText.text
    property alias textColor: labelText.color

    Text{
        id: labelText
        anchors.centerIn: parent
        font.family: "Open Sans, sans-serif"
        font.pixelSize: 11
        font.weight: Font.Light
        text: intSlider.minimumValue
        color: '#dbdede'
    }
}
