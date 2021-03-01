import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import lcvimgproc 1.0 as Img
import visual.input 1.0 as Input

CodePalette {
    id: palette

    type : "qml/lcvimgproc#Threshold"
    property var threshold: null

    item: Input.DropDown {
        id: dropdown
        width: 100
        model: ["BINARY", "BINARY_INV", "TRUNC", "TOZERO", "TOZERO_INV"]
        property var values: [
            Img.Transformations.BINARY,
            Img.Transformations.BINARY_INV,
            Img.Transformations.TRUNC,
            Img.Transformations.TOZERO,
            Img.Transformations.TOZERO_INV
        ]
        onCurrentIndexChanged: {
            if (!threshold) return
            threshold.type = values[dropdown.currentIndex]
        }
    }

    onInit: {
        threshold = value
        var idx = dropdown.values.indexOf(threshold.type)
        dropdown.currentIndex = idx
    }

    onValueFromBindingChanged: {
        threshold = value
        var idx = dropdown.values.indexOf(threshold.type)
        dropdown.currentIndex = idx
    }
}
