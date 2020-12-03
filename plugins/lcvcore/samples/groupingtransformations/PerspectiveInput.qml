import QtQuick 2.3
import lcvcore 1.0

Perspective{
    property double topLeftX: 0
    property double topLeftY: 0
    
    property double topRightX: 512
    property double topRightY: 0
    
    property double bottomRightX: 600
    property double bottomRightY: 300
    
    property double bottomLeftX: 0
    property double bottomLeftY: 384
    
    points: [
        Qt.point(topLeftX, topLeftX), 
        Qt.point(topRightX, topRightY),
        Qt.point(bottomRightX, bottomRightY), 
        Qt.point(bottomLeftX, bottomLeftY)
    ]
    
}