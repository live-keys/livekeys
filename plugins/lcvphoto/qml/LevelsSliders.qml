import QtQuick 2.5
import live 1.0
import workspace 1.0 as Workspace
import lcvcore 1.0 as Cv
import lcvphoto 1.0 as Photo

Rectangle{
    id: root
    width: 240
    height: 200
    color: 'transparent'

    property QtObject style: QtObject{
        property QtObject textStyle: Workspace.TextStyle{}
    }

    property alias input: colorHistogram.input

    property var lightness: []
    property var levelByChannel: ({})

    DropDown{
        id: channelSelection
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        width: 120
        height: 30
        dropBoxHeight: 120
        model: root.input ? (root.input.channels() >= 3 ? ['RGB', 'Red', 'Green', 'Blue'] : ['Grey']) : []
    }

    TextButton{
        text: "Auto"
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 5
        textColor: "#c7c7c7"
        height: 30
        width: 70
        radius: 15
        color: "#1b242c"
        fontPixelSize: 12
        fontFamily: "Open Sans, sans-serif"
        fontWeight: Font.Light
        onClicked : {
            autoLevelsHistogram.input = root.input
            root.levelByChannel = Photo.Adjustments.autoLevels(autoLevelsHistogram.histogram.output)
            root.updateSliders()
            autoLevelsHistogram.input = Cv.MatOp.nullMat
        }
    }

    Cv.ColorHistogramView{
        id: autoLevelsHistogram
        histogram.channel: Cv.ColorHistogram.AllChannels
        visible: false
    }

    function assignSlidersFromValues(black, grey, white){
        dragBlack.x = (black / 255) * (sliderContainer.width - dragBlack.width)
        dragWhite.x = (white / 255) * (sliderContainer.width - dragWhite.width)

        if ( grey > 1.0 ){
            dragGrey.relativeX = (grey / 10) / 2 + 0.5
        } else {
            dragGrey.relativeX = grey / 2
        }
    }

    function updateSliders(){
        if ( colorHistogram.histogram.channel === Cv.ColorHistogram.Total ){
            if ( lightness && lightness.length === 3 ){
                assignSlidersFromValues(lightness[0], lightness[1], lightness[2])
            } else {
                assignSlidersFromValues(0, 1.0, 255)
            }
        } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.RedChannel ){
            if ( levelByChannel.hasOwnProperty(2) && levelByChannel[2].length === 3){
                assignSlidersFromValues(levelByChannel[2][0], levelByChannel[2][1], levelByChannel[2][2])
            } else {
                assignSlidersFromValues(0, 1.0, 255)
            }
        } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.GreenChannel ){
            if ( levelByChannel.hasOwnProperty(1) && levelByChannel[1].length === 3){
                assignSlidersFromValues(levelByChannel[1][0], levelByChannel[1][1], levelByChannel[1][2])
            } else {
                assignSlidersFromValues(0, 1.0, 255)
            }
        } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.BlueChannel ){
            if ( levelByChannel.hasOwnProperty(0) && levelByChannel[0].length === 3){
                assignSlidersFromValues(levelByChannel[0][0], levelByChannel[0][1], levelByChannel[0][2])
            } else {
                assignSlidersFromValues(0, 1.0, 255)
            }
        }
    }

    Cv.ColorHistogramView{
        id: colorHistogram

        anchors.top: channelSelection.bottom
        anchors.topMargin: 10
        anchors.left: root.left
        anchors.leftMargin: 10

        width: root.width - 20
        height: root.height - 90
        histogram.fill: true
        histogram.channel: {
            if ( channelSelection.selectedItem === 'RGB' || channelSelection.selectedItem === 'Grey')
                return Cv.ColorHistogram.Total
            else if ( channelSelection.selectedItem === 'Red' )
                return Cv.ColorHistogram.RedChannel
            else if ( channelSelection.selectedItem === 'Green' )
                return Cv.ColorHistogram.GreenChannel
            else if ( channelSelection.selectedItem === 'Blue' )
                return Cv.ColorHistogram.BlueChannel
            return Cv.ColorHistogram.Total
        }
        histogram.onChannelChanged: root.updateSliders()
    }

    Item{
        id: sliderContainer
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 18
        anchors.left: parent.left
        anchors.leftMargin: 5

        width: root.width - 10
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
                    var dragGreyX = mouse.x - dragGrey.width / 2
                    dragGrey.relativeX = (dragGreyX - dragBlack.x) / (dragWhite.x - dragBlack.x)
                } else if ( whiteCloseness < greyCloseness && whiteCloseness < blackCloseness ){
                    dragWhite.x = mouse.x - dragWhite.width / 2
                    if ( dragWhite.x > sliderContainer.width  - dragWhite.width )
                        dragWhite.x = sliderContainer.width - dragWhite.width
                }
            }
        }

        Rectangle{
            id: dragBlack
            x: 0
            onXChanged: {
                lastX = x
            }

            property double lastX: 0
            property int assignedX: {
                var ax = (x / (parent.width - width)) * 255
                if ( ax > dragWhite.assignedX -2 )
                    ax = dragWhite.assignedX - 2
                return ax
            }
            onAssignedXChanged: {
                if ( colorHistogram.histogram.channel === Cv.ColorHistogram.Total ){
                    root.lightness = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.RedChannel ){
                    var rc = root.levelByChannel
                    rc[2] = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                    root.levelByChannel = rc
                } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.GreenChannel ){
                    var rc2 = root.levelByChannel
                    rc2[1] = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                    root.levelByChannel = rc2
                } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.BlueChannel ){
                    var rc3 = root.levelByChannel
                    rc3[0] = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                    root.levelByChannel = rc3
                }
            }
            width: 10
            height: 15
            radius: 5
            color: '#2a2c39'

