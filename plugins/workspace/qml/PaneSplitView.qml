import QtQuick 2.3
import QtQuick.Controls 1.2
import editor 1.0

SplitView{
    id: splitRoot
    orientation: Qt.Horizontal

    property color handleColor: '#081019'

    handleDelegate: Rectangle{
        implicitWidth: 1
        implicitHeight: 1
        color: splitRoot.handleColor
    }
    objectName: "splitview"

    property var currentWindow : null

    property string paneType : "splitview"

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

    property var mapGlobalPosition: function(){
        if ( parentSplitView ){
            var parentPoint = parentSplitView.mapGlobalPosition()
            return Qt.point(parentPoint.x + x, parentPoint.y + y)
        }
        return Qt.point(x, y)
    }

    property var createNewSplitter : function(){}

    property int paneMinimumWidth: 100
    property int paneMinimumHeight: 100

    property var panes : []
    property var paneSizes : []
    property var paneCleared : function(){}

    onResizingChanged: {
        if ( !resizing )
            __updatePaneSizes()
    }

    onWidthChanged: {
        if ( panes.length === 0 || !parent )
            return

        if ( orientation === Qt.Vertical )
            return

        var totalSize = 0
        for ( var i = 0; i < panes.length; ++i ){
            totalSize += paneSizes[i]
        }

        if ( totalSize === 0 )
            return

        for ( var j = 0; j < panes.length; ++j ){
            var paneSizeRatio = paneSizes[j] / totalSize
            var pane = panes[j]

            var newPaneSize = paneSizeRatio * width
            paneSizes[j] = newPaneSize
            pane.width = newPaneSize

//            if ( pane.width < pane.paneMinimumWidth ){
//                paneSizes[j] = pane.paneMinimumWidth
//                pane.width = pane.paneMinimumWidth
//            }
        }


        var totalSize = 0
        for ( var i = 0; i < panes.length; ++i ){
            totalSize += paneSizes[i]
        }

        // remove panes that don't fit

        var downJ = panes.length - 1
        while ( downJ > -1 ){
            if ( panes[downJ].x + panes[downJ].width > splitRoot.width ){
                splitRoot.removeAt(downJ)
            } else {
                return
            }
            downJ--
        }
    }

    onHeightChanged: {
        if ( panes.length === 0 || !parent )
            return

        if ( orientation === Qt.Horizontal )
            return

        var totalSize = 0
        for ( var i = 0; i < panes.length; ++i ){
            totalSize += paneSizes[i]
        }

        if ( totalSize === 0 )
            return

        for ( var j = 0; j < panes.length; ++j ){
            var paneSizeRatio = paneSizes[j] / totalSize
            var pane = panes[j]

            var newPaneSize = paneSizeRatio * height
            paneSizes[j] = newPaneSize
            pane.height = newPaneSize

//            if ( pane.height < pane.paneMinimumHeight ){
//                paneSizes[j] = pane.paneMinimumHeight
//                pane.height = pane.paneMinimumHeight
//            }
        }

        var totalSize = 0
        for ( var i = 0; i < panes.length; ++i ){
            totalSize += paneSizes[i]
        }

        // remove panes that don't fit

        var downJ = panes.length - 1
        while ( downJ > -1 ){
            if ( panes[downJ].y + panes[downJ].height > splitRoot.height ){
                splitRoot.removeAt(downJ)
            } else {
                return
            }
            downJ--
        }
    }

    function __findLocationToFitPane(pane, options){
        if ( options && options.hasOwnProperty('orientation') ){
            if ( options.orientation === splitRoot.orientation )
                if ( __canFitPane(pane) )
                    return splitRoot
        } else {
            if ( __canFitPane(pane) )
                return splitRoot
        }

        for ( var i = 0; i < panes.length; ++i ){
            if ( panes[i].paneType === "splitview" ){
                var fit = panes[i].__findLocationToFitPane(pane)
                if ( fit )
                    return fit
            }
        }
        return null
    }

    function __canFitPane(pane){
        if ( orientation === Qt.Horizontal ){
            var minimumWidth = 0
            for ( var i = 0; i < panes.length; ++i ){
                minimumWidth += panes[i].paneMinimumWidth
            }

            return minimumWidth + pane.paneMinimumWidth < width
        } else {
            var minimumHeight = 0
            for ( var i = 0; i < panes.length; ++i ){
                minimumHeight += panes[i].paneMinimumHeight
            }

            return minimumHeight + pane.paneMinimumHeight < height
        }
    }

    function resizePane(pane, newSize){
        if ( panes.length === 0 )
            return

        var totalSize = 0
        for ( var i = 0; i < panes.length; ++i ){
            totalSize += paneSizes[i]
        }

        for ( var i = 0; i < panes.length; ++i ){
            if ( panes[i] === pane ){

                var newTotalSize = totalSize - paneSizes[i] + newSize

                if ( splitRoot.orientation === Qt.Vertical ){


                    if ( newTotalSize > totalSize ){
                        var newSizeDiff = newSize - totalSize

                        panes[i].height = newSize
                        paneSizes[i] = newSize

                        // resize panes and make sure they exceed the minimum height

                        var downJ = panes.length - 1
                        while ( downJ > -1 ){
                            var currentPaneHeight = panes[downJ].height
                            panes[downJ].height -= newSizeDiff
                            if ( panes[downJ].height > panes[downJ].paneMinimumHeight ){
                                paneSizes[downJ] = panes[downJ].height
                                return
                            } else {
                                panes[downJ].height = panes[downJ].paneMinimumHeight
                                paneSizes[downJ] = panes[downJ].height
                                newSizeDiff -= (currentPaneHeight - panes[downJ].height)
                            }

                            downJ--
                        }

                        if ( newSizeDiff > 0 ){

                            // remove panes that don't fit

                            var downJ = panes.length - 1
                            while ( downJ > -1 ){
                                if ( panes[downJ].y + panes[downJ].height > splitRoot.height ){
                                    splitRoot.removeAt(downJ)
                                } else {
                                    return
                                }
                                downJ--
                            }
                        }

                    } else if ( newTotalSize < totalSize ){
                        panes[i].height = newSize
                        paneSizes[i] = newSize

                        panes[panes.length - 1].height += totalSize - newSize
                        paneSizes[pane.length - 1] = panes[panes.length - 1].height
                    }

                } else {

                    if ( newTotalSize > totalSize ){
                        var newSizeDiff = newSize - totalSize

                        panes[i].width = newSize
                        paneSizes[i] = newSize

                        // resize panes and make sure they exceed the minimum width

                        var downJ = panes.length - 1
                        while ( downJ > -1 ){
                            var currentPaneWidth = panes[downJ].width
                            panes[downJ].width -= newSizeDiff
                            if ( panes[downJ].width > panes[downJ].paneMinimumWidth ){
                                paneSizes[downJ] = panes[downJ].width
                                return
                            } else {
                                panes[downJ].width = panes[downJ].paneMinimumWidth
                                paneSizes[downJ] = panes[downJ].width
                                newSizeDiff -= (currentPaneWidth - panes[downJ].width)
                            }

                            downJ--
                        }

                        // remove panes that don't fit

                        var downJ = panes.length - 1
                        while ( downJ > -1 ){
                            if ( panes[downJ].x + panes[downJ].width > splitRoot.width ){
                                splitRoot.removeAt(downJ)
                            } else {
                                return
                            }
                            downJ--
                        }

                    } else if ( newTotalSize < totalSize ){
                        panes[i].width = newSize
                        paneSizes[i] = newSize

                        panes[panes.length - 1].width += totalSize - newSize
                        paneSizes[pane.length - 1] = panes[panes.length - 1].width
                    }
                }

                return
            }
        }
    }

    function __updatePaneSizes(){
        for ( var i = 0; i < panes.length; ++i ){
            paneSizes[i] = orientation === Qt.Horizontal ? panes[i].width : panes[i].height
        }
    }

    function __splitPaneForVertical(i, item, positionBefore){
        if ( i === 0 && panes.length === 0 ){
            splitRoot.addItem(item)
            item.parentSplitView = splitRoot
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

        item.parentSplitView = splitRoot
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
            item.parentSplitView = splitRoot
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

        item.parentSplitView = splitRoot
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
            panes[j].parentSplitView = this
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

    function findPanesByType(paneType){
        var result = []
        for ( var j = 0; j < panes.length; ++j ){
            if ( panes[j].paneType === "splitview" ){
                result = result.concat(panes[j].findPanesByType(paneType))
            } else if ( panes[j].paneType === paneType ) {
                result.push(panes[j])
            }
        }
        return result
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

        item.parentSplitView = splitRoot
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
            panes[i].paneCleared()
        }

        panes = []
        paneSizes = []
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
