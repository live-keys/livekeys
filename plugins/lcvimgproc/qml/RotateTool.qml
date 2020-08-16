import QtQuick 2.3
import workspace 1.0

Tool{
    id: root
    toolLabel: 'Rotate'

    property QtObject labelInfoStyle: TextStyle{}
    property Component applyButton : null
    property Component cancelButton : null

    signal apply(double angle)
    signal cancel()

    infoBarContent: Item{
        anchors.fill: parent

        Label{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 0
            textStyle: root.labelInfoStyle
            text: 'Rotate'
        }

        InputBox{
            id: angleInput
            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.top: parent.top
            anchors.topMargin: 2
            margins: 2

            width: 40
            height: 20
            text: '0'

            style: paletteStyle ? paletteStyle.inputStyle : defaultStyle
        }

        Button{
            anchors.left: parent.left
            anchors.leftMargin: 90
            width: 25
            height: 25
            content: root.applyButton
            onClicked: {
                var value = angleInput.text
                angleInput.text = '0'
                root.apply(value)
            }
        }
        Button{
            anchors.left: parent.left
            anchors.leftMargin: 118
            width: 25
            height: 25
            content: root.cancelButton
            onClicked: {
                root.cancel()
            }
        }
    }
}
