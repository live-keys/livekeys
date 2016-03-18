import QtQuick 2.3

Column{
    id: root
    
    width: 100
    height : 100
    
    property var configurationData : ({
        'threshold' : 10,
        'nonmaxSuppresion': true
    })
    
    property list<Component> configurationFields : [
        Component{ 
            ConfigurationField{
                label : "Threshold"
                editor : InputBox{text: root.configurationData["threshold"]; onTextChanged: root.configurationData["threshold"] = text;}
            }
        },
        Component{
            ConfigurationField{
                label : "Non Max Suppression"
                editor : InputBox{text: root.configurationData["nonmaxSuppresion"]; onTextChanged: root.configurationData["nonMaxSuppresion"] = text;}
            }
        }   
    ]
    
    Repeater{
        model: root.configurationFields ? root.configurationFields : []
        delegate : Component{
            Rectangle{
                width: 300
                height: 30
                
                Loader{
                    id: fieldLoader
                    sourceComponent: modelData
                }
                
            }
        }
    }
    
    Rectangle{
        id: propertymonitor
        width: 100
        height : 100
        color: "#333"
        
        MouseArea{
            width: 100
            height : 100
            onClicked: {console.log(configurationData['threshold'])}
        }
    }
}
