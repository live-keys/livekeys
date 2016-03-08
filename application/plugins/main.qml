/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

import QtQuick 2.2
import QtQuick.Dialogs 1.0
import QtQuick.Controls 1.1
import Cv 1.0
import "view"

ApplicationWindow {
    id : root

    visible: true
    width: 1240
    height: 700
    color : "#293039"

    title: qsTr("Live CV")

    signal beforeCompile()
    signal afterCompile()

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
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.right: parent.right

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

        onFontPlus: if ( editorArea.font.pixelSize < 24 ) editorArea.font.pixelSize += 2
        onFontMinus: if ( editorArea.font.pixelSize > 10 ) editorArea.font.pixelSize -= 2
    }

    FileDialog {
        id: fileOpenDialog
        title: "Please choose a file"
        nameFilters: [ "Qml files (*.qml)", "All files (*)" ]
        selectExisting : true
        visible : false
        onAccepted: {
            editor.text = codeDocument.openFile(fileOpenDialog.fileUrl)
            editor.isDirty = false
        }
    }

    FileDialog{
        id : fileSaveDialog
        title: "Please choose a file"
        nameFilters: ["Qml files (*.qml)", "All files (*)"]
        selectExisting : false
        visible : false
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

            Editor{
                id: editor
                height: parent.height
                width: 400

                onSave: {
                    if ( codeDocument.file !==  "" ){
                        codeDocument.saveFile(editor.text)
                        editor.isDirty = false
                    }else
                        fileSaveDialog.open()
                }
                onOpen: {
                    header.openFile()
                }
                onToggleSize: {
                    if ( splitter.x < contentWrap.width / 2)
                        splitter.x = contentWrap.width - contentWrap.width / 4
                    else if ( splitter.x === contentWrap.width / 2 )
                        splitter.x = contentWrap.width / 4
                    else
                        splitter.x = contentWrap.width / 2
                }
            }


            Rectangle{
                id : viewer
                height : parent.height

                color : "#051521"

                Item {
                    id: tester
                    anchors.fill: parent
                    property string program: editor.text
                    property variant item
                    onProgramChanged: {
                        editor.isDirty = true
                        createTimer.restart()
                    }
                    Timer {
                        id: createTimer
                        interval: 1000
                        running: true
                        repeat : false
                        onTriggered: {
                            var newItem;
                            try {
                                root.beforeCompile()
                                newItem = Qt.createQmlObject("import QtQuick 2.1\n" + tester.program, tester, "canvas");
                            } catch (err) {
                                error.text = "Line " + err.qmlErrors[0].lineNumber + ": " + err.qmlErrors[0].message;
                            }
                            if ( tester.program === "Rectangle{\n}" || tester.program === "" )
                                editor.isDirty = false

                            if (newItem){
                                error.text = "";
                                if (tester.item) {
                                    tester.item.destroy();
                                }
                                tester.item = newItem;
                                root.afterCompile()
                            }
                        }
                    }
                }

                Rectangle{
                    id : errorWrap
                    anchors.bottom: parent.bottom
                    height : error.text !== '' ? 30 : 0
                    width : parent.width
                    color : "#141a1a"
                    Behavior on height {
                        SpringAnimation { spring: 3; damping: 0.1 }
                    }

                    Rectangle{
                        width : 14
                        height : parent.height
                        color : "#601818"
                        visible: error.text === "" ? false : true
                    }
                    Text {
                        id: error
                        anchors.left : parent.left
                        anchors.leftMargin: 25
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width
                        font.pointSize: editor.font.pointSize
                        text: ""
                        onTextChanged : console.log(text)
                        color: "#c5d0d7"
                    }
                }

            }

        }

/*
        Rectangle{
            id : splitter
             anchors.top: parent.top
             height : parent.height
             z : 100
             width : 2
             color : "#1c2228"
             Component.onCompleted: x = parent.width / 3.4
             onXChanged: {
                 viewer.width  = contentWrap.width - x - 2
                 editorWrap.width = x + 2
             }
             MouseArea{
                anchors.fill: parent
                drag.target: splitter
                drag.axis: Drag.XAxis
                drag.minimumX: 2
                drag.maximumX: contentWrap.width - 150
             }
             CursorArea{
                 anchors.fill: parent
                 cursorShape: Qt.SplitHCursor
             }
         }

        }
*/
    }

}
