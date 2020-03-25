import QtQuick 2.7
import QtQuick.Controls 2.1

import workspace.quickqanava 2.0 as Qan

Qan.PortItem {
    id: portItem
    width: 15
    height: 15
    property Item objectProperty : null

    Rectangle {
        id: contentItem
        anchors.fill: parent
        radius: width / 2
        color: "#3a3a44"
        border {
            color: "#555"
            width: 1
        }
    }
}
