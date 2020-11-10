import QtQuick 2.3
import workspace 1.0 as Workspace
import live 1.0
import editor 1.0
import lcvcore 1.0 as Cv


CodePalette{
    id: palette
    type : "qml/lcvcore#Mat"

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject style: QtObject{
        property color toolbarColor: theme.colorScheme.middleground
        property color boxColor: theme.colorScheme.background
        property color boxBorderColor: theme.colorScheme.backgroundBorder
        property int boxBorderWidth: 1
        property real boxRadius: 3
        property QtObject labelStyle: theme.inputLabelStyle
        property QtObject colorPicker: QtObject{
            property QtObject input: theme.inputStyle
            property double colorDisplayBoderWidth: 1
            property color  colorDisplayBoderColor: theme.inputStyle.borderColor
            property double colorDisplayRadius: 2
            property color adjustmentBackground:  theme.colorScheme.background
            property color adjustmentBorderColor: theme.colorScheme.backgroundBorder
            property int adjustmentBorderWidth: 1
            property real adjustmentRadius: 3
            property QtObject labelStyle: theme.inputLabelStyle
        }

        property Component saveButton: theme.buttons.save
        property Component brushSizeButton: theme.buttons.penSize
    }

    item: Item{
        width: 500
        height: 300

        Cv.ImageDrawView{
            id: imagePaintView
            anchors.fill: parent
            style: palette.style
            onImagePaint: {
                palette.value.surface = image
            }
        }

        ResizeArea{
            minimumWidth: 400
            minimumHeight: 200
        }

    }

    onInit: {
        imagePaintView.image = Qt.binding(function(){ return value.input})
    }

}


