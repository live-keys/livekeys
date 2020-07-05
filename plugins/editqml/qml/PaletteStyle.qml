import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import workspace 1.0

QtObject{

    property color backgroundColor: 'black'
    property color paletteBackgroundColor: 'black'
    property color paletteHeaderColor: '#333'
    property color sectionLabelColor: '#686868'
    property color sectionHeaderBackgroundColor: '#777'
    property QtObject colorScheme: QtObject{}

    property Component scrollStyle: Component{
        ScrollViewStyle{}
    }

    property QtObject labelStyle : QtObject{
        property color background: 'black'
        property double radius: 3
        property QtObject textStyle: TextStyle{
            color: 'white'
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Light,
                italic: false,
                pixelSize: 11
            })
        }
    }

    property QtObject inputStyle : QtObject{
        property QtObject textStyle: TextStyle{
            color: 'white'
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property double radius: 3
        property QtObject hintTextStyle: TextStyle{
            color: 'white'
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property color backgroundColor: '#070b0f'
        property color borderColor: '#323232'
        property double borderThickness: 1
        property color textSelectionColor: '#3d4856'
    }

    property QtObject monoInputStyle : QtObject{
        property QtObject textStyle: TextStyle{
            color: 'white'
            font : Qt.font({
                family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property double radius: 3
        property QtObject hintTextStyle: TextStyle{
            color: 'white'
            font : Qt.font({
                family: 'Source Code Pro, Ubuntu Mono, Courier New, Courier',
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property color backgroundColor: '#070b0f'
        property color borderColor: '#323232'
        property double borderThickness: 1
        property color textSelectionColor: '#3d4856'
    }

    property QtObject buttonStyle : QtObject{
        property QtObject textStyle: TextStyle{
            color: 'white'
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property QtObject hoverTextStyle: TextStyle{
            color: 'white'
            font : Qt.font({
                family: 'Open Sans, sans-serif',
                weight: Font.Normal,
                italic: false,
                pixelSize: 11
            })
        }
        property color backgroundColor: '#070b0f'
        property color backgroundHoverColor: '#213355'
        property color borderColor: '#323232'
        property color borderHoverColor: '#323232'
        property double borderThickness: 1
        property double radius: 3
    }

    property QtObject propertyLabelStyle : QtObject{
        property color background: '#555'
        property double borderThickness: 1
        property color borderColor: 'black'
    }

    property QtObject buttons : QtObject{}
    property QtObject nodeEditor: QtObject{}
    property QtObject selectableListView: QtObject{}
    property QtObject timelineStyle: QtObject{}

}