//            Drag.active: dragAreaLeft.drag.active
            Drag.hotSpot.x: width / 2
            Drag.hotSpot.y: height / 2

            MouseArea{
                id: dragAreaLeft
                anchors.fill: parent

                drag.axis: Drag.XAxis
                drag.target: parent
                drag.minimumX: 0
                drag.maximumX: dragWhite.x - 1
            }
        }

        Rectangle{
            id: dragGrey
            x: (dragWhite.x - dragBlack.x) * relativeX + dragBlack.x
            onXChanged: {
                if ( dragAreaMiddle.drag.active ){
                    relativeX = (x - dragBlack.x) / (dragWhite.x - dragBlack.x)
                }
            }

            property double relativeX: 0.5 // Grey position proportion [0, 1.0]
            property double assignedX: { // Grey assigned position [0.01, 9.99]
                if ( relativeX > 0.5 ){
                    //          range (0, 0.5) -> (0, 1.0) -> (0, 9.0) -> (1, 10.0)
                    return Math.round(((relativeX - 0.5) * 2 * 9 + 1) * 100) / 100;
                } else {
                    return Math.round(relativeX * 200) / 100;
                }
            }
            onAssignedXChanged: {
                if ( colorHistogram.histogram.channel === Cv.ColorHistogram.Total ){
                    root.lightness = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.RedChannel ){
                    var rc = root.levelByChannel
                    rc[2] = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                    root.levelByChannel = rc
                } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.GreenChannel ){
                    var rc = root.levelByChannel
                    rc[1] = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                    root.levelByChannel = rc
                } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.BlueChannel ){
                    var rc = root.levelByChannel
                    rc[0] = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                    root.levelByChannel = rc
                }
            }

            width: 10
            height: 15
            radius: 5
            color: '#4e4e64'

//            Drag.active: dragArea.drag.active
            Drag.hotSpot.x: width / 2
            Drag.hotSpot.y: height / 2

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
            x: parent.width - width
            property double lastX: x
            onXChanged: {
                lastX = x

                var ax = (x / (parent.width - width)) * 255
                if ( ax < dragBlack.assignedX + 2 )
                    ax = dragBlack.assignedX + 2
                assignedX = ax

                if ( colorHistogram.histogram.channel === Cv.ColorHistogram.Total ){
                    root.lightness = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.RedChannel ){
                    var rc = root.levelByChannel
                    rc[2] = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                    root.levelByChannel = rc
                } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.GreenChannel ){
                    var rc = root.levelByChannel
                    rc[1] = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                    root.levelByChannel = rc
                } else if ( colorHistogram.histogram.channel === Cv.ColorHistogram.BlueChannel ){
                    var rc = root.levelByChannel
                    rc[0] = [dragBlack.assignedX, dragGrey.assignedX, dragWhite.assignedX]
                    root.levelByChannel = rc
                }
            }

            property int assignedX: 255

            width: 10
            height: 15
            radius: 5
            color: '#c1c1c8'

//            Drag.active: dragArea.drag.active
            Drag.hotSpot.x: width / 2
            Drag.hotSpot.y: height / 2

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

    Workspace.Label{
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.left: parent.left
        text: dragBlack.assignedX
        textStyle: root.style.textStyle
    }

    Workspace.Label{
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter
        text: dragGrey.assignedX.toFixed(2)
        textStyle: root.style.textStyle
    }

    Workspace.Label{
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.right: parent.right
        text: dragWhite.assignedX
        textStyle: root.style.textStyle
    }

}
