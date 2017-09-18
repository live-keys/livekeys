import QtQuick 2.3
import live 1.0

Item{
    
    // Using FileReader and JsonFileReader to read data from files
    
    StaticFileReader{
        id: fileReader
        onInit: staticLoad(project.path('../_images/clock-data.json'))
    }
    
    Text{
        id: fileReaderData
        color: 'white'
        text: fileReader.data 
    }
        
    StaticJsonFileReader{
        id: jsonFileReader
        onInit: staticLoad(project.path('../_images/clock-data.json'))
    }
    
    Text{
        id: jsonFileReaderData
        color: 'grey'
        anchors.top: fileReaderData.bottom
        text: 'File Train Image: ' + jsonFileReader.root['train']
    }

} 