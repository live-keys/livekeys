import QtQuick 2.3
import editqml 1.0

Grid{

    PaletteStyle{
        id: paletteStyle
        sectionHeaderBackgroundColor: 'purple'
    }

    Component.onCompleted: {
        var pstyle = lk.layers.workspace.extensions.editqml.paletteStyle
        pstyle.sectionHeaderBackgroundColor = Qt.binding(function(){return paletteStyle.sectionHeaderBackgroundColor})
    }

}
