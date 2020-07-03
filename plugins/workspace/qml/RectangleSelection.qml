import QtQuick 2.3

Rectangle{
    id: root
    color: style.background

    property QtObject defaultStyle: QtObject{
       property color background: "#33333333"
       property color borderColor: "#666"
       property double borderWidth: 2
    }
    property QtObject style: defaultStyle

    MouseArea {
       anchors.fill: parent
       drag.target: parent
       drag.minimumX: 0
       drag.minimumY: 0
       drag.maximumX: parent.parent.width - parent.width
       drag.maximumY: parent.parent.height - parent.height
       drag.smoothed: true
       cursorShape: Qt.SizeAllCursor
       onDoubleClicked: parent.destroy()
    }

    Rectangle {
        id: leftHandle
        width: root.style.borderWidth
        height: parent.height
        color: root.style.borderColor
        anchors.horizontalCenter: parent.left
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.SizeHorCursor
            drag.target: parent
            drag.axis: Drag.XAxis
            onMouseXChanged: {
                if ( drag.active ){
                   root.width = root.width - mouseX
                   root.x = root.x + mouseX
                   if ( root.x < 0 ){
                       root.width += root.x
                       root.x = 0
                   }
                   if ( root.x >= root.parent.width - 1 ){
                       root.x = root.parent.width - 2
                   }

                   if ( root.width < 1 )
                       root.width = 1
                }
            }
        }
    }

    Rectangle {
        id: rightHandle
        width: root.style.borderWidth
        height: parent.height
        color: root.style.borderColor
        anchors.horizontalCenter: parent.right
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.SizeHorCursor
            drag.target: parent
            drag.axis: Drag.XAxis
            onMouseXChanged: {
                if ( drag.active ){
                   root.width = root.width + mouseX
                    if ( root.x + root.width > root.parent.width - 1 ){
                        root.width = root.parent.width - root.x
                    }
                    if ( root.width < 1 )
                       root.width = 1
                }
            }
        }
    }

    Rectangle {
        id: topHandle
        width: parent.width
        height: root.style.borderWidth
        color: root.style.borderColor
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.top

        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.YAxis
            cursorShape: Qt.SizeVerCursor
            onMouseYChanged: {
               if ( drag.active ){
                  root.height = root.height - mouseY
                  root.y = root.y + mouseY
                  if ( root.y < 0 ){
                      root.height += root.y
                      root.y = 0
                  }
                  if ( root.y >= root.parent.height - 1 ){
                      root.y = root.parent.height - 2
                  }

                  if ( root.height < 1 )
                      root.height = 1
               }
            }
        }
    }


    Rectangle {
        id: bottomHandle
        width: parent.width
        height: root.style.borderWidth
        color: root.style.borderColor
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.bottom

        MouseArea {
           anchors.fill: parent
           drag.target: parent
           drag.axis: Drag.YAxis
           cursorShape: Qt.SizeVerCursor
           onMouseYChanged: {
               if ( drag.active ){
                  root.height = root.height + mouseY
                   if ( root.y + root.height > root.parent.height - 1 ){
                       root.height = root.parent.height - root.y
                   }
                   if ( root.height < 1 )
                      root.height = 1
               }
           }
        }
    }
}
