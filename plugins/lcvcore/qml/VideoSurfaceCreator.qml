import QtQuick 2.0
import timeline 1.0
import lcvcore 1.0
import live 1.0
import workspace 1.0 as Workspace

Rectangle{
    id: root
    width: 300
    height: 80
    color: '#03070a'

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

    Workspace.InputBox{
        id: widthInput
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 50
        width: 50
        text: '800'
    }
    Workspace.InputBox{
        id: heightInput
        anchors.left: parent.left
        anchors.leftMargin: 80
        anchors.top: parent.top
        anchors.topMargin: 50
        width: 50
        text: '600'
    }

    Workspace.Button{
        id: applyButton
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.left: parent.left
        anchors.leftMargin: 140
        width: 25
        height: 25
        content: root.currentTheme ? root.currentTheme.buttons.apply : null
        onClicked: { root.surfaceCreated(root.createSurface()) }
    }

    TextButton{
        anchors.right: parent.right
        anchors.rightMargin: 10
        radius: 5
        width: 30
        height: 30
        text: 'X'
        onClicked: {
            parent.cancelled()
        }
    }

}
