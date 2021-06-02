import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import workspace 1.0
import visual.input 1.0 as Input

QtObject{

    property color backgroundColor: 'black'
    property color paletteBackgroundColor: 'black'
    property color paletteHeaderColor: '#333'
    property color sectionLabelColor: '#686868'
    property color sectionHeaderBackgroundColor: '#777'
    property color sectionHeaderFocusBackgroundColor: '#888'
    property color scrollbarColor: "#555"
    property QtObject colorScheme: QtObject{}

    property QtObject labelStyle : Input.LabelOnRectangleStyle{}
    property QtObject inputStyle : Input.InputBoxStyle{}
    property QtObject monoInputStyle : Input.InputBoxStyle{
        textStyle: Input.TextStyle{ font.family : 'Source Code Pro, Ubuntu Mono, Courier New, Courier' }
        hintTextStyle: Input.TextStyle{ font.family : 'Source Code Pro, Ubuntu Mono, Courier New, Courier'}
    }
    property QtObject buttonStyle : Input.TextButtonStyle{}

    property QtObject propertyLabelStyle : QtObject{
        property color background: '#555'
        property double borderThickness: 1
        property color borderColor: 'black'
    }

    property QtObject buttons : QtObject{}
    property QtObject nodeEditor: QtObject{}
    property QtObject selectableListView: Input.SelectableListViewStyle{}
    property QtObject timelineStyle: QtObject{}

}

