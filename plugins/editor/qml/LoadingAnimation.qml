import QtQuick 2.3

Item{
    id: root
    property double size: 6
    property double range: 20
    property color color1: '#365d98'
    property color color2: '#7162bb'
    
    Rectangle{
        width: root.size
        height: root.size
        color: root.color1
        radius: 25
        SequentialAnimation on x {
            loops: Animation.Infinite
            PropertyAnimation { to: root.range; duration: 1000; easing.type: Easing.InOutQuart; }
            PropertyAnimation { to: 0; duration: 1000; easing.type: Easing.InOutQuart; }
        }
        SequentialAnimation on opacity {
            loops: Animation.Infinite
            PropertyAnimation { to: 0.6; duration: 500; }
            PropertyAnimation { to: 1; duration: 500;  }
        }
    }
    
    Rectangle{
        width: root.size
        height: root.size
        color: root.color1
        radius: 25
        x: 25
        SequentialAnimation on x {
            loops: Animation.Infinite
            PropertyAnimation { to: 0; duration: 1000; easing.type: Easing.InOutQuart; }
            PropertyAnimation { to: root.range; duration: 1000; easing.type: Easing.InOutQuart; }
        }
        SequentialAnimation on opacity{
            loops: Animation.Infinite
            PropertyAnimation { to: 0.6; duration: 500; }
            PropertyAnimation { to: 1; duration: 500;  }
        }
    }
    
}
