import QtQuick 2.3
import live 1.0

Item{
    
    // Using FileReader and JsonFileReader to read data from files
    
    FileReader{
        id: fileReader
        source: project.path('../../../samples/_images/clock-data.json')
    }    
    
    Text{
        id: fileReaderData
        color: 'white'
        text: fileReader.data 
    }
        
    JsonFileReader{
        id: jsonFileReader
        source: fileReader.source
    }
    
    Text{
        id: jsonFileReaderData
        color: 'grey'
        anchors.top: fileReaderData.bottom
        text: 'File Train Image: ' + jsonFileReader.root['train']
    }

} 
