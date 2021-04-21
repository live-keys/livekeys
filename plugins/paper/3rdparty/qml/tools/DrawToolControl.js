
function create(paperCanvas, pg, onStateUpdate){

    var paths = []
    var lineCount = []

    var lastMousePos
    var lastMouseDownPos

    var options = {
        pointDistance: 20,
        drawParallelLines: false,
        lines: 3,
        lineDistance: 10,
        closePath: 'near start',
        smoothPath : true
    }

    var onMouseDown = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        if (options.drawParallelLines) {
            lineCount = options.lines;
        } else {
            lineCount = 1;
        }

        for( var i=0; i < lineCount; i++) {
            var path = paths[i];
            path = new paperCanvas.paper.Path();
            path = pg.styles.applyActiveStyle(path)
            paths.push(path);
        }

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        lastMouseDownPos = lastMousePos
        paperCanvas.paint()
    }

    var onMouseDrag = function(event){
        if ( !(event.buttons & Qt.LeftButton) )
            return;

        var eventDelta = lastMousePos
                ? new paperCanvas.paper.Point(event.x - lastMousePos.x, event.y - lastMousePos.y)
                : new paperCanvas.paper.Point(0, 0)


        var offset = eventDelta;
        offset.angle += 90;
        for( var i=0; i < lineCount; i++) {
            var path = paths[i];
            offset.length = options.lineDistance * i;
            path.add(new paperCanvas.paper.Point(event.x + offset.x, event.y + offset.y));
        }

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    var onMouseUp = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        // accidental clicks produce a path but no segments
        // so return if an accidental click happened
        if(paths[0].segments.length === 0)
            return;

        var group;
        if(lineCount > 1) {
            group = new paperCanvas.paper.Group();
        }

        var nearStart = pg.math.checkPointsClose(
            paths[0].firstSegment.point, new paperCanvas.paper.Point(event.x, event.y), 30);

        for( var i=0; i < lineCount; i++) {
            var path = paths[i];

            if(options.closePath === 'near start' && nearStart) {
                path.closePath(true);
            } else if(options.closePath === 'always') {
                path.closePath(true);
            }
            if(options.smoothPath)
                path.smooth();

            if(lineCount > 1) {
                group.addChild(path);
            }
        }

        paperCanvas.paint()
        paths = [];
        pg.undo.snapshot('draw');
    }

    return {
        options: options,
        onMouseDown : onMouseDown,
        onMouseUp : onMouseUp,
        onMouseDrag : onMouseDrag
    }
}
