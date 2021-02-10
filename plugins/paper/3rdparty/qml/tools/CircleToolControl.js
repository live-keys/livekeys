
function create(paperCanvas, pg, onStateUpdate){

    var shape = null
    var lastMouse = null

    var onMouseDown = function(event){
        shape = new paperCanvas.paper.Shape.Ellipse({
            point: [event.x, event.y],
            size: 20
        });

        lastMouse = [event.x, event.y]

        if ( pg )
            shape = pg.styles.applyActiveStyle(shape)

        paperCanvas.paint()
    }

    var onMouseDrag = function(event){
        if ( !(event.buttons & Qt.LeftButton) )
            return;

        var ex = event.x;
        var ey = event.y;

        if( event.modifiers & Qt.ShiftModifier ){
            shape.size = [lastMouse[0] - ex, lastMouse[0] - ex];
        } else {
            shape.size = [lastMouse[0] - ex, lastMouse[1] - ey];
        }

        if( event.modifiers & Qt.AltModifier ){
            shape.position = [lastMouse[0], lastMouse[1]];
        } else {
            shape.position = [lastMouse[0] - shape.size.width / 2, lastMouse[1] - shape.size.height / 2]
        }
        paperCanvas.paint()
    }

    var onMouseUp = function(event){
        if ( event.button !== Qt.LeftButton )
            return

        shape.toPath(true)
        shape.remove()

        pg.undo.snapshot('circle')
    }

    return {
        options: null,
        onMouseDown : onMouseDown,
        onMouseUp : onMouseUp,
        onMouseDrag : onMouseDrag
    }
}
