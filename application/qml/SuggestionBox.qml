import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import Cv 1.0

Rectangle{
    id: root

    width: 250
    height: 280
    color: "#111"
    opacity: 0.95

    property alias suggestionCount : pluginList.count
    property alias model : pluginList.model
    property string fontFamily: 'Courier New, Courier'
    property int fontSize: 11
    property int smallFontSize: 9

    function getCompletion(){
        if ( pluginList.currentItem ){
            return pluginList.currentItem.completion
        }
        return ""
    }

    function incrementSelection(){
        if ( pluginList.currentIndex <  pluginList.count - 1 ){
            pluginList.currentIndex++;
        } else {
            pluginList.currentIndex = 0;
        }
    }
    function decrementSelection(){
        if ( pluginList.currentIndex > 0 ){
            pluginList.currentIndex--;
        } else {
            pluginList.currentIndex = pluginList.count - 1;
        }
    }

    ScrollView{
        anchors.top : parent.top

        height : root.height
        width: root.width

        ListView{
            id : pluginList
            anchors.fill: parent
            anchors.rightMargin: 2
            anchors.bottomMargin: 5
            anchors.topMargin: 0
            boundsBehavior : Flickable.StopAtBounds
            visible: true
            opacity: root.opacity
            currentIndex: 0
            onCountChanged: currentIndex = 0

            delegate: Component{

                Rectangle{
                    property string completion: model.completion

                    width : parent.width
                    height : 25
                    color : ListView.isCurrentItem ? "#444" : "transparent"
                    Text{
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        font.family: root.fontFamily
                        font.pixelSize: root.fontSize

                        color: "#fafafa"
                        text: model.label
                    }
                    Text{
                        id: category
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        font.family: root.fontFamily
                        font.pixelSize: root.smallFontSize

                        color: "#666688"
                        text: model.info + "|" + model.category
                    }

                    MouseArea{
                        anchors.fill: parent
                    }
                }
            }

        }
    }
}
