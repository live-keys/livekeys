import QtQuick 2.3
import QtQuick.Controls 1.2
import editor 1.0

SplitView{
    id: splitRoot
    orientation: Qt.Horizontal
    handleDelegate: Rectangle{
        implicitWidth: 1
        implicitHeight: 1
        color: "#081019"
    }
    objectName: "splitview"

    property var currentWindow : null

    property string paneType : "splitview"

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

    property var createNewSplitter : function(){}

    property var panes : []
    property var paneSizes : []

    function __splitPaneForVertical(i, item, positionBefore){
        if ( i === 0 && panes.length === 0 ){
            splitRoot.addItem(item)
            item.parentSplitter = splitRoot
            panes.push(item)
            paneSizes.push(item.height)
            return
        }

        if ( i > panes.length )
            throw new Error("Index out of bounds: " + i + "(" + panes.length + ")")

        var h = 0;

        for ( var j = 0; j < panes.length; ++j ){
            if ( j === i ){
                h = panes[j].height / 2
                paneSizes[j] = h
            } else {
                paneSizes[j] = panes[j].height
            }
        }

        var insertIndex = positionBefore ? i : i + 1

        item.parentSplitter = splitRoot
        panes.splice(insertIndex, 0, item)
        paneSizes.splice(insertIndex, 0, h)

        for ( var j = insertIndex + 1; j < panes.length; ++j ){
            splitRoot.removeItem(panes[j])
        }

        for ( var j = insertIndex; j < panes.length; ++j ){
            splitRoot.addItem(panes[j])
        }

        for ( var j = 0; j < paneSizes.length; ++j ){
            panes[j].height = paneSizes[j]
        }
    }

    function __splitPaneForHorizontal(i, item, positionBefore){
        if ( i === 0 && panes.length === 0 ){
            splitRoot.addItem(item)
            panes.push(item)
            paneSizes.push(item.width)
            item.parentSplitter = splitRoot
            return
        }

        if ( i > panes.length )
            throw new Error("Index out of bounds: " + i + "(" + panes.length + ")")

        var w = 0;

        for ( var j = 0; j < panes.length; ++j ){
            if ( j === i ){
                w = panes[j].width / 2
                paneSizes[j] = w
            } else {
                paneSizes[j] = panes[j].width
            }
        }

        var insertIndex = positionBefore ? i : i + 1

        item.parentSplitter = splitRoot
        panes.splice(insertIndex, 0, item)
        paneSizes.splice(insertIndex, 0, w)

        for ( var j = insertIndex + 1; j < panes.length; ++j ){
            splitRoot.removeItem(panes[j])
        }

        for ( var j = insertIndex; j < panes.length; ++j ){
            splitRoot.addItem(panes[j])
        }

        for ( var j = 0; j < paneSizes.length; ++j ){
            panes[j].width = paneSizes[j]
        }
    }

    function splitPane(i, item){
        if ( orientation === Qt.Vertical ){
            __splitPaneForVertical(i, item, false)
        } else {
            __splitPaneForHorizontal(i, item, false)
        }
    }

    function splitPaneBefore(i, item){
        if ( orientation === Qt.Vertical ){
            __splitPaneForVertical(i, item, true)
        } else {
            __splitPaneForHorizontal(i, item, true)
        }
    }

    function splitPaneCounterOrientation(i, item){
        if ( i > panes.length )
            throw new Error("Index out of bounds: " + i + "(" + panes.length + ")")

        var newSplit = createNewSplitter(splitRoot.orientation === Qt.Vertical ? Qt.Horizontal : Qt.Vertical)

        var paneToReplace = paneAt(i)

        replacePane(i, newSplit)

        newSplit.splitPane(0, paneToReplace)
        newSplit.splitPane(0, item)
    }

    function splitPaneCounterOrientationBefore(i, item){
        if ( i > panes.length )
            throw new Error("Index out of bounds: " + i + "(" + panes.length + ")")

        var newSplit = createNewSplitter(splitRoot.orientation === Qt.Vertical ? Qt.Horizontal : Qt.Vertical)

        var paneToReplace = paneAt(i)

        replacePane(i, newSplit)

        newSplit.splitPane(0, item)
        newSplit.splitPane(0, paneToReplace)
    }

    function splitHorizontally(i, pane){
        if ( orientation === Qt.Horizontal ){
            splitPane(i, pane)
        } else {
            splitPaneCounterOrientation(i, pane)
        }
    }

    function splitVertically(i, pane){
        if ( orientation === Qt.Vertical ){
            splitPane(i, pane)
        } else {
            splitPaneCounterOrientation(i, pane)
        }
    }

    function splitHorizontallyBefore(i, pane){
        if ( orientation === Qt.Horizontal ){
            splitPaneBefore(i, pane)
        } else {
            splitPaneCounterOrientationBefore(i, pane)
        }
    }

    function splitVerticallyBefore(i, pane){
        if ( orientation === Qt.Vertical ){
            splitPaneBefore(i, pane)
        } else {
            splitPaneCounterOrientationBefore(i, pane)
        }
    }


    function initialize(paneArray){
        panes = paneArray

        if ( orientation === Qt.Horizontal ){
            for ( var j = 0; j < panes.length; ++j ){
                paneSizes.push(panes[j].width)
            }
        } else {
            for ( var j = 0; j < panes.length; ++j ){
                paneSizes.push(panes[j].height)
            }
        }

        for ( var j = 0; j < panes.length; ++j ){
            panes[j].parentSplitter = this
            splitRoot.addItem(panes[j])
        }

        if ( orientation === Qt.Horizontal ){
            for ( var j = 0; j < panes.length; ++j ){
                panes[j].width = paneSizes[j]
            }
        } else{
            for ( var j = 0; j < panes.length; ++j ){
                panes[j].height = paneSizes[j]
            }
        }
    }

    function findPaneByType(paneType){
        for ( var j = 0; j < panes.length; ++j ){
            if ( panes[j].paneType === "splitview" ){
                var nestedPane = panes[j].findPaneByType(paneType)
                if ( nestedPane )
                    return nestedPane
            } else if ( panes[j].paneType === paneType )
                return panes[j];
        }
        return null
    }

    function paneAt(i){
        return panes[i]
    }

    function paneIndex(item){
        return panes.indexOf(item)
    }

    function replacePane(i, item){
        if ( orientation === Qt.Horizontal ){
            for ( var j = 0; j < panes.length; ++j ){
                paneSizes[j] = panes[j].width
            }
        } else {
            for ( var j = 0; j < panes.length; ++j ){
                paneSizes[j] = panes[j].height
            }
        }

        for ( var j = i; j < panes.length; ++j ){
            splitRoot.removeItem(panes[j])
        }

        item.parentSplitter = splitRoot
        panes[i] = item

        for ( var j = i; j < panes.length; ++j ){
            splitRoot.addItem(panes[j])
        }

        if ( orientation === Qt.Horizontal ){
            for ( var j = 0; j < panes.length; ++j ){
                panes[j].width = paneSizes[j]
            }
        } else{
            for ( var j = 0; j < panes.length; ++j ){
                panes[j].height = paneSizes[j]
            }
        }
    }

    function removeAt(i){
        var pane = panes[i]
        panes.splice(i, 1)
        paneSizes.splice(i, 1)
        splitRoot.removeItem(pane)
    }

    function clearPanes(){
        for ( var i = panes.length - 1; i >= 0; --i ){
            if ( panes[i].paneType === 'splitview' )
                panes[i].clearPanes()
            splitRoot.removeItem(panes[i])
        }

        panes = []
        paneSizes = []
    }

    property var mapGlobalPosition: function(){
        if ( parentSplitter ){
            var parentPoint = parentSplitter.mapGlobalPosition()
            return Qt.point(parentPoint.x + x, parentPoint.y + y)
        }
        return Qt.point(x, y)
    }

    function createPositioningModel(currentPosition){
        var position = currentPosition ? currentPosition : {x:0, y:0}
        var pmodel = []
        for ( var j = 0; j < panes.length; ++j ){
            if ( panes[j].paneType === "splitview" ){
                var retModel = panes[j].createPositioningModel({
                    x : position.x + panes[j].x, y: position.y + panes[j].y
                })
                pmodel = pmodel.concat(retModel)
            } else {
                pmodel.push({
                    x : position.x + panes[j].x, y : position.y + panes[j].y,
                    width : panes[j].width, height: panes[j].height,
                    pane: panes[j]
                })
            }
        }

        return pmodel
    }

    function debugContents(indent){
        var contentsIndent = indent ? indent : ''
        var contents =
            contentsIndent + 'splitview[' + (splitRoot.orientation === Qt.Vertical ? 'vertical' : 'horizontal') + ']'
        contentsIndent += '  '
        for ( var j = 0; j < panes.length; ++j ){
            if ( panes[j].paneType === "splitview" ){
                contents += '\n' + panes[j].debugContents(contentsIndent)
            } else {
                contents += '\n' + contentsIndent + panes[j].paneType + '[' + panes[j].width + ',' + panes[j].height + ']'
            }
        }
        return contents
    }
}
