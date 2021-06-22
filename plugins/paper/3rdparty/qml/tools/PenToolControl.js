
function create(paperCanvas, pg, onStateUpdate){

    var path

    var currentSegment
    var mode
    var type
    var hoveredItem = null
    var hitOptions = {
        segments: true,
        stroke: true,
        curves: true,
        guide: false,
        tolerance: 5 / paperCanvas.paper.view.zoom
    }

    var lastMousePos

    var findHandle = function(path, point) {
        var types = ['point', 'handleIn', 'handleOut'];
        for (var i = 0, l = path.segments.length; i < l; i++) {
            for (var j = 0; j < 3; j++) {
                var type = types[j];
                var segment = path.segments[i];
                var segmentPoint = type === 'point'
                        ? segment.point
                        : segment.point + segment[type];
                var distance = (point - segmentPoint).length;
                if (distance < 6) {
                    return {
                        type: type,
                        segment: segment
                    };
                }
            }
        }
        return null;
    };

    var onMouseDown = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        if (currentSegment) {
            currentSegment.selected = false;
        }
        mode = null
        currentSegment = null
        type = null

        if(!path) {
            if(!hoveredItem) {
                pg.selection.clearSelection();
                path = new paperCanvas.paper.Path();
                path = pg.styles.applyActiveStyle(path)

            } else {
                if(!hoveredItem.item.closed) {
                    mode = 'continue';
                    path = hoveredItem.item;
                    currentSegment = hoveredItem.segment;
                    if(hoveredItem.item.lastSegment !== hoveredItem.segment) {
                        path.reverse();
                    }

                } else {
                    path = hoveredItem.item;
                }
            }

        }

        if(path) {
            var result = findHandle(path, new paperCanvas.paper.Point(event.x, event.y));
            if (result && mode !== 'continue') {
                currentSegment = result.segment;
                type = result.type;
                if (result.type === 'point') {
                    if( result.segment.index === 0 &&
                        path.segments.length > 1 &&
                        !path.closed) {
                        mode = 'close';
                        path.closed = true;
                        path.firstSegment.selected = true;

                    } else {
                        mode = 'remove';
                        result.segment.remove();

                    }
                }
            }


            if (!currentSegment) {
                if(hoveredItem) {
                    if(hoveredItem.type === 'segment' &&
                        !hoveredItem.item.closed) {

                        // joining two paths
                        var hoverPath = hoveredItem.item;
                        // check if the connection point is the first segment
                        // reverse path if it is not because join()
                        // always connects to first segment)
                        if(hoverPath.firstSegment !== hoveredItem.segment) {
                            hoverPath.reverse();
                        }
                        path.join(hoverPath);
                        path = null;

                    } else if(hoveredItem.type === 'curve' ||
                        hoveredItem.type === 'stroke') {

                        mode = 'add';
                        // inserting segment on curve/stroke
                        var location = hoveredItem.location;
                        currentSegment = path.insert(location.index + 1, new paperCanvas.paper.Point(event.x, event.y));
                        currentSegment.selected = true;
                    }

                } else {
                    mode = 'add';
                    // add a new segment to the path
                    currentSegment = path.add(new paperCanvas.paper.Point(event.x, event.y));
                    currentSegment.selected = true;

                }
            }


        }

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    var onMouseDrag = function(event){
        if ( !(event.buttons & Qt.LeftButton) )
            return;

        var eventDelta = lastMousePos
                ? new paperCanvas.paper.Point(event.x - lastMousePos.x, event.y - lastMousePos.y)
                : new paperCanvas.paper.Point(0, 0)


        var delta = eventDelta;
        if (type === 'handleOut' || mode === 'add') {
            delta.x = -delta.x
            delta.y = -delta.y
        }
        currentSegment.handleIn.x += delta.x;
        currentSegment.handleIn.y += delta.y;
        currentSegment.handleOut.x -= delta.x;
        currentSegment.handleOut.y -= delta.y;

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    var onMouseMove = function(event){
        var hitResult = paperCanvas.paper.project.hitTest(new paperCanvas.paper.Point(event.x, event.y), hitOptions);
        if(hitResult && hitResult.item && hitResult.item.selected) {
            hoveredItem = hitResult;
        } else {
            hoveredItem = null;
        }

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    var onMouseUp = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        if(path && path.closed) {
            pg.undo.snapshot('bezier');
            path = null;
        }

        paperCanvas.paint()
    }

    return {
        options: null,
        onMouseDown : onMouseDown,
        onMouseUp : onMouseUp,
        onMouseMove: onMouseMove,
        onMouseDrag : onMouseDrag
    }
}
