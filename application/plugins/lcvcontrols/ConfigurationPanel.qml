import QtQml.Models 2.1
import lcvcontrols 1.0

Column{


    ConfigurationField{
        width: 300
        label: "lalala"
        editor: InputBox{}
    }

Repeater{
    id: repeater
    
    property Rectangle var1 : Rectangle{}
    
    signal dataChanged()
    property variant mapData : {return {
        'key' : mydata,
        'value' : value,
        'editor' : mydata//TextInputBox{}
    }}
    
    
    property list<Component> mydata : [
        Component{
            Rectangle{
                id: dataEditor
                
                property string key: 'lk'
                
                signal dataChanged(variant data)
                
                width: 50; height: 50; color: "#ff0000"
                
                MouseArea{
                    anchors.fill : parent
                    onClicked: dataEditor.dataChanged("hello")
                } 
            }}, 
        Component{
            Rectangle{
                id: dataEditor2
                
                property string key: 'asd'
                
                signal dataChanged(variant data)
                
                
                width: 50; height: 50; color: "#ff0000"
                
                MouseArea{
                    anchors.fill : parent
                    onClicked: dataEditor2.dataChanged("hello2")
                }
            }
        }
    ]
    
    model: mydata
    
    delegate: Component{
        Rectangle{
            width: 100
            height: 100
            color: "#666"
            
            Loader{
                id: editorLoader
                sourceComponent: modelData
            }
            
            Text{
                color: "#fff"
                text: editorLoader.item.key
            }
            
            Connections{
                target: editorLoader.item
                onDataChanged: console.log(data)
            }
        }
    }
}

}
