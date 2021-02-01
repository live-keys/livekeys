
function create(paperCanvas, pg, onStateUpdate){

    var rect = null
    var path = null
    var lastMouse = null
    var options = {
        roundedCorners : false,
        cornerRadius: 20
    }

    var onMouseDown = function(event){
        lastMouse = new paperCanvas.paper.Point(event.x, event.y)
    }

    var onMouseDrag = function(event){
        if ( !(event.buttons & Qt.LeftButton) )
            return;

        if ( path ){
            path.remove()
        }

        rect = new paperCanvas.paper.Rectangle(lastMouse, new paperCanvas.paper.Point(event.x, event.y));

        if(event.modifiers & Qt.ShiftModifier) {
            rect.height = rect.width;
        }

        if(options.roundedCorners) {
            path = new paperCanvas.paper.Path.Rectangle(rect, options.cornerRadius);
        } else {
            path = new paperCanvas.paper.Path.Rectangle(rect);
        }

        path = pg.styles.applyActiveStyle(path)

        if( event.modifiers & Qt.AltModifier ) {
            path.position = lastMouse
        }

        onStateUpdate(rect)

        paperCanvas.paint()
    }

    var onMouseUp = function(event){
        if ( event.button !== Qt.LeftButton )
            return;

        pg.undo.snapshot('rectangle');

        path = null
    }

    return {
        options: options,
        onMouseDown : onMouseDown,
        onMouseUp : onMouseUp,
        onMouseDrag : onMouseDrag
    }
}
