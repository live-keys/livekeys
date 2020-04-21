import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import fs 1.0

Item{
    
    FileLineReader{
        id: fileLineReader
        path : project.path('../../../../samples/_images/clock-data.json')
    }
    
    StreamCapture{
        stream: fileLineReader.stream
        onNewData: console.log(data)
    }
    
    
}