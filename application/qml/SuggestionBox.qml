import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import Cv 1.0

Rectangle{
    id: root

    width: 200
    height: 200
    color: "#111"
    opacity: 0.95

    property alias model : pluginList.model
    property string fontFamily: 'Courier New, Courier'
    property int fontSize: 12

    ScrollView{
        anchors.top : parent.top

        height : 200
        width: 200

        ListView{
            id : pluginList
            anchors.fill: parent
            anchors.rightMargin: 2
            anchors.bottomMargin: 5
            anchors.topMargin: 0
            boundsBehavior : Flickable.StopAtBounds
            visible: true
            delegate: Component{

                Rectangle{
                    property string completion: model.completion

                    width : parent.width
                    height : 30
                    color : ListView.isCurrentItem ? "#444" : "#3a3a3a"
                    Text{
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 86
                        anchors.verticalCenter: parent.verticalCenter

                        font.family: root.fontFamily
                        font.pixelSize: root.fontSize

                        color: "#fafafa"
                        text: model.label
                    }

                    MouseArea{
                        anchors.fill: parent
                    }
                }
            }

        }
    }
}
