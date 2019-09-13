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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import editor 1.0
import editor.private 1.0
import base 1.0

Rectangle{
    id: root
    visible: false
    color: "transparent"
    signal closeFile(string path)

    onVisibleChanged: {
        if ( visible ){
            show()
        } else {
            close()
            cancel()
        }

    }

    signal cancel()
    signal open(string path)

    function show(){
        navigationInput.forceActiveFocus()
    }
    function close(){
        documentView.currentIndex = 0
        navigationInput.text = ''
        visible = false
    }


    Rectangle{
        id: navigationInputBox
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width - 150

        color: "#050d13"
        height: 30

        border.color: "#0c1117"
        border.width: 1

        TextInput{
            id : navigationInput
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 16

            color : "#afafaf"
            font.family: "Source Code Pro, Ubuntu Mono, Courier New, Courier"
            font.pixelSize: 12
            font.weight: Font.Light

            selectByMouse: true

            text: ""
            onTextChanged: {
                project.navigationModel.setFilter(text)
            }

            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: Qt.IBeamCursor
            }

            Keys.onPressed: {
                if ( event.key === Qt.Key_Down ){
                    documentView.highlightNext()
                    event.accepted = true
                } else if ( event.key === Qt.Key_Up ){
                    documentView.highlightPrev()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageDown ){
                    documentView.highlightNextPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageUp ){
                    documentView.highlightPrevPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_K && ( event.modifiers & Qt.ControlModifier) ){
                    root.close()
                    root.cancel()
                    event.accepted = true
                }
            }
            Keys.onReturnPressed: {
                root.open(documentView.currentItem.path)
                root.close()
                event.accepted = true
            }
            Keys.onEscapePressed: {
                root.close()
                root.cancel()
                event.accepted = true
            }
        }
    }

    Rectangle{
        anchors.fill: parent
        anchors.topMargin: 30
        color: "#030609"
        opacity: root.visible ? 0.92 : 0
        Behavior on opacity{ NumberAnimation{ duration: 250} }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                root.cancel()
                root.visible = false
                mouse.accepted = true;
            }
            onPressed: mouse.accepted = true;
            onReleased: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true;
            onPositionChanged: mouse.accepted = true;
            onPressAndHold: mouse.accepted = true;
            onWheel: wheel.accepted = true
        }
    }

    ScrollView{
        width: parent.width
        height: parent.height - 35
        anchors.top: parent.top
        anchors.topMargin: navigationInputBox.height

        style: ScrollViewStyle {
            transientScrollBars: false
            handle: Item {
                implicitWidth: 10
                implicitHeight: 10
                Rectangle {
                    color: "#0b1f2e"
                    anchors.fill: parent
                }
            }
            scrollBarBackground: Item{
                implicitWidth: 10
                implicitHeight: 10
                Rectangle{
                    anchors.fill: parent
                    color: "#091823"
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Rectangle{color: "transparent"}
            corner: Rectangle{color: "#091823"}
        }

        ListView{
            id: documentView
            model : navigationInput.text === '' ? project.documentModel : project.navigationModel
            width: parent.width
            height: parent.height
            clip: true
            currentIndex : 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            property int delegateHeight : 40

            function highlightNext(){
                if ( documentView.currentIndex + 1 <  documentView.count ){
                    documentView.currentIndex++;
                } else {
                    documentView.currentIndex = 0;
                }
            }
            function highlightPrev(){
                if ( documentView.currentIndex > 0 ){
                    documentView.currentIndex--;
                } else {
                    documentView.currentIndex = documentView.count - 1;
                }
            }

            function highlightNextPage(){
                var noItems = Math.floor(documentView.height / documentView.delegateHeight)
                if ( documentView.currentIndex + noItems < documentView.count ){
                    documentView.currentIndex += noItems;
                } else {
                    documentView.currentIndex = documentView.count - 1;
                }
            }
            function highlightPrevPage(){
                var noItems = Math.floor(documentView.height / documentView.delegateHeight)
                if ( documentView.currentIndex - noItems >= 0 ){
                    documentView.currentIndex -= noItems;
                } else {
                    documentView.currentIndex = 0;
                }
            }

            delegate: Rectangle{

                property string path: model.path

                color: ListView.isCurrentItem ? "#0f1b24" : "#060c10"
                width: root.width
                height: documentView.delegateHeight
                Text{
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    text: model.name !== '' ? model.name : 'untitled'
                    color: "#ebebeb"

                    font.family: "Open Sans, sans-serif"
                    font.pixelSize: 12
                    font.weight: Font.Light
                    font.italic: model.isOpen
                }
                Text{
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.top: parent.top
                    anchors.topMargin: 20

                    text: model.path
                    color: "#aaa5a5"

                    font.family: "Open Sans, sans-serif"
                    font.pixelSize: 12
                    font.weight: Font.Light
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        root.open(path)
                        root.close()
                    }
                }

                Text{
                    text: 'x'
                    width: 30
                    height: 30
                    visible : model.isOpen
                    anchors.right: parent.right
                    anchors.rightMargin: 0
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#acabab"
                    font.family: "Source Code Pro, sans-serif"
                    font.pixelSize: 16
                    font.weight: Font.Light
                    MouseArea{
                        anchors.fill: parent
                        onClicked: root.closeFile(model.path)
                    }
                }
            }
        }
    }

    Rectangle{
        anchors.centerIn: parent
        color: "#001122"
        width: 100
        height: 100
        visible: project.navigationModel.isIndexing
        Text{
            anchors.centerIn: parent
            text: 'Indexing...'
            color: "#aaa5a5"
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            font.weight: Font.Light
        }
    }

}
