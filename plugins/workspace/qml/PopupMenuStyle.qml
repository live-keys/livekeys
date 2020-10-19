import QtQuick 2.0
import workspace 1.0

QtObject{
    property color backgroundColor: "#000"
    property color highlightBackgroundColor: "#111"
    property color borderColor: "#333"
    property double borderWidth: 1
    property double radius: 0

    property QtObject textStyle: TextStyle{}
    property QtObject highlightTextStyle: TextStyle{}

}

