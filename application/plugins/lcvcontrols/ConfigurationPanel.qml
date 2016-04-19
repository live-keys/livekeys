import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle{
    id : root
    width: 500
    height : 200

    property var configurationData : ({
        'sample1' : 10
    })

    property list<Component> configurationFields : [
        Component{
            ConfigurationField{
                label : "Sample1"
                editor : InputBox{
                    text: root.configurationData["sample1"];
                    onTextChanged: root.configurationData["sample1"] = text;
                }
            }
        }
    ]


    ScrollView{
        style : LiveCVScrollStyle{}
        anchors.fill: parent


        Column{
            width: parent.width

            Repeater{
                id : configurationView
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
        }

    }

}
