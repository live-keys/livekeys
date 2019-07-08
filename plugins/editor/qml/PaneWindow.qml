import QtQuick 2.3
import QtQuick.Window 2.2

Window{
    id: root

    width: 640
    height: 400
    visible: true

    property var mainSplit : mainSplit
    property var createNewSplitter : function(){}

    property Item paneDropArea: paneDropArea
    property QtObject panes : null

    PaneSplitView{
        id: mainSplit
        anchors.fill: parent
        orientation: Qt.Vertical
        currentWindow: root
        createNewSplitter: root.createNewSplitter
    }

    PaneDropArea{
        id: paneDropArea
        anchors.fill: parent

        onDropped : {
            if ( data.pane === currentPane )
                return

            var parentSplitter = data.pane.parentSplitter
            var paneIndex = data.pane.parentSplitterIndex()
            var clone = currentPane

            root.panes.removePane(currentPane)

            if ( location === paneDropArea.topPosition ){
                root.panes.splitPaneVerticallyBeforeWith(parentSplitter, paneIndex, clone)
            } else if ( location === paneDropArea.rightPosition ){
                root.panes.splitPaneHorizontallyWith(parentSplitter, paneIndex, clone)
            } else if ( location === paneDropArea.bottomPosition ){
                root.panes.splitPaneVerticallyWith(parentSplitter, paneIndex, clone)
            } else if ( location === paneDropArea.leftPosition ){
                root.panes.splitPaneHorizontallyBeforeWith(parentSplitter, paneIndex, clone)
            }
        }
    }
}
