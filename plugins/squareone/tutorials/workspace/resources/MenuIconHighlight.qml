import QtQuick 2.9
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceControl{

    property Component highlightFactory : Highlighter{}

    function createHighlight(state){
        var ob = lk.layers.window.dialogs.overlayBox(highlightFactory.createObject())
        ob.centerBox = false
        ob.backgroundVisible = false
        ob.box.visible = true

        if ( !state.currentHighlight )
            state.currentHighlight = []
        state.currentHighlight.push(ob)

        return ob
    }

    run: function(workspace, state, fragment){
        var highlight = null
        if ( fragment === 'logo' ){
            highlight = createHighlight(state)
            highlight.boxX = 5
            highlight.boxY = -10
        } else if ( fragment === 'new-project' ){
            highlight = createHighlight(state)
            highlight.boxX = 115
            highlight.boxY = -10
        } else if ( fragment === 'open-project-file' ){
            highlight = createHighlight(state)
            highlight.boxX = 115 + 35*2
            highlight.boxY = -10
        } else if ( fragment === 'open-project-folder' ){
            highlight = createHighlight(state)
            highlight.boxX = 115 + 35*3
            highlight.boxY = -10
        } else if ( fragment === 'save-file-as' ){
            highlight = createHighlight(state)
            highlight.boxX = 115 + 35
            highlight.boxY = -10
        } else if ( fragment === 'log-view' ){
            highlight = createHighlight(state)
            highlight.boxX = 220 + 35 * 3 - 15
            highlight.boxY = -10
        } else if ( fragment === 'command-view' ){
            highlight = createHighlight(state)
            highlight.boxX = 220 + 35 * 4 - 15
            highlight.boxY = -10
        } else if ( fragment === 'settings-view' ){
            highlight = createHighlight(state)
            highlight.boxX = 220 + 35 * 5 - 15
            highlight.boxY = -10
        } else if ( fragment === 'license-view' ){
            highlight = createHighlight(state)
            highlight.boxX = 220 + 35 * 6 - 15
            highlight.boxY = -10
        } else if ( fragment === 'active-file' ){
            highlight = createHighlight(state)
            highlight.boxX = 520
            highlight.box.width = 220
            highlight.boxY = -10
        } else if ( fragment === 'header' ){
            highlight = createHighlight(state)
            highlight.boxX = 5
            highlight.boxY = 0
            highlight.box.width = lk.layers.window.handle.size().width - 10
            highlight.box.height = 33
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'
        } else if ( fragment === 'pane-area' ){
            highlight = createHighlight(state)
            highlight.boxX = 5
            highlight.boxY = 30
            highlight.box.width = lk.layers.window.handle.size().width - 10
            highlight.box.height = lk.layers.window.handle.size().height - 30
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'
        }
    }

}
