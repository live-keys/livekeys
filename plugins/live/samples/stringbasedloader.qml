import QtQuick 2.3
import live 1.0

Item{
    anchors.fill: parent
    
    StringBasedLoader{
        id: stringBasedLoader
        source: "import QtQuick 2.3\nRectangle{width: 200; height: 300; color: '#330';}"
    }
    
}