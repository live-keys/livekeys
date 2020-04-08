import QtQuick 2.0
import lcvcore 1.0

Item{
    width: 300
    height: 100

    property Component segmentFactory : VideoSegment{

    }

    PathInputBox{
        onPathSelected: {
            console.log(path)
        }
    }

}
