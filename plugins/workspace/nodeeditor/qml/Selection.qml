import QtQuick  2.7

Rectangle {
    id: selectionItem

    // QuickQanava global selection properties support: see qan::Selectable,
    // qan::Graph::configureSelectionItem() and qan::Graph::createSelectionItem() for documentation
    property real   selectionMargin: 3
    property color  selectionColor: "#999"
    property real   selectionWeight: 1

    anchors.margins: selectionMargin
    border {
        width: selectionWeight
        color: selectionColor
    }
    opacity: 0.1
    color: Qt.rgba(0.,0.,0.,0.)
    clip: true
    radius: 4
    visible: true

    transformOrigin: Item.Center    // scale is used in transitions

    // States Management //----------------------------------------------------
    states: [
        State {
            name: "UNSELECTED"
            PropertyChanges { target: selectionItem;    opacity : 0.;   scale : 0.99  }
        },
        State {
            name: "SELECTED"
            PropertyChanges { target: selectionItem;    opacity : 0.8;   scale : 1.    }
        }
    ]
    transitions: [
        Transition {
            from: "UNSELECTED"; to: "SELECTED"
            SequentialAnimation {
                ScriptAction { script: selectionItem.visible = true }
                ParallelAnimation {
                    NumberAnimation { target: selectionItem; properties: "opacity"; duration: 300 }
                    NumberAnimation { target: selectionItem; properties: "scale";
                        easing.overshoot: 30.; easing.type: Easing.OutBack; duration: 300 }
                }
            }
        },
        Transition {
            from: "SELECTED";   to: "UNSELECTED"
            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation { target: selectionItem; properties: "opacity"; duration: 300 }
                    NumberAnimation { target: selectionItem; properties: "scale";
                        easing.type: Easing.InBack;
                        easing.overshoot: 30.;
                        duration: 300 }
                }
                ScriptAction { script: selectionItem.visible = false }
            }
        }
    ]
}
