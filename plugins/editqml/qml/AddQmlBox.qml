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
import base 1.0
import live 1.0
import editor.private 1.0

Rectangle{
    id: root

    width: 380 + (isForNode? 100: 0)
    height: 280
    color: "#03070a"
    opacity: 0.95
    objectName: "addQmlBox"

    property color selectionColor: "#091927"

    property QtObject addContainer : null

    property string fontFamily: 'Open Sans, Courier'
    property int fontSize: 12
    property int smallFontSize: 9
    property var codeQmlHandler: null

    property bool isForNode: false
    property bool objectsOnly: false

    onObjectsOnlyChanged: {
        if (objectsOnly)
            activeIndex = 2
    }

    property int activeIndex : 0
    property bool idChecked: true
    onActiveIndexChanged: {
        searchInput.text = ''
        root.addContainer.model.setFilter('')
        if (activeIndex === 2) idChecked = true
        root.addContainer.model.setCategoryFilter(activeIndex)
    }

    function assignFocus(){
        searchInput.forceActiveFocus()
    }

    property var cancel: function(){ }
    property var accept : function(type, data){ }

    function getCompletion(){
        if ( listView.currentItem ){
            return listView.currentItem.completion
        }
        return ""
    }

    function highlightNext(){
        if ( listView.currentIndex + 1 <  listView.count ){
            listView.currentIndex++;
        } else {
            listView.currentIndex = 0;
        }
    }
    function highlightPrev(){
        if ( listView.currentIndex > 0 ){
            listView.currentIndex--;
        } else {
            listView.currentIndex = listView.count - 1;
        }
    }

    function highlightNextPage(){
        var noItems = Math.floor(listView.height / 25)
        if ( listView.currentIndex + noItems < listView.count ){
            listView.currentIndex += noItems;
        } else {
            listView.currentIndex = listView.count - 1;
        }
    }
    function highlightPrevPage(){
        var noItems = Math.floor(listView.height / 25)
        if ( listView.currentIndex - noItems >= 0 ){
            listView.currentIndex -= noItems;
        } else {
            listView.currentIndex = 0;
        }
    }

    Text{
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5

        color : "#efefef"
        font.family: "Open Sans, sans-serif"
        font.pixelSize: 12
        font.weight: Font.Normal

        text: {
            if (!addContainer) return ""
            var result;
            switch (activeIndex) {
                case 0: result = "All"; break;
                case 1: result = "Properties"; break;
                case 2: result = "Items"; break;
                case 3: result = "Events"; break;
                case 4: result = "Functions"; break;
            }

            result = result + " for "  + addContainer.objectType
        }
    }

    TextButton{
        visible: !objectsOnly
        anchors.top: parent.top
        anchors.topMargin: 3
        anchors.right: parent.right
        anchors.rightMargin: {
            if (objectsOnly) return 0
            return 215+ (isForNode? 71: 0)
        }
        text: 'All'
        height: 22
        width: 70
        fontPixelSize: 12
        backgroundColor: isActive ? "#061a29" : "#111"
        fontFamily: "Open Sans, sans-serif"
        radius: 5

        property bool isActive : activeIndex === 0
        onClicked : {
            root.activeIndex = 0
        }
    }

    TextButton{
        visible: !objectsOnly
        anchors.top: parent.top
        anchors.topMargin: 3
        anchors.right: parent.right
        anchors.rightMargin: {
            if (objectsOnly) return 0
            return 144+ (isForNode? 71: 0)
        }
        text: 'Property'
        height: 22
        width: 70
        fontPixelSize: 12
        backgroundColor: isActive ? "#061a29" : "#111"
        fontFamily: "Open Sans, sans-serif"
        radius: 5

        property bool isActive : activeIndex === 1
        onClicked : {
            root.activeIndex = 1
        }
    }

    TextButton{
        anchors.top: parent.top
        anchors.topMargin: 3
        anchors.right: parent.right
        anchors.rightMargin: {
            if (objectsOnly) return 0
            return 73+ (isForNode? 71: 0)
        }

        text: 'Object'
        height: 22
        width: 70
        fontPixelSize: 12
        fontFamily: "Open Sans, sans-serif"
        radius: 5

        backgroundColor: isActive ? "#061a29" : "#111"
        property bool isActive : activeIndex === 2

        onClicked : {
            root.activeIndex = 2
        }
    }

    TextButton{
        visible: !objectsOnly
        anchors.top: parent.top
        anchors.topMargin: 3
        anchors.right: parent.right
        anchors.rightMargin: {
            if (objectsOnly) return 0
            return 2+ (isForNode? 71: 0)
        }
        text: 'Event'
        height: 22
        width: 70
        fontPixelSize: 12
        backgroundColor: isActive ? "#061a29" : "#111"
        fontFamily: "Open Sans, sans-serif"
        radius: 5

        property bool isActive : activeIndex === 3
        onClicked : {
            root.activeIndex = 3
        }
    }

    TextButton{
        anchors.top: parent.top
        visible: isForNode && !objectsOnly
        anchors.topMargin: 3
        anchors.right: parent.right
        anchors.rightMargin: {
            if (objectsOnly) return 0
            return 2
        }
        text: 'Function'
        height: 22
        width: 70
        fontPixelSize: 12
        backgroundColor: isActive ? "#061a29" : "#111"
        fontFamily: "Open Sans, sans-serif"
        radius: 5

        property bool isActive : activeIndex === 4
        onClicked : {
            root.activeIndex = 4
        }
    }


    Item {
        id: idInputItem
        visible: activeIndex === 2
        height: 30
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: 25

        Rectangle {
            x: 15
            y: 5
            width: 16
            height: 16
            border.width: 2
            border.color: "#0d1f2d"
            color: "black"

            Rectangle {
                color: "#0d1f2d"
                width: 8
                height: 8
                x: 4
                y: 4
                visible: idChecked
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    idChecked = !idChecked
                }
            }
        }

        Text {
            x: 45
            y: 3
            text: "Id"
            color : "#efefef"
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 14
            font.weight: Font.Normal
        }

        Rectangle {
            x: 60
            y: 5
            border.width: 1
            border.color: "#0d1f2d"
            width: parent.width - 80
            height: 18
            anchors.right: parent.right
            anchors.rightMargin: 5
            color: "black"
            TextInput{
                id : idInput
                property bool userInput: false

                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.right: parent.right
                anchors.rightMargin: 8

                width: parent.width > implicitWidth ? parent.width : implicitWidth

                color : "#afafaf"
                font.family: "Open Sans, Courier"
                font.pixelSize: 14
                font.weight: Font.Light

                selectByMouse: true

                text: {
                    if (!listView || !listView.currentItem) return text

                    if (!userInput)
                    {
                        var id = listView.currentItem.code
                        var result = id[0].toLowerCase() + id.substring(1)
                        var index = 1
                        if (codeQmlHandler)
                        {

                            var origResult = result
                            var docIds = codeQmlHandler.getDocumentIds()

                            while (true)
                            {
                                var found = false
                                for (var i=0; i<docIds.length;++i)
                                    if (docIds[i] === result)
                                    {
                                        found = true
                                        break
                                    }
                                if (!found) break
                                index++
                                result = origResult + index
                            }

                            return result
                        }

                        return result
                    }

                    return text
                }

                MouseArea{
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: Qt.IBeamCursor
                }

                Keys.onPressed: {
                    if (focus)
                    {
                        userInput = true
                    }
                }
            }
        }

    }

    Rectangle{
        id: searchInputBox
        anchors.top: parent.top
        anchors.topMargin: idInputItem && idInputItem.visible? 55 : 25
        anchors.left: parent.left
        anchors.leftMargin: 1
        width: parent.width - 1

        color: "transparent"
        height: 28

        border.color: "#0d1f2d"
        border.width: 1

        TextInput{
            id : searchInput
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 8

            width: parent.width > implicitWidth ? parent.width : implicitWidth

            color : "#afafaf"
            font.family: "Open Sans, Courier"
            font.pixelSize: 12
            font.weight: Font.Light

            selectByMouse: true

            text: ""
            onTextChanged: {
                root.addContainer.model.setFilter(text)
            }

            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: Qt.IBeamCursor
            }

            Keys.onPressed: {
                if ( event.key === Qt.Key_Down ){
                    root.highlightNext()
                    event.accepted = true
                } else if ( event.key === Qt.Key_Up ){
                    root.highlightPrev()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageDown ){
                    root.highlightNextPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageUp ){
                    root.highlightPrevPage()
                    event.accepted = true
                }
            }
            Keys.onReturnPressed: {
                acceptSelection()
            }
            Keys.onEscapePressed: {
                root.cancel()
            }
        }
    }

    function acceptSelection(){
        var selector = root.activeIndex === 0 ? listView.currentItem.category : root.activeIndex

        if ( selector === 1 ){
            root.activeIndex = 1
            root.accept(listView.currentItem.type, listView.currentItem.code)
        } else if (selector === 2){
            root.activeIndex = 2
            var result = listView.currentItem.code
            if (idChecked && idInput.text !== "") result = result + "#" + idInput.text
            root.accept(listView.currentItem.importSpace, result)
        } else if (selector === 3){
            root.activeIndex = 3
            root.accept(listView.currentItem.type, listView.currentItem.code)
        } else if (selector === 4){
            root.activeIndex = 4
            root.accept(listView.currentItem.type, listView.currentItem.code)
        }
    }

    Item{
        id: container
        anchors.fill: parent
        anchors.topMargin: idInputItem && idInputItem.visible? 85 : 55

        ScrollView{
            anchors.top : parent.top
            anchors.left: parent.left

            height : root.height - container.anchors.topMargin
            width: root.width / 2

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
                scrollBarBackground: Item {
                    implicitWidth: 10
                    implicitHeight: 10
                    Rectangle{
                        anchors.fill: parent
                        color: root.color
                    }
                }
                decrementControl: null
                incrementControl: null
                frame: Rectangle{color: "transparent"}
                corner: Rectangle{color: root.color}
            }

            ListView{
                id : categoryList
                anchors.fill: parent
                anchors.rightMargin: 2
                anchors.bottomMargin: 5
                anchors.topMargin: 0
                visible: true
                opacity: root.opacity
                model: root.addContainer ? root.addContainer.model.types() : null

                currentIndex: 0
                onCountChanged: currentIndex = 0

                boundsBehavior : Flickable.StopAtBounds
                highlightMoveDuration: 100

                delegate: Component{

                    Rectangle{
                        width : categoryList.width
                        height : 25
                        color : ListView.isCurrentItem ? root.selectionColor : "transparent"
                        Text{
                            id: label
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter

                            font.family: root.fontFamily
                            font.pixelSize: root.fontSize
                            font.weight: Font.Light

                            color: "#fafafa"
                            text: modelData
                        }

                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                categoryList.currentIndex = index
                                if ( modelData === 'All' ) {
                                    if (root.activeIndex === 2){
                                        root.addContainer.model.setImportFilter('')
                                    } else {
                                        root.addContainer.model.setTypeFilter('')
                                    }
                                }
                                else {
                                    if (root.activeIndex === 2) {
                                        root.addContainer.model.setImportFilter(modelData)
                                    } else {
                                        root.addContainer.model.setTypeFilter(modelData)
                                    }

                                }
                            }
                        }
                    }
                }
            }
        }

        ScrollView{
            anchors.top : parent.top
            anchors.right: parent.right

            height : root.height - container.anchors.topMargin
            width: root.width / 2

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
                        color: root.color
                    }
                }
                decrementControl: null
                incrementControl: null
                frame: Rectangle{color: "transparent"}
                corner: Rectangle{color: root.color}
            }

            ListView{
                id : listView
                anchors.fill: parent
                anchors.rightMargin: 2
                anchors.bottomMargin: 5
                anchors.topMargin: 0
                visible: true
                opacity: root.opacity
                model: root.addContainer ? root.addContainer.model : null

                currentIndex: 0
                onCountChanged: currentIndex = 0

                boundsBehavior : Flickable.StopAtBounds
                highlightMoveDuration: 100

                delegate: Component{
                    Rectangle{
                        property string objectType : model.objectType
                        property string type : model.type
                        property string code: model.code
                        property int category: model.category

                        width : listView.width
                        height : 25
                        color : ListView.isCurrentItem ? root.selectionColor : "transparent"
                        Text{
                            id: label
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter

                            font.family: root.fontFamily
                            font.pixelSize: root.fontSize
                            font.weight: Font.Light

                            color: "#fafafa"
                            text: model.label
                        }

                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                listView.currentIndex = index
                            }
                            onDoubleClicked: {
                                acceptSelection()
                            }
                        }
                    }
                }

            }
        }
    }

    Keys.onEscapePressed: {
        root.cancel()
    }

}
