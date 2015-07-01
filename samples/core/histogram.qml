import "lcvcore" 1.0

Rectangle{
    
    width: 400
    height : 400
    color : "#222"
    
    DrawHistogram{
        width: 400
        height : 400
        maxValue : 200
        colors : ['green', 'blue']
        values : [
            [10, 44, 30, 50, 200, 10, 300, 50, 70], 
            [20, 30, 40, 50, 100, 20, 200, 50, 30]
        ]
    }
    
}