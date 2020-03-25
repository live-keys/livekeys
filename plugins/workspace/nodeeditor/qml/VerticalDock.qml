import QtQuick 2.2
import QtQuick.Layouts 1.1

import workspace.quickqanava 2.0 as Qan

Item {
    id: root
    width: 15
    height: hostNodeItem ? hostNodeItem.height : 30
    
    z: 1.5   // Selection item z=1.0, dock must be on top of selection
    states: [
        State {
            name: "left"
            when: hostNodeItem && dockType === Qan.NodeItem.Left

            AnchorChanges {
                target: root
                anchors {
                    horizontalCenter : hostNodeItem.left
                    verticalCenter: hostNodeItem.verticalCenter
                }
            }

            PropertyChanges {
                target: root
                rightMargin: root.rightMargin
            }
        },
        State {
            name: "right"
            when: hostNodeItem && dockType === Qan.NodeItem.Right

            AnchorChanges {
                target: root
                anchors {
                    horizontalCenter: hostNodeItem.right
                    verticalCenter: hostNodeItem.verticalCenter
                }
            }

            PropertyChanges {
                target: root
                leftMargin: root.leftMargin
            }
        }
    ]

    property var hostNodeItem
    property int dockType: -1
    property int leftMargin: 7
    property int rightMargin: 7
}
