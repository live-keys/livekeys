import QtQuick 2.3
import editor 1.0

CodePalette {
    id: palette
    type: "edit/qml"

    item : Rectangle{
        width: 80
        height: 30
        color: 'transparent'

        Rectangle{

            width: 65
            height: 20
            color: commitArea.containsMouse ? '#0a2532' : '#0a1522'

            Image{
                id : commitImage
                anchors.centerIn: parent
                source : "qrc:/images/palette-commit.png"
            }

            MouseArea{
                id: commitArea
                hoverEnabled: true
                anchors.fill: parent
                onClicked: palette.value = editFragment.parse()
            }
        }
    }
}
