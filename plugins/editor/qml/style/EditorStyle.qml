import QtQuick 2.0
import visual.input 1.0

QtObject{
    property color backgroundColor: 'black'
    property color lineSurfaceBackgroundColor: 'black'

    property color selectionColor: '#fff'
    property color selectionBackgroundColor: '#3d4856'

    property color scrollbarHandleColor: '#1f2227'
    property TextStyle textStyle: TextStyle{
        color: '#fff'
        font : Qt.font({
            family: 'Source Code Pro, Courier New, Courier',
            weight: Font.Normal,
            italic: false,
            pixelSize: 12
        })
    }
    property color lineSurfaceTextColor: "#3e464d"
}
