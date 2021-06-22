import QtQuick 2.3

QtObject{
    
    property color backgroundColor: "#111"
    property color borderColor: "#232b30"
    property double borderSize: 1
    property double radius: 3
    
    property Component checkMark: Rectangle{
        width: 12
        height: 12
        color: "#777"
    }
}
