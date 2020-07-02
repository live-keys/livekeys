import QtQuick 2.3
import workspace 1.0 as Workspace
import live 1.0
import editor 1.0
import lcvcore 1.0 as Cv


CodePalette{
    id: palette
    type : "qml/lcvcore#Mat"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    property QtObject style: QtObject{
        property color toolbarColor: paletteStyle ? paletteStyle.colorScheme.middleground : imagePaintView.defaultStyle.toolbarColor
        property color boxColor: paletteStyle ? paletteStyle.colorScheme.background : imagePaintView.defaultStyle.boxColor
        property color boxBorderColor: paletteStyle ? paletteStyle.colorScheme.backgroundBorder : imagePaintView.defaultStyle.boxBorderColor
        property int boxBorderWidth: 1
        property real boxRadius: 3
        property QtObject labelStyle: paletteStyle ? paletteStyle.labelStyle : imagePaintView.defaultStyle.labelStyle
        property QtObject colorPicker: QtObject{
            property QtObject input: paletteStyle ? paletteStyle.inputStyle : imagePaintView.defaultStyle.input
            property double colorDisplayBoderWidth: 1
            property color  colorDisplayBoderColor: paletteStyle ? paletteStyle.inputStyle.borderColor : imagePaintView.defaultStyle.colorPicker.colorDisplayBoderColor
            property double colorDisplayRadius: 2
            property color adjustmentBackground:  paletteStyle ? paletteStyle.colorScheme.background : imagePaintView.defaultStyle.colorPicker.adjustmentBackground
            property color adjustmentBorderColor: paletteStyle ? paletteStyle.colorScheme.backgroundBorder : imagePaintView.defaultStyle.colorPicker.adjustmentBorderColor
            property int adjustmentBorderWidth: 1
            property real adjustmentRadius: 3
            property QtObject labelStyle: paletteStyle ? paletteStyle.labelStyle : imagePaintView.defaultStyle.colorPicker.labelStyle
        }

        property Component saveButton: paletteStyle ? paletteStyle.buttons.save : imagePaintView.defaultStyle.saveButton
        property Component brushSizeButton: paletteStyle ? paletteStyle.buttons.penSize : imagePaintView.defaultStyle.brushSizeButton
    }

    item: Item{
        width: 500
        height: 300

        Cv.ImageDrawView{
            id: imagePaintView
            anchors.fill: parent
            style: palette.style
            onImagePaint: {
                console.log(palette.value)
                console.log(palette.value.surface)
                palette.value.surface = image
            }
        }

        ResizeArea{
            minimumWidth: 400
            minimumHeight: 200
        }

    }

    onInit: {
        imagePaintView.image = value.input
    }

}


