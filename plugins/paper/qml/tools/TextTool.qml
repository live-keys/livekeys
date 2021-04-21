import QtQuick 2.0
import QtQuick.Layouts 1.12
import paper 1.0
import "../papergrapherqml/tools/TextToolControl.js" as TextToolControl

Tool{
    id: root

    toolLabel: 'Text'

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            color: "white"
            text: "Text"
        }
    }


    optionsPanelContent: Item{
        anchors.fill: parent

        GridLayout{
            id: layout
            columns: 1
            anchors.margins: 5
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.righ

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Font"
            }

            Loader{
                id: fontInput
                height: 25
                width: 150
                sourceComponent: root.paperGrapherLoader.style.dropDownInput
                onItemChanged: {
                    item.valueChanged.connect(function(index, value){
                        root.implementation.options.fontFamily = value
                        updateStyles()
                        root.implementation.updateText(textInput.item.currentValue)
                    })
                }
            }

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Font Style"
            }

            Loader{
                id: styleInput
                height: 25
                width: 150
                sourceComponent: root.paperGrapherLoader.style.dropDownInput
                onItemChanged: {
                    item.valueChanged.connect(function(index, value){
                        root.implementation.options.fontStyle = value
                        root.implementation.updateText(textInput.item.currentValue)
                    })
                }
            }

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Size"
            }

            Loader{
                id: sizeInput
                height: 20
                width: 150
                sourceComponent: root.paperGrapherLoader.style.intInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.fontSize = value
                        root.implementation.updateText(textInput.item.currentValue)
                    })
                }
            }

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Spacing"
            }

            Loader{
                id: spacingInput
                height: 20
                width: 150
                sourceComponent: root.paperGrapherLoader.style.intInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.letterSpacing = value
                        root.implementation.updateText(textInput.item.currentValue)
                    })
                }
            }

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Text"
            }

            Loader{
                id: textInput
                height: 20
                width: 150
                sourceComponent: root.paperGrapherLoader.style.textInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.updateText(value)
                    })
                }
            }
        }
    }


    property var implementation: null
    property var pg

    function updateFonts(){

        var importedFonts = pg.text.getImportedFonts();
        var selectedIndex = 0
        var model = []
        for ( var i = 0; i < importedFonts.length; ++i ){
            var font = importedFonts[i]
            model.push(font.name)
            if ( implementation.options.fontFamily === font.name ){
                selectedIndex = i
            }
        }

        fontInput.item.setup({ model: model, index: selectedIndex })

        if ( importedFonts.length > 0 && implementation.options.fontFamily === '')
            implementation.options.fontFamily = importedFonts[selectedIndex].name

        updateStyles()
    }

    function updateStyles(){
        var importedFonts = pg.text.getImportedFonts();
        var font = null
        if ( importedFonts.length > 0 ){
            for( var i = 0; i < importedFonts.length; i++ ){
                var currentFont = importedFonts[i];
                if ( currentFont.name === fontInput.item.selectedValue ){
                    font = currentFont
                }
            }
        }

        var model = []
        var styleIndex = 0

        if ( font ){
            for( var j = 0; j < font.styles.length; j++ ){
                var fStyle = font.styles[j];
                model.push(fStyle.style)
                if ( implementation.options.fontStyle === fStyle.style ){
                    styleIndex = j
                }
            }

            if ( font.styles.length > 0 && implementation.options.fontStyle === '' ){
                implementation.options.fontStyle = font.styles[styleIndex].style
            }
        }

        styleInput.item.setup({ model: model, index: styleIndex })
    }

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = TextToolControl.create(paperCanvas, pg, function(state){
                if ( state ){
                    var importedFonts = pg.text.getImportedFonts();
                    if ( importedFonts.length > 0 ){
                        for( var i = 0; i < importedFonts.length; i++ ){
                            var font = importedFonts[i];
                            if ( font.name === state.fontFamily ){
                                var j = 0
                                for( j = 0; j < font.styles.length; j++ ){
                                    var fStyle = font.styles[j];
                                    if ( state.fontStyle === fStyle.style ){
                                        break
                                    }
                                }
                                fontInput.item.setup({ index: i })
                                styleInput.item.setup({ index : j })
                                root.implementation.options.fontFamily = fontInput.item.selectedValue
                                root.implementation.options.fontStyle = styleInput.item.selectedValue
                            }
                        }
                    }

                    sizeInput.item.setup({ value: state.fontSize })
                    root.implementation.options.fontSize = state.fontSize

                    spacingInput.item.setup({value: state.letterSpacing})
                    root.implementation.options.letterSpacing = state.letterSpacing

                    textInput.item.setup({ value: state.text })
                } else {
                    textInput.item.setup({ value: '' })
                }

            })

            updateFonts()
            sizeInput.item.setup({ value: 32 })
            spacingInput.item.setup({ value: 0 })

            mouseDown = implementation.onMouseDown
            mouseMove = implementation.onMouseMove
            deactivate = implementation.deactivate
        })
    }

}
