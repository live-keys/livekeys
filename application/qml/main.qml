/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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
import Cv 1.0

ApplicationWindow {
    id : root

    visible: true
    width: 1240
    height: 700
    color : "#293039"

    title: qsTr("Live CV")

    signal beforeCompile()
    signal afterCompile()

    FontLoader{ id: ubuntuMonoBold;       source: "qrc:/fonts/UbuntuMono-Bold.ttf"; }
    FontLoader{ id: ubuntuMonoRegular;    source: "qrc:/fonts/UbuntuMono-Regular.ttf"; }
    FontLoader{ id: ubuntuMonoItalic;     source: "qrc:/fonts/UbuntuMono-Italic.ttf"; }
    FontLoader{ id: sourceCodeProLight;   source: "qrc:/fonts/SourceCodePro-Light.ttf"; }
    FontLoader{ id: sourceCodeProRegular; source: "qrc:/fonts/SourceCodePro-Regular.ttf"; }
    FontLoader{ id: sourceCodeProBold;    source: "qrc:/fonts/SourceCodePro-Bold.ttf"; }
    FontLoader{ id: openSansLight;        source: "qrc:/fonts/OpenSans-Light.ttf"; }
    FontLoader{ id: openSansRegular;      source: "qrc:/fonts/OpenSans-Regular.ttf"; }
    FontLoader{ id: openSansBold;         source: "qrc:/fonts/OpenSans-Bold.ttf"; }

    LogWindow{
        id : logWindow
        visible : false
        Component.onCompleted: width = root.width
        text : lcvlog.data
        onTextChanged: {
            if ( !visible && text !== "" )
                header.isLogWindowDirty = true
        }
    }

    Top{
        id : header
        visible: false
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        color: "#08141d"

        property var callback : function(){}

        isTextDirty: editor.isDirty

        property string action : ""

        onMessageYes: {
            fileSaveDialog.open()
        }
        onMessageNo: {
            callback()
            callback = function(){}
        }
        onNewFile  : {
            if ( editor.isDirty ){
                callback = function(){
                    editor.text    = "Rectangle{\n}"
                    editor.isDirty = false;
                }
                questionSave()
            } else {
                editor.text    = "Rectangle{\n}"
                editor.isDirty = false
            }
        }
        onOpenFile : {
            if ( editor.isDirty ){
                callback = function(){
                    fileOpenDialog.open()
                }
                questionSave()
            } else {
                fileOpenDialog.open()
            }
        }
        onSaveFile : {
            fileSaveDialog.open()
        }

        onToggleLogWindow : {
            if ( !logWindow.visible ){
                logWindow.show()
                isLogWindowDirty = false
            }
        }

        onFontPlus: if ( editor.font.pixelSize < 24 ) editor.font.pixelSize += 2
        onFontMinus: if ( editor.font.pixelSize > 10 ) editor.font.pixelSize -= 2
    }

    FileDialog {
        id: fileOpenDialog
        title: "Please choose a file"
        nameFilters: [ "Qml files (*.qml)", "All files (*)" ]
        selectExisting : true
        visible : isLinux ? true : false // fixes a display bug in some linux distributions
        onAccepted: {
            editor.text = codeDocument.openFile(fileOpenDialog.fileUrl)
            editor.isDirty = false
        }
        Component.onCompleted: {
            visible = false
            close()
        }
    }

    FileDialog{
        id : fileSaveDialog
        title: "Please choose a file"
        nameFilters: ["Qml files (*.qml)", "All files (*)"]
        selectExisting : false
        visible : isLinux ? true : false // fixes a display bug in some linux distributions
        onAccepted: {
            codeDocument.saveFile(fileSaveDialog.fileUrl, editor.text)
            editor.isDirty = false
            header.callback()
            header.callback = function(){}
        }
        onRejected:{
            header.callback()
            header.callback = function(){}
        }
        Component.onCompleted: {
            visible: false
            close()
        }
    }

    Rectangle{
        id : contentWrap
        anchors.top : header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height : parent.height - header.height

        SplitView{
            anchors.fill: parent
            orientation: Qt.Horizontal
            handleDelegate: Rectangle {
                implicitWidth: 1
                implicitHeight: 1
                color: "#071723"
            }

            Project{
                id: projectView
                height: parent.height
                width: 520
            }

            Editor{
                id: editor
                height: parent.height
                width: 400

                onSave: {
                    if ( codeDocument.file.toString() !==  "" ){
                        codeDocument.saveFile(editor.text)
                        editor.isDirty = false
                    } else
                        fileSaveDialog.open()
                }
                onOpen: {
                    header.openFile()
                }
                onToggleSize: {
                    if ( editor.width < contentWrap.width / 2)
                        editor.width = contentWrap.width - contentWrap.width / 4
                    else if ( editor.width === contentWrap.width / 2 )
                        editor.width = contentWrap.width / 4
                    else
                        editor.width = contentWrap.width / 2
                }

                Component.onCompleted: forceFocus()
            }


            Rectangle{
                id : viewer
                height : parent.height

                color : "#05111b"

                Item {
                    id: tester
                    anchors.fill: parent
                    property string program: editor.text
                    property variant item
                    onProgramChanged: {
//                        editor.isDirty = true
                        createTimer.restart()
                    }
                    Timer {
                        id: createTimer
                        interval: 1000
                        running: true
                        repeat : false
                        onTriggered: {
                            engine.createObjectAsync(
                                "import QtQuick 2.3\n" + tester.program,
                                tester,
                                codeDocument.file.toString() !== '' ? codeDocument.file : 'untitled.qml'
                            );
                        }
                    }
                    Connections{
                        target: engine
                        onAboutToCreateObject : {
                            root.beforeCompile()
                        }
                        onObjectCreated : {
                            error.text = ''
                            if (tester.item) {
                                tester.item.destroy();
                            }
                            tester.item = object;
                            root.afterCompile()
                        }
                        onObjectCreationError : {
                            var lastErrorsText = ''
                            for ( var i = 0; i < errors.length; ++i ){
                                var lerror = errors[i]
                                var errorFile = lerror.fileName
                                var index = errorFile.lastIndexOf('/')
                                if ( index !== -1 && index < errorFile.length - 1)
                                    errorFile = errorFile.substring(index + 1)

                                lastErrorsText +=
                                    (i !== 0 ? '<br>' : '') +
                                    '<a href=\"' + lerror.fileName + ':' + lerror.lineNumber + '\">' +
                                        errorFile + ':' + lerror.lineNumber +
                                    '</a>' +
                                    ' ' + lerror.message
                            }
                            error.text = lastErrorsText
                        }
                    }
                }

                Rectangle{
                    id : errorWrap
                    anchors.bottom: parent.bottom
                    height : error.text !== '' ? error.height + 20 : 0
                    width : parent.width
                    color : "#141a1a"
                    Behavior on height {
                        SpringAnimation { spring: 3; damping: 0.1 }
                    }

                    Rectangle{
                        width : 14
                        height : parent.height
                        color : "#601818"
                        visible: error.visible
                    }
                    Text {
                        id: error
                        anchors.left : parent.left
                        anchors.leftMargin: 25
                        anchors.verticalCenter: parent.verticalCenter

                        width: parent.width
                        wrapMode: Text.Wrap
                        textFormat: Text.StyledText

                        linkColor: "#c5d0d7"
                        font.family: "Ubuntu Mono, Courier New, Courier"
                        font.pointSize: editor.font.pointSize
                        text: ''
                        onTextChanged : if ( text !== '' ) console.log(text)
                        color: "#c5d0d7"
                        visible : text === "" ? false : true

                        onLinkActivated: {
                            project.openFile(link.substring(0, link.lastIndexOf(':')))
                        }
                    }
                }

            }

        }

    }

}
