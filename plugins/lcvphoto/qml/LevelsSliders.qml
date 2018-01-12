import QtQuick 2.5

Item{
    id: sliderContainer
    width: 200
    height: 30
    
    MouseArea{
        anchors.fill: parent
        onClicked: {
            var blackCloseness = Math.abs((dragBlack.x + dragBlack.width / 2) - mouse.x)
            var whiteCloseness = Math.abs((dragWhite.x + dragWhite.width / 2) - mouse.x)
            var greyCloseness  = Math.abs((dragGrey.x + dragGrey.width / 2) - mouse.x)
            if ( blackCloseness < whiteCloseness && blackCloseness < greyCloseness ){
                dragBlack.x = mouse.x - dragBlack.width / 2
                if ( dragBlack.x < 0 )
                    dragBlack.x = 0
            } else if ( greyCloseness < whiteCloseness && greyCloseness < blackCloseness ){
                dragGrey.x = mouse.x - dragGrey.width / 2
            } else if ( whiteCloseness < greyCloseness && whiteCloseness < blackCloseness ){
                dragWhite.x = mouse.x - dragWhite.width / 2
                if ( dragWhite.x > sliderContainer.width  - dragWhite.width )
                    dragWhite.x = sliderContainer.width - dragWhite.width
            }
        }
    }
    
    function updateGrey(lastW, lastB){
        var b = dragBlack.x
        var w = dragWhite.x
        var relativeGrey = (dragGrey.x - lastB) / (lastW - lastB)
        dragGrey.x = relativeGrey * (w - b) + b
    }

    Rectangle{
        id: dragBlack
        x: 0
        property double lastX: 0
        onXChanged: {
            sliderContainer.updateGrey(dragWhite.x, dragBlack.lastX)
            lastX = x
        }
        width: 10
        height: 15         radius: 5
        color: '#292b39'
        
        Drag.active: dragArea.drag.active
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: 10

        MouseArea {
            id: dragArea
            anchors.fill: parent
            
            drag.axis: Drag.XAxis
            drag.target: parent
            drag.minimumX: 0
            drag.maximumX: dragWhite.x - 1
        }
    }
    
    Rectangle{
        id: dragGrey
        x: parent.width / 2 - 5;
        width: 10
        height: 15         radius: 5
        color: '#4e4e64'

        Drag.active: dragArea.drag.active
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: 10

        MouseArea {
            id: dragAreaMiddle
            anchors.fill: parent
            
            drag.axis: Drag.XAxis
            drag.target: parent
            drag.minimumX: dragBlack.x + 1
            drag.maximumX: dragWhite.x - 1
        }
    }
    
    Rectangle{
        id: dragWhite
        x: parent.width - 10;
        property double lastX: x
        onXChanged: {
            sliderContainer.updateGrey(dragWhite.lastX, dragBlack.x)
            lastX = x
        }
        width: 10
        height: 15         radius: 5
        color: '#c1c1c8'

        Drag.active: dragArea.drag.active
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: 10

        MouseArea {
            id: dragAreaRight
            anchors.fill: parent
            
            drag.axis: Drag.XAxis
            drag.target: parent
            drag.minimumX: dragBlack.x + 1
            drag.maximumX: sliderContainer.width - width
        }
    }
    
}
