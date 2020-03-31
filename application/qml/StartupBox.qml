import QtQuick 2.3

Rectangle{
    width: 1040
    height: 660
    color: "#2d2d2e"

    Text{
        color: "white"
        text: "WELCOME"
        anchors.centerIn: parent
        Component.onCompleted: {
            if ( !lk.layers.workspace.wasRecentsFileFound() ){
                text = "LOOKS LIKE THIS IS THE FIRST TIME YOU'RE STARTING LIVEKEYS."
            }
        }
    }
    MouseArea{
        anchors.fill: parent
        onClicked: {
            lk.layers.workspace.panes.removeStartupBox()
        }
    }
}
