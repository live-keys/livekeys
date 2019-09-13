/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.0
import base 1.0
import editor 1.0
import editor.private 1.0
import live 1.0

ApplicationWindow{
    id : root

    visible: true
    width: 1240
    minimumWidth: 640
    height: 700
    minimumHeight: 400
    color : "#000509"
    objectName: "window"
    title: qsTr("Livekeys")

    property int prevVisibility: 2

    property QtObject dialogs : QtObject{

        function saveFile(options, callback){
            var title = options.title ? options.title : "Pleace choose a file";
            var filters = options.filters ? options.filters : ["All files (*)"]
            var selectExisting = false

            fileSaveDialog.title = title
            fileSaveDialog.nameFilters = filters
            fileSaveDialog.callback = callback

            fileSaveDialog.open()
        }

        function openFile(options, callback){
            var title = options.title ? options.title : "Pleace choose a file";
            var filters = options.filters ? options.filters : ["All files (*)"]
            var selectExisting = false

            fileOpenDialog.title = title
            fileOpenDialog.nameFilters = filters
            fileOpenDialog.callback = callback

            fileOpenDialog.open()
        }

        function openDir(options, callback){
            var title = options.title ? options.title : "Pleace choose a directory";

            dirOpenDialog.title = title
            dirOpenDialog.callback = callback

            dirOpenDialog.open()
        }

        function message(message, options){
            var ob = overlayBoxFactory.createObject(root)
            ob.box = messageDialogConfirmFactory.createObject()
            ob.box.show(message, options)
            return ob
        }

        function overlayBox(object){
            var ob = overlayBoxFactory.createObject(root)
            ob.box = object
            return ob
        }
    }

    property QtObject handle: QtObject{

        function toggleFullScreen(){
            if (root.visibility !== Window.FullScreen){
                root.prevVisibility = root.visibility
                root.visibility = Window.FullScreen
            } else {
                root.visibility = root.prevVisibility
            }
        }

        function minimize(){
            root.showMinimized()
        }

        function maximize(){
            root.showMaximized()
        }
    }

    property Item container : container

    FontLoader{ id: ubuntuMonoBold;       source: "qrc:/fonts/UbuntuMono-Bold.ttf"; }
    FontLoader{ id: ubuntuMonoRegular;    source: "qrc:/fonts/UbuntuMono-Regular.ttf"; }
    FontLoader{ id: ubuntuMonoItalic;     source: "qrc:/fonts/UbuntuMono-Italic.ttf"; }
    FontLoader{ id: sourceCodeProLight;   source: "qrc:/fonts/SourceCodePro-Light.ttf"; }
    FontLoader{ id: sourceCodeProRegular; source: "qrc:/fonts/SourceCodePro-Regular.ttf"; }
    FontLoader{ id: sourceCodeProBold;    source: "qrc:/fonts/SourceCodePro-Bold.ttf"; }
    FontLoader{ id: openSansLight;        source: "qrc:/fonts/OpenSans-Light.ttf"; }
    FontLoader{ id: openSansBold;         source: "qrc:/fonts/OpenSans-LightItalic.ttf"; }
    FontLoader{ id: openSansRegular;      source: "qrc:/fonts/OpenSans-Regular.ttf"; }
    FontLoader{ id: openSansLightItalic;  source: "qrc:/fonts/OpenSans-Bold.ttf"; }


    FileDialog {
        id: fileOpenDialog
        title: "Please choose a file"
        nameFilters: ["All files (*)" ]
        selectExisting : true
        visible : script.environment.os.platform === 'linux' ? true : false // fixes a display bug in some linux distributions

        property var callback: null

        onAccepted: {
            fileOpenDialog.callback(fileOpenDialog.fileUrl)
            fileOpenDialog.callback = null
        }
        onRejected:{
            fileOpenDialog.callback = null
        }

        Component.onCompleted: {
            if ( script.environment.os.platform === 'darwin' )
                folder = '~' // fixes a warning message that the path was constructed with an empty filename
            visible = false
            close()
        }
    }

    FileDialog {
        id: dirOpenDialog
        title: "Please choose a directory"
        selectExisting : true
        selectMultiple : false
        selectFolder : true

        visible : script.environment.os.platform === 'linux' ? true : false /// fixes a display bug in some linux distributions

        property var callback: null

        onAccepted: {
            dirOpenDialog.callback(dirOpenDialog.fileUrl)
            dirOpenDialog.callback = null
        }
        onRejected:{
            dirOpenDialog.callback = null
        }

        Component.onCompleted: {
            if ( script.environment.os.platform === 'darwin' )
                folder = '~' // fixes a warning message that the path was constructed with an empty filename
            visible = false
            close()
        }
    }

    FileDialog{
        id : fileSaveDialog
        title: "Please choose a file"
        nameFilters: ["All files (*)"]
        selectExisting : false
        visible : script.environment.os.platform === 'linux' ? true : false /// fixes a display bug in some linux distributions

        property var callback: null

        onAccepted: {
            fileSaveDialog.callback(fileSaveDialog.fileUrl)
            fileSaveDialog.callback = null
        }
        onRejected:{
            fileSaveDialog.callback = null
        }
        Component.onCompleted: {
            if ( script.environment.os.platform === 'darwin' )
                folder = '~' // fixes a warning message that the path was constructed with an empty filename
            visible: false
            close()
        }
    }

    Component{
        id: messageDialogConfirmFactory

        MessageDialogConfirm{
            anchors.fill: parent
            color: "#02070b"
            border.width: 1
            border.color: "#0c151c"
        }
    }

    Component{
        id: overlayBoxFactory

        OverlayBox{
            anchors.fill: parent
        }
    }

    Item{
        id: container
        anchors.fill: parent
    }

    // Logo

    Image{
        id: logo

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 20
        visible: true

        opacity: 0.3
        source : "qrc:/images/logo.png"
        z: 2222


        Connections{
            target: lk
            onLayerReady: {
                if ( layer.name === 'workspace' ){
                    logo.opacity = 1.0
                }
            }
        }
    }

}
