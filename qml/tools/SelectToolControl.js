
function create(paperCanvas, pg, onStateUpdate){

    var rect = null
    var path = null

    var hitOptions = {
        segments: true,
        stroke: true,
        curves: true,
        fill: true,
        guide: false,
        tolerance: 8 / paperCanvas.paper.view.zoom
    }

    var mode = 'none'
    var selectionRect

    var itemGroup
    var pivot
    var corner
    var origPivot
    var origSize
    var origCenter
    var scaleItems

    var rotItems = []
    var rotGroupPivot
    var prevRot = []

    var boundsPath;
    var boundsScaleHandles = []
    var boundsRotHandles = []

    var lastMousePos
    var lastMouseDownPos


    var removeBoundsPath = function() {
        pg.guides.removeHelperItems()
        boundsPath = null
        boundsScaleHandles.length = 0
        boundsRotHandles.length = 0
    }

    var setSelectionBounds = function() {
        removeBoundsPath();

        var items = pg.selection.getSelectedItems();
        if(items.length <= 0) return;

        var rect;

        for ( var index = 0; index < items.length; ++index ){
            var item = items[index]

            if(rect) {
                rect = rect.unite(item.bounds);
            } else {
                rect = item.bounds;
            }
        }

        if(!boundsPath) {
            boundsPath = new paperCanvas.paper.Path.Rectangle(rect);
            boundsPath.curves[0].divideAtTime(0.5);
            boundsPath.curves[2].divideAtTime(0.5);
            boundsPath.curves[4].divideAtTime(0.5);
            boundsPath.curves[6].divideAtTime(0.5);
        }
        boundsPath.guide = true;
        boundsPath.data.isSelectionBound = true;
        boundsPath.data.isHelperItem = true;
        boundsPath.fillColor = null;
        boundsPath.strokeScaling = false;
        boundsPath.fullySelected = true;
        boundsPath.parent = pg.layer.getGuideLayer();

        for ( var index = 0; index < boundsPath.segments.length; ++index ){
            var segment = boundsPath.segments[index]

            var size = 4;

            if(index%2 === 0) {
                size = 6;
            }

            if(index === 7) {
                var offset = new paperCanvas.paper.Point(0, 10 / paperCanvas.paper.view.zoom);
                boundsRotHandles[index] =
                    new paperCanvas.paper.Path.Circle({
                        center: new paperCanvas.paper.Point(segment.point.x + offset.x, segment.point.y + offset.y),
                        data: {
                            offset: offset,
                            isRotHandle: true,
                            isHelperItem: true,
                            noSelect: true,
                            noHover: true
                        },
                        radius: 5 / paperCanvas.paper.view.zoom,
                        strokeColor: pg.guides.getGuideColor('blue'),
                        fillColor: 'white',
                        strokeWidth: 0.5 / paperCanvas.paper.view.zoom,
                        parent: pg.layer.getGuideLayer()
                    });
            }

            boundsScaleHandles[index] =
                new paperCanvas.paper.Path.Rectangle({
                    center: segment.point,
                    data: {
                        index:index,
                        isScaleHandle: true,
                        isHelperItem: true,
                        noSelect: true,
                        noHover: true
                    },
                    size: [size/paperCanvas.paper.view.zoom,size/paperCanvas.paper.view.zoom],
                    fillColor: pg.guides.getGuideColor('blue'),
                    parent: pg.layer.getGuideLayer()
                });
        }
    }

    var getRectCornerNameByIndex = function(index) {
        switch(index) {
            case 0:
                return 'bottomLeft'

            case 1:
                return 'leftCenter'

            case 2:
                return 'topLeft'

            case 3:
                return 'topCenter'

            case 4:
                return 'topRight'

            case 5:
                return 'rightCenter'

            case 6:
                return 'bottomRight'

            case 7:
                return 'bottomCenter'
        }
    }

    var getOpposingRectCornerNameByIndex = function(index) {
        switch(index) {
            case 0:
                return 'topRight';

            case 1:
             return 'rightCenter';

            case 2:
                return 'bottomRight';

            case 3:
                return 'bottomCenter';

            case 4:
                return 'bottomLeft';

            case 5:
                return 'leftCenter';

            case 6:
                return 'topLeft';

            case 7:
                return 'topCenter';
        }
    };

    var onMouseDown = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        pg.hover.clearHoveredItem();

        var hitResult = paperCanvas.paper.project.hitTest({x: event.x, y: event.y}, hitOptions);
        if (hitResult) {
            if(hitResult.item.data && hitResult.item.data.isScaleHandle) {
                mode = 'scale';
                var index = hitResult.item.data.index;
                pivot = boundsPath.bounds[getOpposingRectCornerNameByIndex(index)].clone();
                origPivot = boundsPath.bounds[getOpposingRectCornerNameByIndex(index)].clone();
                corner = boundsPath.bounds[getRectCornerNameByIndex(index)].clone();
                origSize = corner.subtract(pivot);
                origCenter = boundsPath.bounds.center;
                scaleItems = pg.selection.getSelectedItems();

            } else if( hitResult.item.data && hitResult.item.data.isRotHandle ){
                mode = 'rotate';
                rotGroupPivot = boundsPath.bounds.center;
                rotItems = pg.selection.getSelectedItems();

                for ( var i = 0; i < rotItems.length; ++i ){
                    var pdelta = new paperCanvas.paper.Point(event.x - rotGroupPivot.x, event.y - rotGroupPivot.y)
                    prevRot[i] = pdelta.angle;
                }

            } else {
                // deselect all by default if the shift key isn't pressed
                // also needs some special love for compound paths and groups,
                // as their children are not marked as "selected"
                if(! (event.modifiers & Qt.ShiftModifier )) {
                    var root = pg.item.getRootItem(hitResult.item);
                    if(pg.item.isCompoundPathItem(root) || pg.group.isGroup(root)) {
                        if(!root.selected) {
                            pg.selection.clearSelection();
                        }
                    } else if(!hitResult.item.selected) {
                        pg.selection.clearSelection();
                    }
                }
                // deselect a currently selected item if shift is pressed
                if((event.modifiers & Qt.ShiftModifier) && hitResult.item.selected) {
                    pg.selection.setItemSelection(hitResult.item, false);

                } else {
                    pg.selection.setItemSelection(hitResult.item, true);

                    if ( event.modifiers & Qt.AltModifier ){
                        mode = 'cloneMove';
                        pg.selection.cloneSelection();
                    } else {
                        mode = 'move';
                    }
                }
            }
            // while transforming object, never show the bounds stuff
            removeBoundsPath();

        } else {
            if (!event.modifiers.shift) {
                removeBoundsPath();
                pg.selection.clearSelection();
            }
            mode = 'rectSelection';
        }

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        lastMouseDownPos = lastMousePos
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

        var modOrigSize = origSize;

        if ( selectionRect ){
            selectionRect.remove()
            selectionRect = null
        }

        if(mode === 'rectSelection') {
            selectionRect = pg.guides.rectSelect(paperEvent);
            // Remove this rect on the next drag and up event

        } else if(mode === 'scale') {
            itemGroup = new paperCanvas.paper.Group(scaleItems);
            itemGroup.addChild(boundsPath);
            itemGroup.data.isHelperItem = true;
//            itemGroup.strokeScaling = false;
            itemGroup.applyMatrix = false;

            if (paperEvent.modifiers.alt) {
                pivot = origCenter;
                modOrigSize = origSize*0.5;
            } else {
                pivot = origPivot;
            }

            corner = corner.add(paperEvent.delta);
            var size = corner.subtract(pivot);
            var sx = 1.0
            var sy = 1.0
            if (Math.abs(modOrigSize.x) > 0.0000001) {
                sx = size.x / modOrigSize.x;
            }
            if (Math.abs(modOrigSize.y) > 0.0000001) {
                sy = size.y / modOrigSize.y;
            }

            if (paperEvent.modifiers.shift) {
                var signx = sx > 0 ? 1 : -1;
                var signy = sy > 0 ? 1 : -1;
                sx = sy = Math.max(Math.abs(sx), Math.abs(sy));
                sx *= signx;
                sy *= signy;
            }

            itemGroup.scale(sx, sy, pivot);

            for ( var index = 0; index < boundsScaleHandles.length; ++index ){
                var handle = boundsScaleHandles[index]
                handle.position = itemGroup.bounds[getRectCornerNameByIndex(index)];
                handle.bringToFront();
            }

            for ( var index = 0; index < boundsRotHandles.length; ++index ){
                var handle = boundsRotHandles[index]
                if(handle) {
                    handle.position = itemGroup.bounds[getRectCornerNameByIndex(index)]+handle.data.offset;
                    handle.bringToFront();
                }
            }

        } else if(mode === 'rotate') {
            var rotAngle = (new paperCanvas.paper.Point(event.x - rotGroupPivot.x, event.y - rotGroupPivot.y)).angle;

            for ( var i = 0; i < rotItems.length; ++i ){
                var item = rotItems[i]

                if(!item.data.origRot) {
                    item.data.origRot = item.rotation;
                }

                if(paperEvent.modifiers.shift) {
                    rotAngle = Math.round(rotAngle / 45) *45;
                    item.applyMatrix = false;
                    item.pivot = rotGroupPivot;
                    item.rotation = rotAngle;

                } else {
                    item.rotate(rotAngle - prevRot[i], rotGroupPivot);
                }
                prevRot[i] = rotAngle;
            }

        } else if(mode === 'move' || mode === 'cloneMove') {

            var dragVector = new paperCanvas.paper.Point(event.x - lastMouseDownPos.x, event.y - lastMouseDownPos.y);
            var selectedItems = pg.selection.getSelectedItems();

            for(var i=0; i<selectedItems.length; i++) {
                var item = selectedItems[i];
                // add the position of the item before the drag started
                // for later use in the snap calculation
                if(!item.data.origPos) {
                    item.data.origPos = item.position;
                }

                if (paperEvent.modifiers.shift) {
                    item.position = item.data.origPos +
                    pg.math.snapDeltaToAngle(dragVector, Math.PI*2/8);

                } else {
                    item.position.x += paperEvent.delta.x
                    item.position.y += paperEvent.delta.y
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

        if(mode === 'rectSelection' && selectionRect) {

            pg.selection.processRectangularSelection(paperEvent, selectionRect);
            selectionRect.remove();

        } else if (mode === 'move' || mode === 'cloneMove'){

            // resetting the items origin point for the next usage
            var selectedItems = pg.selection.getSelectedItems();

            for ( var index = 0; index < selectedItems.length; ++index ){
                var item = selectedItems[index]
                item.data.origPos = null
            }
            pg.undo.snapshot('moveSelection');

        } else if(mode === 'scale') {
            itemGroup.applyMatrix = true;

//             mark text items as scaled (for later use on font size calc)
            for(var i=0; i<itemGroup.children.length; i++) {
                var child = itemGroup.children[i];
                if(child.data.isPGTextItem) {
                    child.data.wasScaled = true;
                }
            }

            itemGroup.layer.addChildren(itemGroup.children);
            itemGroup.remove();
            pg.undo.snapshot('scaleSelection');

        } else if(mode === 'rotate') {
            for ( var i = 0; i < rotItems.length; ++i ){
                var item = rotItems[i]
                item.applyMatrix = true;
            }
            pg.undo.snapshot('rotateSelection');
        }

        mode = 'none'
        selectionRect = null

        if(pg.selection.getSelectedItems().length <= 0) {
            removeBoundsPath();
        } else {
            setSelectionBounds();
        }

        lastMousePos = new paperCanvas.paper.Point(event.x, event.y)
        paperCanvas.paint()
    }

    var deactivate = function(){
        pg.hover.clearHoveredItem()
        removeBoundsPath()
        paperCanvas.paint()
    }

    return {
        options: null,
        onMouseDown : onMouseDown,
        onMouseUp : onMouseUp,
        onMouseDrag : onMouseDrag,
        onMouseMove : onMouseMove,
        deactivate: deactivate
    }
}
