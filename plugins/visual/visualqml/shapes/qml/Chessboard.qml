import QtQuick 2.3

Rectangle{
    color: 'white'
    width: 120
    height: 120

    property double cellSize: width / 8
    property color alternateColor: 'grey'

    Canvas{
        anchors.fill : parent
        property color fillColor: parent.alternateColor
        property double cellSize: parent.cellSize
        onPaint: {
            var ctx = getContext("2d")
            var nrows = height / cellSize;
            var ncols = width / cellSize;
            ctx.fillStyle = fillColor;
            for(var i = 0; i < nrows; ++i){
                for (var j = 0; j < ncols; ++j){
                    if ( j % 2 === 0 && i % 2 === 0 )
                        ctx.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                    else if ( j % 2 === 1 && i % 2 === 1)
                        ctx.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                }

            }
        }
    }
}
