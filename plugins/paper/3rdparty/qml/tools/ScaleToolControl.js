
function create(paperCanvas, pg, onStateUpdate){

    var path = null
    var selectedItems
    var fixedGroupPivot
    var pivotMarker = []
    var randomSizes = []
    var transformed = false
    var mouseDown

    var lastMousePos
    var lastMouseDownPos

    var options = {
        scaleCenter: 'selection',
        randomScale: false
    }

    var onMouseDown = function(event){
        transformed = false

        selectedItems = pg.selection.getSelectedItems();

        if(selectedItems.length === 0)
            return;

        if(options.scaleCenter === 'individual') {
            for(var i=0; i < selectedItems.length; i++) {
                pivotMarker.push(pg.guides.crossPivot(selectedItems[i].position));
            }
            if(options.randomScale) {
                randomSizes = [];
                for(var i=0; i < selectedItems.length; i++) {
                    randomSizes.push(Math.random()*0.005);
                }
            }

        } else {
            // only set the fixedPivot once per tool activation/mode switch
            // or the center point moves based on the selection bounds
            if(!fixedGroupPivot) {
                var bounds = null;
                for(var i = 0; i<selectedItems.length; i++) {
                    var item = selectedItems[i];
                    if(i === 0) {
                        bounds = item.bounds;
                    } else {
                        bounds = bounds.unite(item.bounds);
                    }
                }
                fixedGroupPivot = bounds.center;
            }

            if(options.scaleCenter === 'cursor') {
                fixedGroupPivot = new paperCanvas.paper.Point(event.x, event.y);
            }
            // paint pivot guide
            pivotMarker[0] = pg.guides.crossPivot(fixedGroupPivot);
        }


        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        lastMouseDownPos = lastMousePos
    }

    var onMouseDrag = function(event){
        if ( !(event.buttons & Qt.LeftButton) )
            return;

        var eventDelta = lastMousePos
                ? new paperCanvas.paper.Point(event.x - lastMousePos.x, event.y - lastMousePos.y)
                : new paperCanvas.paper.Point(0, 0)

        if(selectedItems.length === 0)
            return;

        var amount = eventDelta.normalize(0.01);
        amount.x += 1;
        amount.y += 1;

        if(!event.modifiers.shift) {
            amount = amount.x * amount.y;
        }

        if(options.scaleCenter === 'individual') {

            for(var i=0; i < selectedItems.length; i++) {
                if(options.randomScale) {
                    selectedItems[i].scale(amount+randomSizes[i], selectedItems[i].position);

                } else {
                    selectedItems[i].scale(amount, selectedItems[i].position);

                }
                if(selectedItems[i].data.isPGTextItem) {
                    selectedItems[i].data.wasScaled = true;
                }
            }

        } else {
            for(var i=0; i < selectedItems.length; i++) {
                selectedItems[i].scale(amount, fixedGroupPivot);
                if(selectedItems[i].data.isPGTextItem) {
                    selectedItems[i].data.wasScaled = true;
                }
            }
        };

        transformed = true

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)

        paperCanvas.paint()
    }

    var onMouseMove = function(event){
        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
    }

    var onMouseUp = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        if(selectedItems.length === 0)
            return;

        for(var i=0; i < pivotMarker.length; i++) {
            pivotMarker[i].remove();
        }

        if(transformed) {
            pg.undo.snapshot('scale');
            transformed = false;
        }

        path = null
    }

    return {
        options: options,
        onMouseDown : onMouseDown,
        onMouseUp : onMouseUp,
        onMouseDrag : onMouseDrag,
        onMouseMove : onMouseMove
    }
}
