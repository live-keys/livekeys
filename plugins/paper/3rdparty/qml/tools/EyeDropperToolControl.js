
function create(paperCanvas, pg, onStateUpdate){

    var hitOptions = {
        segments: true,
        stroke: true,
        curves: true,
        fill: true,
        guide: false,
        tolerance: 5 / paperCanvas.paper.view.zoom
    };

    var onMouseDown = function(event){
        if ( !(event.buttons & Qt.LeftButton) )
            return;

        var hitResult = paperCanvas.paper.project.hitTest(new paperCanvas.paper.Point(event.x, event.y), hitOptions);
        if (hitResult) {
            if(event.modifiers & Qt.AltModifier) {
                pg.undo.snapshot('applyToolbarStyles');
                //TODO
//                pg.stylebar.applyActiveToolbarStyle(hitResult.item);
            } else {
                var item = hitResult.item

                if(item.fillColor) {
                    pg.styleBar.foreground = pg.color.fromPaper(item.fillColor)
                } else {
                    pg.styleBar.foreground = 'transparent'
                }
                if(item.strokeColor) {
                    pg.styleBar.background = pg.color.fromPaper(item.strokeColor)
                } else {
                    pg.styleBar.background = 'transparent'
                }

                pg.styleBar.setup({strokeSize: item.strokeWidth})

                if ( item.blendMode )
                    pg.styleBar.blendMode = item.blendMode
                else
                    pg.styleBar.blendMode = 'normal'

                pg.styleBar.opacity = item.opacity
            }
        }
    }

    var onMouseMove = function(event){
        pg.hover.handleHoveredItem(hitOptions, {
            point: { x: event.x, y: event.y }
        });
        paperCanvas.paint()
    }

    return {
        options: {},
        onMouseDown : onMouseDown,
        onMouseMove : onMouseMove
    }
}
