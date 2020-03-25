
import QtQuick          2.7
import QtQuick.Shapes   1.0

import workspace.quickqanava 2.0 as Qan

Qan.EdgeItem {
    id: edgeItem

    // Private hack for visual connector edge color dynamic modification
    property color color: '#6a6a6a'
    Qan.EdgeTemplate {
        anchors.fill: parent
        edgeItem: parent
        color: parent.color
    }
}
