import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import workspace 1.0

QtObject{

    property color backgroundColor: 'black'
    property color paletteBackgroundColor: 'black'
    property color paletteHeaderColor: '#333'
    property color sectionLabelColor: '#686868'
    property color sectionHeaderBackgroundColor: '#777'
    property color sectionHeaderFocusBackgroundColor: '#888'
    property color scrollbarColor: "#555"
    property QtObject colorScheme: QtObject{}

    property QtObject labelStyle : LabelOnRectangleStyle{}
    property QtObject inputStyle : InputBoxStyle{}
    property QtObject monoInputStyle : InputBoxStyle{
        textStyle: TextStyle{ font.family : 'Source Code Pro, Ubuntu Mono, Courier New, Courier' }
        hintTextStyle: TextStyle{ font.family : 'Source Code Pro, Ubuntu Mono, Courier New, Courier'}
    }
    property QtObject buttonStyle : TextButtonStyle{}

    property QtObject propertyLabelStyle : QtObject{
        property color background: '#555'
        property double borderThickness: 1
        property color borderColor: 'black'
    }

    property QtObject buttons : QtObject{}
    property QtObject nodeEditor: QtObject{}
    property QtObject selectableListView: SelectableListViewStyle{}
    property QtObject timelineStyle: QtObject{}

}

