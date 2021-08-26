import QtQuick 2.3
import workspace 1.0
import base 1.0 as Base

QtObject{
    id: root
    property Item activePane : null
    property Item activeItem : null
    property Item container : null

    property Item paneDropArea: null
    property ContextMenu contextMenu:  null

//    property var logsOpened: []

    property QtObject style: QtObject{
        property color splitterColor: 'transparent'
    }

    property var openWindows : []

    property QtObject __factories: QtObject{

        property ProjectFileSystem projectFileSystem: ProjectFileSystem{
            id: projectView
            width: 300
            panes: root
        }

        property Item viewer : Viewer {
            id: viewer
            panes: root
        }

        property Component runView: Component{
            RunView{
                id: runViewComponent
                panes: root
            }
        }

        property Component documentation: Component{
            DocumentationViewPane{
                id: documentationViewComponent
                panes: root
            }
        }

        property Component log: Component{
            LogContainer{
                id: logView
                isInWindow: false
                width: 300
                height: 200
            }
        }

        property LogContainer mainLog : LogContainer{
            visible: false
            isInWindow: false
            width: 300
            height: 200

            //TODO: Fix this

            onItemAdded: {
                if ( !parent  )
                    header.isLogWindowDirty = true
            }
        }
    }

    property var types : {
        return {
            "projectFileSystem" : {
                create : function(p, s){
                    if ( s )
                        root.__factories.projectFileSystem.paneInitialize(s)
                    return root.__factories.projectFileSystem
                },
                single: true
            },
            "viewer" : {
                create: function(p, s){
                    return root.__factories.viewer
                },
                single: true
            },
            "runView" : {
                create: function(p, s){
                    var pane = root.__factories.runView.createObject(p)
                    if ( s )
                        pane.paneInitialize(s)
                    return pane
                },
                single: false
            },
            "documentation" : {
                create: function(p, s){
                    var pane = root.__factories.documentation.createObject(p)
                    if ( s )
                        pane.paneInitialize(s)
                    return pane
                },
                single: false
            },
            "log" : {
                create: function(p, s){
                    if ( !root.__factories.mainLog.parent ){
                        root.__factories.mainLog.visible = true
                        root.__factories.mainLog.parent = p
                        header.isLogWindowDirty = false
                        return root.__factories.mainLog
                    } else {
                        var pane = root.__factories.log.createObject(p)
                        return pane
                    }
                },
                single: false
            }
        }
    }

    property var initializePanes: function(windowConfiguration, paneConfiguration){
        lk.layers.workspace.startup.close()

        var currentWindowPanes = paneConfiguration[0]

        var initializeStructure = [
            [lk.layers.window.window()],
            [initializeSplitterPanes(container, currentWindowPanes)]
        ]

        for ( var i = 1; i < paneConfiguration.length; ++i ){
            var window = paneWindowFactory.createObject(root)
            openWindows.push(window)
            initializeStructure[0].push(window)
            initializeStructure[1].push(initializeSplitterPanes(window.mainSplit, paneConfiguration[i]))

            window.closing.connect(function(){
                var index = openWindows.indexOf(window);
                if (index > -1) {
                    openWindows.splice(index, 1);
                }
            })
        }

        return initializeStructure
    }

    property var createPane: function(paneType, paneState, paneSize){

        if ( paneType in root.types ){
            var paneFactory = root.types[paneType]
            var paneObject = paneFactory.create(null, paneState)
            if ( paneSize ){
                paneObject.width = paneSize[0]
                paneObject.height = paneSize[1]
            }

            return paneObject

        } else {
            throw new Error('Pane type not found: ' + paneType)
        }
    }

    property var movePaneToNewWindow : function(pane){
        removePane(pane)

        var window = paneWindowFactory.createObject(root)
        openWindows.push(window)

        window.closing.connect(function(){
            var index = openWindows.indexOf(window);
            if (index > -1) {
                openWindows.splice(index, 1);
            }
        })

        lk.layers.workspace.addWindow(window)

        splitPaneVerticallyWith(window.mainSplit, 0, pane)
    }

    property var splitPaneHorizontallyWith : function(splitter, index, pane){
        splitter.splitHorizontally(index, pane)

        lk.layers.workspace.addPane(pane, pane.paneWindow(), pane.splitterHierarchyPositioning())
    }

    property var splitPaneVerticallyWith : function(splitter, index, pane){
        splitter.splitVertically(index, pane)

        lk.layers.workspace.addPane(pane, pane.paneWindow(), pane.splitterHierarchyPositioning())
    }

    property var splitPaneHorizontallyBeforeWith : function(splitter, index, pane){
        splitter.splitHorizontallyBefore(index, pane)

        lk.layers.workspace.addPane(pane, pane.paneWindow(), pane.splitterHierarchyPositioning())
    }

    property var splitPaneVerticallyBeforeWith : function(splitter, index, pane){
        splitter.splitVerticallyBefore(index, pane)

        lk.layers.workspace.addPane(pane, pane.paneWindow(), pane.splitterHierarchyPositioning())
    }

    property var splitPaneHorizontally : function(pane, newPane){
        var index = pane.parentSplitViewIndex()
        splitPaneHorizontallyWith(pane.parentSplitView, index, newPane)
    }
    property var splitPaneVertically : function(pane, newPane){
        var index = pane.parentSplitViewIndex()
        splitPaneVerticallyWith(pane.parentSplitView, index, newPane)
    }
    property var splitPaneHorizontallyBefore : function(pane, newPane){
        var index = pane.parentSplitViewIndex()
        splitPaneHorizontallyBeforeWith(pane.parentSplitView, index, newPane)
    }
    property var splitPaneVerticallyBefore : function(pane, newPane){
        var index = pane.parentSplitViewIndex()
        splitPaneVerticallyBeforeWith(pane.parentSplitView, index, newPane)
    }

    property var insertPaneAtIndex: function(splitView, index, newPane){
        if ( !splitView.__canFitPane(newPane) ){
            throw new Error("Failed to fit pane inside layout.")
        }

        if ( splitView.orientation === Qt.Horizontal ){
            var delta = splitView.width - newPane.width
            if ( delta < 0 ){
                delta = splitView.width - newPane.minimumPaneWidth
                newPane.width = newPane.minimumPaneWidth
            }

            var scale = delta / splitView.width

            var newPaneSizes = splitView.paneSizes.map(function(size){ return size * scale })
            newPaneSizes.splice(index, 0, newPane.width)

            splitView.__splitPaneForHorizontal(index, newPane, true)

            // resize all panes smaller than minimum width
            for ( var i = 0; i < splitView.panes.length; ++i ){
                if ( newPaneSizes[i] < splitView.panes[i].paneMinimumWidth ){
                    newPaneSizes[i] = splitView.panes[i].paneMinimumWidth
                }
            }

            var totalSize = 0;
            for ( var i = 0; i < newPaneSizes.length; ++i ){
                totalSize += newPaneSizes[i]
            }
            var sizeLeft = totalSize - splitView.width

            // due to the previous resize, panes might exceed splitView width and need to be resized
            for ( var i = newPaneSizes.length - 1; i >= 0; --i ){
                if ( sizeLeft < 0 )
                    break;

                if ( newPaneSizes[i] > splitView.panes[i].paneMinimumWidth){
                    sizeLeft -= (newPaneSizes[i] - splitView.panes[i].paneMinimumWidth)
                    newPaneSizes[i] = splitView.panes[i].paneMinimumWidth
                }
            }

            for ( var i = 0; i < splitView.panes.length; ++i ){
                splitView.panes[i].width = newPaneSizes[i]
                splitView.paneSizes[i] = newPaneSizes[i]
            }

        } else {
            var delta = splitView.height - newPane.height
            if ( delta < 0 ){
                delta = splitView.height - newPane.minimumPaneHeight
                newPane.height = newPane.minimumPaneHeight
            }

            var scale = delta / splitView.height

            var newPaneSizes = splitView.paneSizes.map(function(size){ return size * scale })
            newPaneSizes.splice(index, 0, newPane.height)

            splitView.__splitPaneForVertical(index, newPane, true)

            // resize all panes smaller than minimum height
            for ( var i = 0; i < splitView.panes.length; ++i ){
                if ( newPaneSizes[i] < splitView.panes[i].paneMinimumHeight ){
                    newPaneSizes[i] = splitView.panes[i].paneMinimumHeight
                }
            }

            var totalSize = 0;
            for ( var i = 0; i < newPaneSizes.length; ++i ){
                totalSize += newPaneSizes[i]
            }
            var sizeLeft = totalSize - splitView.height

            // due to the previous resize, panes might exceed splitView height and need to be resized
            for ( var i = newPaneSizes.length - 1; i >= 0; --i ){
                if ( sizeLeft < 0 )
                    break;

                if ( newPaneSizes[i] > splitView.panes[i].paneMinimumHeight){
                    sizeLeft -= (newPaneSizes[i] - splitView.panes[i].paneMinimumHeight)
                    newPaneSizes[i] = splitView.panes[i].paneMinimumHeight
                }
            }

            for ( var i = 0; i < splitView.panes.length; ++i ){
                splitView.panes[i].height = newPaneSizes[i]
                splitView.paneSizes[i] = newPaneSizes[i]
            }
        }

        lk.layers.workspace.addPane(newPane, newPane.paneWindow(), newPane.splitterHierarchyPositioning())
    }
    property var insertPane: function(newPane, options){
        var splitViewToFit = root.container.__findLocationToFitPane(newPane, options)
        if ( !splitViewToFit ){
            throw new Error("Failed to fit pane inside layout.")
        }

        root.insertPaneAtIndex(splitViewToFit, splitViewToFit.panes.length, newPane)
    }

    property var canFitPane : function(splitView, pane){
        return splitView.__canFitPane(pane)
    }

    property var paneTypes: function(){
        var result = []
        for (var key in root.types) {
            // check if the property/key is defined in the object itself, not in parent
            if (root.types.hasOwnProperty(key)) {
                result.push({name: key, single: root.types[key].single})
            }
        }
        return result
    }

    function clearPane(pane){
        removePane(pane)
        pane.paneCleared()
    }

    function removePane(pane){
        if ( pane.parentSplitView ){
            var split = pane.parentSplitView
            var paneIndex = split.paneIndex(pane)

            if ( pane.paneType !== 'splitview' )
                lk.layers.workspace.removePane(pane)

            split.removeAt(paneIndex)

            if ( root.activePane === pane ){
                root.activePane = null
                root.activeItem = null
            }

            if ( split.panes.length === 0 && split !== root.container ){
                removePane(split)
            }
        } else if ( pane.paneType === 'splitview' ){
            if ( pane.currentWindow !== lk.layers.window.window() ){
                pane.currentWindow.close()
            }
        }
    }

    function reset(callback){
        __clearAll()
        var split = root.container.createNewSplitter(Qt.Horizontal)
        root.container.initialize([split])
        if ( callback )
            Base.Time.delay(0, callback)
    }

    function focusPane(paneType){
        var ap = root.activePane
        if ( ap && ap.paneType === paneType ){
            return ap
        }

        var pane = root.container.findPaneByType(paneType)
        if ( pane )
            return pane

        for ( var i = 0; i < root.openWindows.length; ++i ){
            var w = root.openWindows[i]
            var p = w.mainSplit.findPaneByType(paneType)
            if ( p )
                return p
        }
        return null
    }

    function findPanesByType(paneType){
        var result = []
        result = result.concat(root.container.findPanesByType(paneType))

        for ( var i = 0; i < root.openWindows.length; ++i ){
            var w = root.openWindows[i]
            result = result.concat(w.mainSplit.findPanesByType(paneType))
        }
        return result
    }

    function setActiveItem(item, pane){
        activeItem = item
        var p = pane ? pane : item
        while ( p !== null ){
            if ( p.objectName === 'editor' || p.objectName === 'project' || p.objectName === 'viewer' ){
                activePane = p
                return
            }
            p = p.parent
        }
    }

    function activateItem(item, pane){
        if ( activeItem && activeItem !== item ){
            activeItem.focus = false
        }

        activeItem = item
        activeItem.forceActiveFocus()
        var p = pane ? pane : item
        while ( p !== null ){
            if ( p.objectName === 'editor' || p.objectName === 'project' || p.objectName === 'viewer' ){
                activePane = p
                return
            }
            p = p.parent
        }
    }

    function openContextMenu(item, pane){
        if ( !item )
            item = activeItem
        if ( !pane )
            pane = activePane

        var res = lk.layers.workspace.interceptMenu(pane, item)
        contextMenu.show(res)
    }

    property var __clearAll: function(){
        root.container.clearPanes()
        activePane = null
        activeItem = null
    }

    function __dragStarted(pane){
        for ( var i = 0; i < openWindows.length; ++i ){
            var w = openWindows[i]
            w.paneDropArea.currentPane = pane
            w.paneDropArea.model = w.mainSplit.createPositioningModel()
            w.paneDropArea.visible = true
        }

        if (typeof pane === 'function') {
            root.paneDropArea.paneFactory = pane
        } else {
            root.paneDropArea.currentPane = pane
        }

        root.paneDropArea.model = root.container.createPositioningModel()
        root.paneDropArea.visible = true
    }

    function __dragFinished(pane){
        root.paneDropArea.currentPane = null
        root.paneDropArea.paneFactory = null
        root.paneDropArea.model = []
        for ( var i = 0; i < openWindows.length; ++i ){
            var w = openWindows[i]
            w.paneDropArea.currentPane = null
            w.paneDropArea.model = []
            w.paneDropArea.visible = false
        }
        root.paneDropArea.visible = false
    }

    function initializeSplitterPanes(splitter, paneConfiguration){
        var orientation = paneConfiguration[0] === 'h' ? Qt.Horizontal : Qt.Vertical
        splitter.orientation = orientation

        if ( splitter !== root.container ){
            if ( orientation === Qt.Vertical && paneConfiguration.length > 1 ){
                splitter.width = paneConfiguration[1].width
            } else {
                splitter.height = paneConfiguration[1].width
            }
        }

        var panesToOpen = []
        var recurseSplitters = []

        for ( var i = 1; i < paneConfiguration.length; ++i ){

            if ( Array.isArray(paneConfiguration[i]) ){
                var split = splitter.createNewSplitter(Qt.Vertical)
                panesToOpen.push(split)
                recurseSplitters.push(i)
            } else {
                var state = 'state' in paneConfiguration[i] ? paneConfiguration[i].state : null
                var pane = createPane(paneConfiguration[i].type, state, paneConfiguration[i].size)
                panesToOpen.push(pane)
            }
        }

        splitter.initialize(panesToOpen)

        for ( var i = 0; i < recurseSplitters.length; ++i ){
            var configIndex = recurseSplitters[i]
            initializeSplitterPanes(panesToOpen[configIndex - 1], paneConfiguration[configIndex])
        }

        splitter.__updatePaneSizes()

        return panesToOpen
    }
}
