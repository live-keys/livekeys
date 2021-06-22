import QtQuick 2.2
import QtQuick.Controls 1.1
import timeline 1.0

Rectangle{
    id: root

    signal segmentFocused(Segment segment)
    signal segmentDoubleClicked(Segment segment)
    signal segmentRightClicked(Item delegate, Segment segment)
    signal trackClicked(int position)
    signal trackDoubleClicked(int position)

    property variant model: SegmentModel{}
    property int modelLength: 0

    property int zoomScale: 1

    property alias viewportX: rangeView.viewportX
    property alias viewportWidth: rangeView.viewportWidth

    property color elementColor: "#505050"
    property color elementBorderColor: "#555"
    property color elementBorderFocusColor: "#fff"

    property Component segmentDelegate: SegmentView{
        y: 3
        zoom: root.zoomScale
        height: rangeView.height - 6
    }

    color: "#ccc"
    width: modelLength * zoomScale

    RangeView{
        id: rangeView

        model: root.model
        height: root.height
        width: root.width

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.trackClicked(mouse.x / root.zoomScale)
            }
            onDoubleClicked: {
                root.trackDoubleClicked(mouse.x / root.zoomScale)
            }
        }

        delegate: root.segmentDelegate
    }
}
