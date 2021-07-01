import QtQuick 2.3

Rectangle{

    property var parentSplitView : null
    property var parentSplitViewIndex : function(){
        return parentSplitView ? parentSplitView.paneIndex(this) : -1
    }
    property var splitterHierarchyIndex : function(){
        if (!parentSplitView){
            return []
        }
        return parentSplitView.splitterHierarchyIndex().concat([parentSplitView.paneIndex(this)])
    }
    property var splitterHierarchyPositioning : function(){
        if (!parentSplitView){
            return []
        }

        var currentIndex = (parentSplitView.paneIndex(this) + 1) *
                (parentSplitView.orientation === Qt.Vertical ? -1 : 1)

        return parentSplitView.splitterHierarchyPositioning().concat([currentIndex])
    }
    property var paneWindow : function(){
        return parentSplitView ? parentSplitView.currentWindow : null
    }

    property var paneHelp : function(){
        return null
    }

    property var mapGlobalPosition: function(){
        if ( parentSplitView ){
            var parentPoint = parentSplitView.mapGlobalPosition()
            return Qt.point(parentPoint.x + x, parentPoint.y + y)
        }
        return Qt.point(x, y)
    }

    property string paneType: ''
    property int paneMinimumWidth: 100
    property int paneMinimumHeight: 50

    property var paneState : { return {} }
    property var paneInitialize : function(s){}
    property var paneCleared : function(){}
    property var paneClone : null
    property var paneFocusItem : null
}
