import QtQuick 2.3

Rectangle{

    property var parentSplitter : null
    property var parentSplitterIndex : function(){
        return parentSplitter ? parentSplitter.paneIndex(this) : -1
    }
    property var splitterHierarchyIndex : function(){
        if (!parentSplitter){
            return []
        }
        return parentSplitter.splitterHierarchyIndex().concat([parentSplitter.paneIndex(this)])
    }
    property var splitterHierarchyPositioning : function(){
        if (!parentSplitter){
            return []
        }

        var currentIndex = (parentSplitter.paneIndex(this) + 1) *
                (parentSplitter.orientation === Qt.Vertical ? -1 : 1)

        return parentSplitter.splitterHierarchyPositioning().concat([currentIndex])
    }
    property var paneWindow : function(){
        return parentSplitter ? parentSplitter.currentWindow : null
    }

    property var paneHelp : function(){
        return null
    }


    property var mapGlobalPosition: function(){
        if ( parentSplitter ){
            var parentPoint = parentSplitter.mapGlobalPosition()
            return Qt.point(parentPoint.x + x, parentPoint.y + y)
        }
        return Qt.point(x, y)
    }

    property string paneType: ''
    property var paneState : { return {} }
    property var paneInitialize : function(s){}
    property var paneCleared : function(){}
    property var paneClone : null
    property var paneFocusItem : null
}
