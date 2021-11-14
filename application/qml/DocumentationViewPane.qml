import QtQuick 2.3
import editor 1.0
import base 1.0
import live 1.0
import fs 1.0 as Fs
import workspace 1.0

Pane{
    id: root
    objectName : 'documentation'
    paneType: 'documentation'
    paneState : { return {} }
    paneCleared : function(){
        root.page.destroy()
    }

    paneInitialize : function(s){
        if ( s.document ){
            var path = ''

            if (typeof s.document === 'string' || s.document instanceof String){
                path = s.document

                if (Fs.Path.isRelative(path) )
                    path = lk.layers.workspace.project.path(path)
            } else { // if url
                path = s.document.toString()
            }

            if ( path ){
                if ( !root.page ){
                    root.page = root.viewFactory.createObject(viewWrapper)
                }

                if ( s.styleSheet){
                    root.page.styleSheet = root.page.__defaultStyleSheet + ' ' + s.styleSheet
                } else {
                    root.page.styleSheet = root.page.__defaultStyleSheet
                }

                root.page.loadDocumentationHtml(path)

                if ( s.title ){
                    root.pageTitle = s.title
                } else {
                    root.pageTitle = Fs.Path.baseName(path)
                }

            }
        }
    }

    property var panes: null
    property Theme currentTheme : lk.layers.workspace ? lk.layers.workspace.themes.current : null
    property color topColor: currentTheme ? currentTheme.paneTopBackground : 'black'
    property color backgroundColor: currentTheme ? currentTheme.paneBackground : 'black'

    property var page : null
    onPageChanged: {
        if ( page && page.parent !== viewWrapper ){
            page.parent = viewWrapper
        }
    }

    property string pageTitle : ''

    property var viewFactory : Component{
        DocumentationView{}
    }

    Rectangle{
        id: paneHeader
        anchors.left: parent.left
        anchors.top: parent.top

        width: parent.width
        height: 30
        color: root.topColor

        PaneDragItem{
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            onDragStarted: root.panes.__dragStarted(root)
            onDragFinished: root.panes.__dragFinished(root)
            display: titleText.text
        }

        Text{
            id: titleText
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#808691"
            text: root.pageTitle
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            font.weight: Font.Normal
        }

        Item{
            anchors.right: parent.right
            width: 30
            height: parent.height

            Image{
                id : paneOptions
                anchors.centerIn: parent
                source : "qrc:/images/pane-menu.png"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    paneMenu.visible = !paneMenu.visible
                }
            }
        }

    }

    Rectangle{
        id: paneMenu
        visible: false
        anchors.right: root.right
        anchors.top: root.top
        anchors.topMargin: 30
        property int buttonHeight: 30
        property int buttonWidth: 180
        opacity: visible ? 1.0 : 0
        z: 1000

        Behavior on opacity{ NumberAnimation{ duration: 200 } }

        Rectangle{
            id: removePaneButton
            anchors.top: parent.top
            anchors.right: parent.right
            width: parent.buttonWidth
            height: parent.buttonHeight
            color : "#03070b"
            Text {
                id: removePaneText
                text: qsTr("Remove Pane")
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 12
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                color: removePaneArea.containsMouse ? "#969aa1" : "#808691"
            }
            MouseArea{
                id : removePaneArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    paneMenu.visible = false
                    root.panes.removePane(root)
                }
            }
        }
    }

    Rectangle{
        id: viewWrapper
        anchors.topMargin: 30
        anchors.top: parent.top
        anchors.fill: parent
        color: root.backgroundColor

//        children: root.page ? [root.page] : null
    }
}
