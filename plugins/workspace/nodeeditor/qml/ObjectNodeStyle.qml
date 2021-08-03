import QtQuick 2.3
import visual.input 1.0 as Input

QtObject{
    property color background: "yellow"
    property double radius: 15
    property color borderColor: "#555"
    property double borderWidth: 1

    property color titleBackground: "#666"
    property double titleRadius: 5
    property QtObject titleTextStyle : Input.TextStyle{}
}
