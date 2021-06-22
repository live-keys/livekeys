import QtQuick 2.11
import visual.input 1.0 as Input

QtObject{
    property color backgroundColor: "#1b242c"
    property color backgroundColorHighlight: "#071825"
    property color borderColor: "#2b343c"
    property double borderSize: 1
    property double radius: 3

    property QtObject textStyle: Input.TextStyle{}

}
