
function create(paperCanvas, pg, onStateUpdate){

    var selectedItems
    var fixedGroupPivot
    var pivotMarker = []
    var rotGuideMarker
    var rotGuideLine
    var rand = []
    var initAngles = []
    var transformed = false
    var prevRot = []

    var options = {
        rotationCenter: 'individual',
        randomSpeed: false,
        lookAt: false
    }

    var lastMousePos
    var lastMouseDownPos

    var onMouseDown = function(event){
        transformed = false

        selectedItems = pg.selection.getSelectedItems();

        if(selectedItems.length === 0)
            return;

        if(options.rotationCenter === 'individual') {
            for(var i=0; i<selectedItems.length; i++) {
                var item = selectedItems[i];
                item.applyMatrix = false;
                pivotMarker.push(pg.guides.crossPivot(item.position));
                if(options.randomSpeed) {
                    rand.push((Size.random().width));
                }
                initAngles[i] = item.rotation;
            }

            // paint rotation guide line

            rotGuideLine = pg.guides.line(new paperCanvas.paper.Point(event.x, event.y), new paperCanvas.paper.Point(event.x, event.y));
            rotGuideMarker = pg.guides.rotPivot(new paperCanvas.paper.Point(event.x, event.y), 'grey');

        } else {

            for(var i=0; i<selectedItems.length; i++) {
                var item = selectedItems[i];
                item.applyMatrix = true;
            }

            // only set the fixedPivot once per tool activation/mode switch
            // or the center point moves based on the selection bounds
            if(!fixedGroupPivot) {
                var bounds = null;
                for(var i=0; i < selectedItems.length; i++) {
                    var item = selectedItems[i];
                    if(i === 0) {
                        bounds = item.bounds;
                    } else {
                        bounds = bounds.unite(item.bounds);
                    }
                }
                fixedGroupPivot = bounds.center;
            }

            if(options.rotationCenter === 'cursor') {
                fixedGroupPivot = new paperCanvas.paper.Point(event.x, event.y);
            }

            for(var i=0; i<selectedItems.length; i++) {
                var item = selectedItems[i];
                pg.item.setPivot(item, fixedGroupPivot.clone());
                prevRot[i] = new paperCanvas.paper.Point(event.x - fixedGroupPivot, event.y - fixedGroupPivot).angle;
            }

            // paint pivot guide
            pivotMarker[0] = pg.guides.rotPivot(fixedGroupPivot, 'grey');

            // paint rotation guide line
            rotGuideLine = pg.guides.line(fixedGroupPivot, event.point);
        }


        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        lastMouseDownPos = lastMousePos
    }

    var onMouseMove = function(event){
        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
    }

    var onMouseDrag = function(event){
        if ( !(event.buttons & Qt.LeftButton) )
            return;

        var eventDelta = lastMousePos
                ? new paperCanvas.paper.Point(event.x - lastMousePos.x, event.y - lastMousePos.y)
                : new paperCanvas.paper.Point(0, 0)

        if(selectedItems.length === 0)
            return;

        if(options.rotationCenter === 'individual') {

            var eventPointDelta = new paperCanvas.paper.Point(event.x - lastMouseDownPos.x, event.y - lastMouseDownPos.y)
            var rotAngle = eventPointDelta.angle;

            for(var i=0; i<selectedItems.length; i++) {
                var item = selectedItems[i];
                item.applyMatrix = false;

                if(eventPointDelta.length < 20) {
                    // the initial drag angle is determined by the first 20 units
                    // of drag (used for initial rotation fix)
                    initAngles[i] = eventPointDelta.angle - item.rotation;
                }
                // shift snaps the rotation in 45 degree steps
                if(event.modifiers.shift) {
                    rotAngle = Math.round(rotAngle / 45) *45;
                    item.rotation = rotAngle;

                } else if(options.lookAt) {
                    item.applyMatrix = false;
                    var ang = (new paperCanvas.paper.Point(event.x - item.position.x, event.y - item.position.y)).angle;
                    item.rotation = ang;

                } else {
                    // rotations with random speed use item.rotate instead of
                    // item.rotation for smoother handling and better vis. feedback
                    if(options.randomSpeed) {
                        var currentAngle = eventPointDelta.angle;
                        var lastAngle = (new paperCanvas.paper.Point(lastMousePos.x - lastMouseDownPos.x, lastMousePos.y - lastMouseDownPos.y)).angle;
                        var angleDiff = currentAngle - lastAngle;

                        // nullify the rotation until the user dragged 20 units
                        // prevents irratic behaviour since event.point and
                        // event.downPoint start in the same location
                        if( eventPointDelta.length < 20) {
                            angleDiff = 0;
                        }

                        var randomSpeed = angleDiff < 0 ? -rand[i] : rand[i];
                        item.rotate(angleDiff + randomSpeed, item.position);

                    } else {
                        item.rotation = rotAngle - initAngles[i];
                    }
                }
            }

        } else {
            var rotAngle = (new paperCanvas.paper.Point(event.x - fixedGroupPivot.x, event.y - fixedGroupPivot.y)).angle;

            for(var i=0; i<selectedItems.length; i++) {
                var item = selectedItems[i];
                // shift snaps the rotation in 45 degree steps

                if(event.modifiers.shift) {
                    rotAngle = Math.round(rotAngle / 45) *45;
                    item.applyMatrix = false;
                    item.rotation = rotAngle;

                } else {
                    item.rotate(rotAngle - prevRot[i]);

                }
                prevRot[i] = rotAngle;
            }
        }
        transformed = true;
        rotGuideLine.lastSegment.point = new paperCanvas.paper.Point(event.x, event.y);

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    var onMouseUp = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        if(selectedItems.length === 0)
            return;

        for(var i=0; i < pivotMarker.length; i++) {
            pivotMarker[i].remove();
        }

        if(rotGuideLine) {
            rotGuideLine.remove();
        }
        if(rotGuideMarker) {
            rotGuideMarker.remove();
        }

        // resetting pivots to item centers
        for(var i=0; i < selectedItems.length; i++) {
            var item = selectedItems[i];

            pg.item.setPivot(item, item.bounds.center);
        }

        initAngles = [];
        if(transformed) {
            pg.undo.snapshot('rotate');
            transformed = false;
        }
    }

    return {
        options: options,
        onMouseDown : onMouseDown,
        onMouseUp : onMouseUp,
        onMouseDrag : onMouseDrag,
        onMouseMove : onMouseMove
    }
}
