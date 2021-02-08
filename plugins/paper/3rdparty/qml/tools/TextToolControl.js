
function create(paperCanvas, pg, onStateUpdate){

    var hitItem = null;

    var currentText = ''
    var textItem;
    var toolMode = 'create';
    var creationPoint = paperCanvas.paper.view.center;

    var textSize = -1;
    var textAngle = 0;

    var hitOptions = {
        fill: true,
        curves: true,
        tolerance: 5 / paperCanvas.paper.view.zoom
    };

    var options = {
        fontFamily: '',
        fontStyle: '',
        fontSize: 80,
        letterSpacing: 0
    };

    var onMouseMove = function(event){
        var hitResult = paperCanvas.paper.project.hitTest(new paperCanvas.paper.Point(event.x, event.y), hitOptions);
        if(hitResult && hitResult.item) {
            var root = pg.item.getRootItem(hitResult.item);
            if(root.data.isPGTextItem) {
                hitItem = root;
            } else {
                hitItem = null;
            }
        } else {
            hitItem = null;
        }
    }

    var onMouseDown = function(event){
        if(toolMode === 'edit') {
            finalizeInput();
        }

        if(toolMode === 'create') {
            toolMode = 'edit';
            if(hitItem) {
                pg.selection.clearSelection();
                pg.selection.setItemSelection(hitItem, true);
                handleSelectedItem(hitItem);
            } else {
                pg.selection.clearSelection();
                if ( onStateUpdate )
                    onStateUpdate(null)
                createItem('', new paperCanvas.paper.Point(event.x, event.y));
            }
            pg.undo.snapshot('texteditstarted');
        }
    };

    var createItem = function(text, pos) {
        var wasScaled = false;
        if(textItem) {
            wasScaled = textItem.data.wasScaled;
            textItem.remove();
        }
        toolMode = 'edit';
        creationPoint = pos;
        textItem = pg.text.createPGTextItem(text, options, pos)
        textItem.data.wasScaled = wasScaled

        pg.styles.applyActiveStyle(textItem)

        // apply original rotation and scale to the new item
        if(textItem) {
            var helperCurve = getFirstCurve(textItem);
            if(helperCurve) {
                if( textSize > -1 && textItem.data.wasScaled) {
                    var glyphHeight = helperCurve.length;
                    textItem.scaling = textSize / glyphHeight;
                }

                var angle = helperCurve.line.vector.angle;
                if(textAngle !== 0) {
                    textItem.rotation = textAngle - angle;
                }
            }
        }
        paperCanvas.paint()
    };


    var handleSelectedItem = function(selectedItem) {
        creationPoint = selectedItem.position;
        textItem = selectedItem;

        if ( onStateUpdate ){
            onStateUpdate({
                text: selectedItem.data.text,
                fontFamily: selectedItem.data.fontFamily,
                fontStyle: selectedItem.data.fontStyle,
                fontSize: selectedItem.data.fontSize,
                letterSpacing: selectedItem.data.letterSpacing
            })
        }

        if(textItem) {
            var helperCurve = getFirstCurve(textItem);
            if(helperCurve){
                textSize = helperCurve.length;
                textAngle = helperCurve.line.vector.angle;
            }
        }
    };

    var getFirstCurve = function(item) {
        for(var i=0; i<item.children.length; i++) {
            for(var j=0; j<item.children[i].children.length; j++) {
                var child = item.children[i].children[j];
                if(child.data.isPGGlyphRect) {
                    return child.curves[0];
                }
            }
        }
    };

    var finalizeInput = function(){
        if (currentText === '') {
            if ( textItem )
                textItem.remove();
            textItem = null
        } else {
            creationPoint = paperCanvas.paper.view.center;
            textItem = null;
            pg.undo.snapshot('textcreated');
        }
        toolMode = 'create';
    }

    var updateText = function(text){
        currentText = text
        createItem(text, creationPoint)
    }

    return {
        options: options,
        updateText: updateText,
        onMouseDown : onMouseDown,
        onMouseMove : onMouseMove,
        deactivate: finalizeInput
    }
}
