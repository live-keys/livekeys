import "lcvcore" 1.0

Rectangle{
    
    width: 200
    height : 200
    color : "#222333"
    
    DrawHistogram{
        width: 200
        height : 200
        maxValue : 200
        colors : ['green', 'darkblue']
        render : DrawHistogram.Rectangles
        values : [
            [10, 44, 30, 50, 200, 10, 300, 50, 70], 
            [20, 30, 40, 50, 100, 30, 200, 50, 30]
        ]
    }
    
}