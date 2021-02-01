
function create(paperCanvas, pg, onStateUpdate){

    var doRectSelection
    var selectionRect

    var hitType
    var hitOptions = {
        segments: true,
        stroke: true,
        curves: true,
        handles: true,
        fill: true,
        guide: false,
        tolerance: 3 / paperCanvas.paper.view.zoom
    }

    var selectionDragged = false

    var lastMousePos
    var lastMouseDownPos

    var onMouseDown = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        selectionDragged = false;

        hitType = null;
        pg.hover.clearHoveredItem();

        var hitResult = paperCanvas.paper.project.hitTest(new paperCanvas.paper.Point(event.x, event.y), hitOptions);
        if (!hitResult) {
            if (!(event.modifiers & Qt.ShiftModifier)) {
                pg.selection.clearSelection();
            }
            doRectSelection = true;
        }

        // dont allow detail-selection of PGTextItem
        if(hitResult && pg.item.isPGTextItem(pg.item.getRootItem(hitResult.item))) {
            return;
        }

        if ( !doRectSelection ){
            if(hitResult.type === 'fill') {

                hitType = 'fill';
                if(hitResult.item.selected) {
                    if(event.modifiers & Qt.ShiftModifier) {
                        hitResult.item.fullySelected = false;
                    }
                    if(event.modifiers & Qt.AltModifier)
                        pg.selection.cloneSelection();

                } else {
                    if(event.modifiers & Qt.ShiftModifier) {
                        hitResult.item.fullySelected = true;
                    } else {
                        paperCanvas.paper.project.deselectAll();
                        hitResult.item.fullySelected = true;


                        if(event.modifiers & Qt.AltModifier)
                            pg.selection.cloneSelection();
                    }
                }

            } else if(hitResult.type === 'segment') {
                hitType = 'point';

                if(hitResult.segment.selected) {
                    // selected points with no handles get handles if selected again
                    hitResult.segment.selected = true;
                    if(event.modifiers & Qt.ShiftModifier) {
                        hitResult.segment.selected = false;
                    }

                } else {
                    if(event.modifiers & Qt.ShiftModifier) {
                        hitResult.segment.selected = true;
                    } else {
                        paperCanvas.paper.project.deselectAll();
                        hitResult.segment.selected = true;
                    }
                }

                if(event.modifiers & Qt.AltModifier)
                    pg.selection.cloneSelection();


            } else if(
                hitResult.type === 'stroke' ||
                hitResult.type === 'curve') {
                hitType = 'curve';

                var curve = hitResult.location.curve;
                if(event.modifiers & Qt.ShiftModifier) {
                    curve.selected = !curve.selected;

                } else if(!curve.selected) {
                    paperCanvas.paper.project.deselectAll();
                    curve.selected = true;
                }

                if(event.modifiers & Qt.AltModifier)
                    pg.selection.cloneSelection();

            } else if(
                hitResult.type === 'handle-in' ||
                hitResult.type === 'handle-out') {
                hitType = hitResult.type;

                if(!(event.modifiers & Qt.AltModifier)) {
                    paperCanvas.paper.project.deselectAll();
                }

                hitResult.segment.handleIn.selected = true;
                hitResult.segment.handleOut.selected = true;
            }
        }

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        lastMouseDownPos = lastMousePos
        paperCanvas.paint()
    }

    var onMouseDoubleClicked = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        if (!(event.modifiers & Qt.ShiftModifier)) {
            pg.selection.clearSelection();
        }

        if ( event.button !== Qt.LeftButton )
            return;

        selectionDragged = false;

        hitType = null;
        pg.hover.clearHoveredItem();

        var hitResult = paperCanvas.paper.project.hitTest(new paperCanvas.paper.Point(event.x, event.y), hitOptions);
        if (!hitResult) {
            return;
        }
        if(hitResult && pg.item.isPGTextItem(pg.item.getRootItem(hitResult.item))) {
            return;
        }

        hitType = 'fill';
        if(hitResult.item.selected) {
            hitResult.item.selected = false;
            hitResult.item.fullySelected = true;
        }
        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    var onMouseMove = function(event){
        pg.hover.handleHoveredItem(hitOptions, {
            point: { x: event.x, y: event.y }
        });
        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    var onMouseDrag = function(event){
        if ( !(event.buttons & Qt.LeftButton) )
            return;

        var paperEvent = paperCanvas.convertToMouseEvent(event)
        paperEvent.delta = lastMousePos
            ? new paperCanvas.paper.Point(event.x - lastMousePos.x, event.y - lastMousePos.y)
            : new paperCanvas.paper.Point(0, 0)
        paperEvent.downPoint = lastMouseDownPos

        if ( selectionRect ){
            selectionRect.remove()
            selectionRect = null
        }

        if(doRectSelection) {

            selectionRect = pg.guides.rectSelect(paperEvent);
            // Remove this rect on the next drag and up event

        } else {
            doRectSelection = false;
            selectionDragged = true;

            var selectedItems = pg.selection.getSelectedItems();
            var dragVector = (paperEvent.point - paperEvent.downPoint);

            for(var i=0; i < selectedItems.length; i++) {
                var item = selectedItems[i];

                if(hitType === 'fill' || !item.segments) {

                    // if the item has a compound path as a parent, don't move its
                    // own item, as it would lead to double movement
                    if(item.parent && pg.compoundPath.isCompoundPath(item.parent)) {
                        continue;
                    }

                    // add the position of the item before the drag started
                    // for later use in the snap calculation
                    if(!item.origPos) {
                        item.origPos = item.position;
                    }

                    if (paperEvent.modifiers.shift) {
                        item.position = item.origPos +
                        pg.math.snapDeltaToAngle(dragVector, Math.PI*2/8);

                    } else {
                        item.position.x += paperEvent.delta.x;
                        item.position.y += paperEvent.delta.y
                    }

                } else {
                    for(var j=0; j < item.segments.length; j++) {
                        var seg = item.segments[j];
                        // add the point of the segment before the drag started
                        // for later use in the snap calculation
                        if(!seg.origPoint) {
                            seg.origPoint = seg.point.clone();
                        }

                        if( seg.selected && (
                            hitType === 'point' ||
                            hitType === 'stroke' ||
                            hitType === 'curve')){

                            if (paperEvent.modifiers.shift) {
                                seg.point = seg.origPoint +
                                pg.math.snapDeltaToAngle(dragVector, Math.PI*2/8);

                            } else {
                                seg.point.x += paperEvent.delta.x;
                                seg.point.y += paperEvent.delta.y
                            }

                        } else if(seg.handleOut.selected &&
                            hitType === 'handle-out'){
                            //if option is pressed or handles have been split,
                            //they're no longer parallel and move independently
                            if( paperEvent.modifiers.option ||
                                !seg.handleOut.isColinear(seg.handleIn)) {

                                seg.handleOut.x += paperEvent.delta.x;
                                seg.handleOut.y += paperEvent.delta.y;

                            } else {
                                seg.handleIn.x -= paperEvent.delta.x;
                                seg.handleIn.y -= paperEvent.delta.y;

                                seg.handleOut.x += paperEvent.delta.x;
                                seg.handleOut.y += paperEvent.delta.y;
                            }

                        } else if(seg.handleIn.selected &&
                            hitType === 'handle-in') {

                            //if option is pressed or handles have been split,
                            //they're no longer parallel and move independently
                            if( paperEvent.modifiers.alt ||
                                !seg.handleOut.isColinear(seg.handleIn)) {

                                seg.handleIn.x += paperEvent.delta.y;
                                seg.handleIn.y += paperEvent.delta.y;

                            } else {
                                seg.handleIn.x += paperEvent.delta.x;
                                seg.handleOut.y -= paperEvent.delta.y;
                            }
                        }
                    }

                }
            }
        }

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    var onMouseUp = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        var paperEvent = paperCanvas.convertToMouseEvent(event)

        if(doRectSelection && selectionRect) {
            pg.selection.processRectangularSelection(paperEvent, selectionRect, 'detail');
            selectionRect.remove();

        } else {

            if(selectionDragged) {
                pg.undo.snapshot('moveSelection');
                selectionDragged = false;
            }

            // resetting the items and segments origin points for the next usage
            var selectedItems = pg.selection.getSelectedItems();

            for(var i=0; i < selectedItems.length; i++) {
                var item = selectedItems[i];
                // for the item
                item.origPos = null;
                // and for all segments of the item
                if(item.segments) {
                    for(var j=0; j < item.segments.length; j++) {
                        var seg = item.segments[j];
                            seg.origPoint = null;
                    }
                }
            }
        }

        doRectSelection = false;
        selectionRect = null;

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    return {
        options: null,
        onMouseDoubleClicked: onMouseDoubleClicked,
        onMouseMove: onMouseMove,
        onMouseDown : onMouseDown,
        onMouseUp : onMouseUp,
        onMouseDrag : onMouseDrag
    }
}
