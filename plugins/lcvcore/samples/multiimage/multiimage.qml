import QtQuick 2.3
import base 1.0
import editqml 1.0
    
ListView{
    anchors.fill: parent
    
    property var files: [
        project.dir() + '/../../../../samples/_images/object_101_piano_query.jpg',
        project.dir() + '/../../../../samples/_images/coast_6.jpg',
        project.dir() + '/../../../../samples/_images/object_101_piano_query.jpg'
    ]
    
    model: files
    delegate: Builder{ 
        id: builder
        source: project.path("Operations.qml")
    }
}
