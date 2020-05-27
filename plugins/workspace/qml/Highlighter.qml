import QtQuick 2.3

Rectangle{
    width: visible ? 50 : 0
    height: visible ? 50 : 0
    color: '#c7ddc7'
    radius: 25
    opacity: visible ? 0.4 : 0
    visible: false
    onVisibleChanged: {
        if ( visible ){
            xAnimBehavior.enabled = false
            x = 25
            xAnimBehavior.enabled = true
            x = 0

            yAnimBehavior.enabled = false
            y = 25
            yAnimBehavior.enabled = true
            y = 0
        }
    }

    Behavior on opacity {
        NumberAnimation{ duration: 250 }
    }
    Behavior on width{
        NumberAnimation{ duration: 250 }
    }
    Behavior on height{
        NumberAnimation{ duration: 250 }
    }
    Behavior on x{
        id: xAnimBehavior
        NumberAnimation{ id: xAnim; duration: 250 }
    }
    Behavior on y{
        id: yAnimBehavior
        NumberAnimation{ id: yAnim; duration: 250 }
    }
}
