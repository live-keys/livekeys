import QtQuick 2.3
import base 1.0
import editor 1.0
import fs 1.0

Item{

    FileReader{
        id: fileReader
        file: project.path('readingfiles.txt')
    }

    TextSearch{
        id: textSearch
        text: fileReader.data
        value: 'visual elements'
    }

    IndexSelector{
        id: indexSelector
        list: textSearch.output
        index: 0
    }
    
    ConvertToInt{
        id: convertToInt
        nanValue: -1
        input: indexSelector.current
    }
    
    TextLineAtPosition{
        id: textLineAtPosition
        position: convertToInt.result
        text: fileReader.data
    }
    
    TextClip{
        id: textClip
        text: fileReader.data
        positions: textLineAtPosition.result
    }

    Text{
        color: 'white'
        text: textClip.result
    }

}
