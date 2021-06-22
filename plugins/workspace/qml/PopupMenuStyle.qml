import QtQuick 2.0
import workspace 1.0
import visual.input 1.0 as Input

QtObject{
    property color backgroundColor: "#000"
    property color highlightBackgroundColor: "#111"
    property color borderColor: "#333"
    property double borderWidth: 1
    property double radius: 0

    property QtObject textStyle: Input.TextStyle{}
    property QtObject highlightTextStyle: Input.TextStyle{}

}

