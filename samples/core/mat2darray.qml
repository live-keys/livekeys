import QtQuick 2.3
import lcvcore 1.0

Grid{
    
    // Creating a custom value based matrix
    
    Mat2DArray{
        id : kernel
        type : Mat.CV8U
        width : 200
        height : 200
        linearFilter : false
        values : [
            [100, 150, 255], 
            [70, 50, 10], 
            [30, 20, 0]]
    }
}
