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
        property color toolHighlightColor: theme.colorScheme.middlegroundOverlay
        property QtObject formButtonStyle: theme.formButtonStyle
        property QtObject labelBoxStyle: theme.inputLabelStyle

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
    onValueFromBindingChanged: {
        imagePaintView.image = Qt.binding(function(){ return value.input})
    }

}


