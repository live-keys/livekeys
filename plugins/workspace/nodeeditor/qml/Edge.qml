
import QtQuick          2.7
import QtQuick.Shapes   1.0

import workspace.quickqanava 2.0 as Qan

Qan.EdgeItem {
    id: edgeItem

    // Private hack for visual connector edge color dynamic modification
    property color color: style ? style.lineColor : Qt.rgba(0.,0.,0.,1.)
    property var lineType: Qan.EdgeStyle.Straight
    EdgeTemplate {
        anchors.fill: parent
        edgeItem: parent
        color: parent.color
        lineType: parent.lineType
    }
}
