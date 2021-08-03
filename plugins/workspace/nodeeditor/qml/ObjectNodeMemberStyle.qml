import QtQuick 2.3
import visual.input 1.0 as Input
import workspace.icons 1.0 as Icons

QtObject{
    property color background: "#333"
    property double radius: 5
    property QtObject textStyle: Input.TextStyle{}

    property Component methodIcon: Icons.FunctionIcon{ color: 'white'; width: 15; height: 15 }
    property Component eventIcon: Icons.EventIcon{ color: 'white'; width: 15; height: 15 }
}
