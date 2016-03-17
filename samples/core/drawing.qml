import lcvcore 1.0


// This example shows how to use the drawing functions 
// made available by the MatDraw element.

Rectangle{
    MatEmpty{
        id : matE
        width : 400
        height : 400
        channels : 4
        matSize : "400x400"
        color : "#aa3311"
    }
    
    MatDraw{
        id : matD
        input : matE.output
        anchors.left : matE.right
        anchors.leftMargin : 10
        onInputChanged : {
            line( Qt.point(10, 10) , Qt.point(100, 200), "#fff", 1, 8, 0 )
            fillPoly( [
                [Qt.point(10, 10), Qt.point(20, 20), Qt.point(300, 20)],
                [Qt.point(20, 20), Qt.point(100, 100), Qt.point(10, 300)]
            ], "#fff" )
            circle( Qt.point(150, 150), 50, "#3333cc", -1)
            rectangle( Qt.point(110, 220), Qt.point(200, 300), "#33aa33", -1)
        }
        width : 400
        height : 400
    }
}
