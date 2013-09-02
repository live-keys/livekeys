import QtQuick 2.0
import QtQuick.Dialogs 1.0
import Cv 1.0

Rectangle {
    width: 1240
    height: 700
    color : "#293039"

    Top{
        id : header
        anchors.top : parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        property string action : ""
        function saveFileDialog(){
            fileDialog.selectExisting = false
            fileDialog.open()
        }
        function executeAction(){
            if ( action === "new" )
                editor.text = "Rectangle{\n}"
            else if ( action === "open" ){
                fileDialog.selectExisting = true
                fileDialog.open()
            }
            action = ""
        }

        onMessageYes: {
            saveFile()
        }
        onMessageNo: {
            executeAction()
        }
        onNewFile  : {
            action = "new"
            if ( editor.text !== "" && editor.text !== "Rectangle{\n}" )
                header.questionSave()
            else
                executeAction()
        }
        onOpenFile : {
            action = "open"
            if ( editor.text !== "" && editor.text !== "Rectangle{\n}" )
                header.questionSave()
            else {
                executeAction()
            }
        }
        onSaveFile : saveFileDialog()

        onFontPlus: if ( editor.font.pixelSize < 24 ) editor.font.pixelSize += 2
        onFontMinus: if ( editor.font.pixelSize > 10 ) editor.font.pixelSize -= 2
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        nameFilters: [ "Qml files (*.qml)", "All files (*)" ]
        selectExisting : false
        visible : false
        onAccepted: {
            if ( !selectExisting ){
                codeDocument.saveFile(fileDialog.fileUrl, editor.text)
                header.executeAction()
            } else {
                editor.text = codeDocument.openFile(fileDialog.fileUrl)
            }
        }
    }

    Rectangle{
        id : contentWrap
        anchors.top : header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height : parent.height - header.height

        Rectangle{
            id : editorWrap
            color : "#1c2228"
            height : parent.height
            anchors.left: parent.left
            clip : true

            Flickable {
                id: flick

                anchors.fill: parent
                anchors.leftMargin: 9
                anchors.topMargin: 8
                anchors.bottomMargin: 30
                contentWidth: editor.paintedWidth
                contentHeight: editor.paintedHeight

                function ensureVisible(r)
                {
                    if (contentX >= r.x)
                        contentX = r.x;
                    else if (contentX+width <= r.x+r.width)
                        contentX = r.x+r.width-width;
                    if (contentY >= r.y)
                        contentY = r.y;
                    else if (contentY+height <= r.y+r.height)
                        contentY = r.y+r.height-height;
                }

                Editor{
                    id : editor
                    text : "Rectangle{\n}"
                    anchors.left: parent.left
                    width : flick.width
                    height : flick.height
                    Behavior on font.pixelSize {
                        NumberAnimation { duration: 40 }
                    }
                }

            }

            Rectangle{
                id : errorWrap
                anchors.bottom: parent.bottom
                height : 30
                width : parent.width
                color : "#1c1f23"
                Rectangle{
                    width : 10
                    height : parent.height
                    color : "#401818"
                    visible: error.text === "" ? false : true
                }
                Text {
                    id: error
                    anchors.left : parent.left
                    anchors.leftMargin: 25
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width
                    font.pointSize: 25 * editor.fontScale
                    text: ""
                    color: "#95a0a7"
                }
            }

        }

        Rectangle{
            id : splitter
             anchors.top: parent.top
             height : parent.height
             z : 100
             width : 2
             color : "#1c2228"
             x : parent.width / 3.4
             onXChanged: {
                 viewer.width  = contentWrap.width - x - 2
                 editorWrap.width = x + 2
             }
             MouseArea {
                anchors.fill: parent
                drag.target: splitter
                drag.axis: Drag.XAxis
                drag.minimumX: 150
                drag.maximumX: contentWrap.width - 150
             }
             CursorArea{
                 anchors.fill: parent
                 cursorShape: Qt.SplitHCursor
             }
         }

        Rectangle{
            id : viewer
            anchors.left : splitter.right
            height : parent.height
            color : "#293039"

            Item {
                id: tester
                anchors.fill: parent
                property string program: editor.text
                property variant item
                onProgramChanged: createTimer.restart()
                Timer {
                    id: createTimer
                    interval: 1000
                    running: true
                    onTriggered: {
                        var newItem;
                        try {
                            newItem = Qt.createQmlObject("import QtQuick 2.0\nimport Cv 1.0\n" + tester.program, tester, "testee");
                        } catch (err) {
                            error.text = err.qmlErrors[0].lineNumber + ":" + err.qmlErrors[0].message;
                        }
                        if (newItem){
                            error.text = "";
                            if (tester.item) {
                                tester.item.destroy();
                            }
                            tester.item = newItem;
                        }
                    }
                }
            }
        }


    }

}
