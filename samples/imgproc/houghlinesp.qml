import 'lcvcore' 1.0
import 'lcvimgproc' 1.0

Row{
    MatSource{
        id : sc
        file : todo
        width : 400
        height : 400
    }
    Canny{
        id : cny
        width : 400
        height : 400
        input : sc.output
        threshold1 : 50
        threshold2 : 200
        apertureSize : 3
    }
    HoughLinesP{
        input : cny.output
        width : 1500
        height : 1100
        rho : 1
        theta : Math.PI / 180
        threshold : 50
        lineThickness : 3
        lineColor : "#00ff00"
    }
}
