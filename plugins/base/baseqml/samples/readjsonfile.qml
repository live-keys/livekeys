import QtQuick 2.3
import base 1.0
import fs 1.0

Item{

    FileReader{
        id: fileReader
        source: project.dir() + '/../../../../samples/_images/clock-data.json'
    }
    
    JsonDecoder{
        id: jsonDecoder
        data: fileReader.data
    }
    
    PropertyLog{
        input: jsonDecoder.result
    }

}
