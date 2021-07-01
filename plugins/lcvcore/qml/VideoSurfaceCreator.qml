import QtQuick 2.0
import timeline 1.0
import lcvcore 1.0
import live 1.0
import visual.input 1.0 as Input

Rectangle{
    id: root
    width: 300
    height: 160
    color: '#141b20'
    border.width: 1
    border.color: '#232b30'

    property QtObject currentTheme: lk.layers.workspace ? lk.layers.workspace.themes.current : null

    signal surfaceCreated(VideoSurface videoSurface)
    signal cancelled()

    property Component surfaceFactory : VideoSurface{}

    function createSurface(){

        var w = parseInt(widthInput.text)
        var h = parseInt(heightInput.text)
        if ( w > 0 && h > 0 ){
            var surface = surfaceFactory.createObject()
            surface.imageWidth = w
            surface.imageHeight = h
            return surface
        }
        return null
    }

    Text{
        anchors.top: parent.top
        anchors.topMargin: 14
        anchors.left: parent.left
        anchors.leftMargin: 14
        text: 'Create Video Surface'
        font.family: 'Open Sans, Arial, sans-serif'
        font.pixelSize: 12
        font.weight: Font.Normal
        color: "#afafaf"
    }

    Input.DropDown{
        id: presetInput
        anchors.left: parent.left
        anchors.leftMargin: 75
        anchors.top: parent.top
        anchors.topMargin: 47
        width: 150
        style: currentTheme.dropDownStyle

        property var values: [
            { w: 800, h: 600, label: '800x600 4:3 (Default)'},
            { w: 1280, h: 720, label: '1280x720 16:9 (HD)'},
            { w: 1920, h: 1080, label: '1920x1080 16:9 (Full HD)'},
            { w: 3840, h: 2160, label: '3840x600 16:9 (Ultra HD)'}
        ]

        model: values.map(function(val){ return val.label })

        onCurrentIndexChanged: {
            var val = values[currentIndex]
            widthInput.text = val.w
            heightInput.text = val.h
        }
    }

    Input.InputBox{
        id: widthInput
        anchors.left: parent.left
        anchors.leftMargin: 75
        anchors.top: parent.top
        anchors.topMargin: 97
        width: 50
        text: '800'
    }
    Input.InputBox{
        id: heightInput
        anchors.left: parent.left
        anchors.leftMargin: 75 + 60
        anchors.top: parent.top
        anchors.topMargin: 97
        width: 50
        text: '600'
    }

    Input.Button{
        id: applyButton
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.left: parent.left
        anchors.leftMargin: 75 + 60 + 60
        width: 25
        height: 25
        content: root.currentTheme ? root.currentTheme.buttons.apply : null
        onClicked: { root.surfaceCreated(root.createSurface()) }
    }

    Input.TextButton{
        anchors.right: parent.right
        anchors.rightMargin: 10
        radius: 5
        width: 30
        height: 30
        text: 'X'
        style:  Input.TextButtonStyle{
            backgroundColor: '#3f444d'
            backgroundHoverColor: Qt.lighter('#3f444d', 1.2)
            borderColor: '#575b63'
        }

        onClicked: {
            parent.cancelled()
        }
    }

}
