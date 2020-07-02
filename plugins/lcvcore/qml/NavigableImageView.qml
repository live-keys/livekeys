import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import lcvcore 1.0

Item{
    id: root

    property double maxWidth: 700
    property double maxHeight: 400

    width: image && image.dimensions().width < maxWidth ? image.dimensions().width : maxWidth
    height: image && image.dimensions().height < maxHeight ? image.dimensions().height : maxHeight

    property Mat image: null
    property double scale: 1.0

    signal clicked(var event)
    signal pressed(var event)
    signal positionChanged(var event)

    function autoScale(){
        if ( !image )
            return

        var wscale = 1.0
        var hscale = 1.0
        var dim = image.dimensions()
        if ( dim.width > root.width ){
            wscale = root.width / dim.width
        }
        if ( dim.height > root.height ){
            hscale = root.height / dim.height
        }

        var calculatedScale = wscale < hscale ? wscale : hscale
        if ( calculatedScale < 1.0 ){
            root.scale = calculatedScale
        } else {
            root.scale = 1.0
        }
    }

    property QtObject defaultStyle : QtObject{

        property Component scrollStyle: ScrollViewStyle {
            transientScrollBars: false
            handle: Item {
                implicitWidth: 10
                implicitHeight: 10
                Rectangle {
                    color: "#1f2227"
                    anchors.fill: parent
                }
            }
            scrollBarBackground: Item{
                implicitWidth: 10
                implicitHeight: 10
                Rectangle{
                    anchors.fill: parent
                    color: 'transparent'
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Item{}
            corner: Rectangle{color: 'transparent'}
        }
    }
    property QtObject style: defaultStyle

    ScrollView{
        id: scrollView
        anchors.fill: parent
        style: root.style.scrollStyle

        ImageView{
            id: imageView
            image: root.image
            width: implicitWidth * root.scale
            height: implicitHeight * root.scale

            linearFilter: false
        }
    }


    MouseArea{
        anchors.fill: parent
        onClicked: {
            var x = mouse.x / root.scale + scrollView.flickableItem.contentX / root.scale
            var y = mouse.y / root.scale + scrollView.flickableItem.contentY / root.scale
            parent.clicked({
                imageX: x, imageY: y, mouse: mouse
            })
        }
        onPressed: {
            var x = mouse.x / root.scale + scrollView.flickableItem.contentX / root.scale
            var y = mouse.y / root.scale + scrollView.flickableItem.contentY / root.scale
            parent.pressed({
                imageX: x, imageY: y, mouse: mouse
            })
        }
        onPositionChanged: {
            var x = mouse.x / root.scale + scrollView.flickableItem.contentX / root.scale
            var y = mouse.y / root.scale + scrollView.flickableItem.contentY / root.scale
            parent.positionChanged({
                imageX: x, imageY: y, mouse: mouse
            })
        }

        onWheel: {
            if (wheel.modifiers & Qt.ControlModifier){
                var delta = Math.abs(wheel.angleDelta.y)
                var x = wheel.x
                var y = wheel.y

                if (wheel.angleDelta.y > 0){
                    root.scale += (0.05 * delta)
                    scrollView.flickableItem.contentX += x - (root.width / 2)
                    scrollView.flickableItem.contentY += y
                } else {
                    if ( root.scale > (0.05 * delta) ){
                        root.scale -= (0.05 * delta)
                    }
                }
                wheel.accepted = true
            }
            else{
                wheel.accepted = false
            }
        }
    }

}
