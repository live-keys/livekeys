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

        function lcvImgProcQualifier(){
            var imports = editFragment.language.importsModel()
            if (!imports)
                return ''

            var lcvimgprocImport = imports.getImportAtUri('lcvimgproc')
            if (!lcvimgprocImport)
                return ''

            return lcvimgprocImport.as.length ? lcvimgprocImport.as : ''

        }

        onCurrentIndexChanged: {
            if ( isBindingChange() )
                return

            if (!threshold)
                return

            threshold.type = values[dropdown.currentIndex]
            if (!editFragment)
                return

            var qual = lcvImgProcQualifier()
            if (qual.length){
                var writeValue = qual + ".Transformations." + model[dropdown.currentIndex]
                editFragment.writeProperties({
                    'type' : {"__ref": writeValue}
                })
            }
            else {
                editFragment.writeProperties({
                    'type' : threshold.type
                })
            }
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
