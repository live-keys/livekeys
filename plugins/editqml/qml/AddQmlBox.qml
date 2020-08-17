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
import workspace 1.0 as Workspace

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
    property bool idChecked: false
    onActiveIndexChanged: {
        searchInput.text = ''
        root.addContainer.model.setFilter('')
        if (activeIndex === 2) idChecked = true
        root.addContainer.model.setCategoryFilter(activeIndex)
        root.addContainer.model.setImportFilter('')
        root.addContainer.model.setTypeFilter('')
        categoryList.currentIndex = 0
        listView.currentIndex = 0

    }

    function assignFocus(){
        searchInput.textInput.forceActiveFocus()
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

    Item {
        id: header
        height: title.height + buttonsContainer.height
        anchors.top: parent.top

        Text{
            id: title
            anchors.top: parent.top
            anchors.topMargin: 5
            height: 25
            width: parent.width
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

        Row {
            id: buttonsContainer
            anchors.top: title.bottom
            height: 30

            property QtObject buttonsStyle : QtObject{
                property QtObject textStyle: Workspace.TextStyle{}
                property QtObject hoverTextStyle: Workspace.TextStyle{}
                property color backgroundColor: '#070b0f'
                property color backgroundHoverColor: '#213355'
                property color borderColor: 'black'
                property color borderHoverColor: 'black'
                property double borderThickness: 0
                property double radius: 5
            }

            Workspace.TextButton{
                visible: !objectsOnly
                text: 'All'
                height: 22
                width: 70
                style: buttonsContainer.buttonsStyle
                color: activeIndex === 0 ? "#061a29" : "#111"

                onClicked : {
                    root.activeIndex = 0
                }
            }

            Workspace.TextButton{
                visible: !objectsOnly
                text: 'Property'
                height: 22
                width: 70
                style: buttonsContainer.buttonsStyle
                color: activeIndex === 1 ? "#061a29" : "#111"

                onClicked : {
                    root.activeIndex = 1
                }
            }

            Workspace.TextButton{
                text: 'Object'
                height: 22
                width: 70

                style: buttonsContainer.buttonsStyle
                color: activeIndex === 2 ? "#061a29" : "#111"

                onClicked : {
                    root.activeIndex = 2
                }
            }

            Workspace.TextButton{
                visible: !objectsOnly
                text: 'Event'
                height: 22
                width: 70
                style: buttonsContainer.buttonsStyle
                color: activeIndex === 3 ? "#061a29" : "#111"

                onClicked : {
                    root.activeIndex = 3
                }
            }

            Workspace.TextButton{
                visible: isForNode && !objectsOnly
                text: 'Function'
                height: 22
                width: 70
                style: buttonsContainer.buttonsStyle
                color: activeIndex === 4 ? "#061a29" : "#111"

                onClicked : {
                    root.activeIndex = 4
                }
            }
        }
    }


    Item {
        id: idInputItem
        visible: activeIndex === 2
        height: 30
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: header.height

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
            width: parent.width - 80
            height: 18
            anchors.right: parent.right
            anchors.rightMargin: 5
            color: "black"

            property QtObject idInputStyle : QtObject{
                property QtObject textStyle: Workspace.TextStyle{
                    font.family: "Open Sans, Courier"
                    font.pixelSize: 14
                    font.weight: Font.Light

                    color : "#afafaf"
                }
                property QtObject hintTextStyle: Workspace.TextStyle{}
                property color backgroundColor: '#070b0f'
                property color borderColor: '#323232'
                property double borderThickness: 1
                property color textSelectionColor: '#3d4856'
                property double radius: 3
            }

            Workspace.InputBox {

                id : idInput

                property bool userInput: false
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.right: parent.right
                anchors.rightMargin: 8

                style: parent.idInputStyle

                width: parent.width > implicitWidth ? parent.width : implicitWidth

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
        anchors.topMargin: idInputItem && idInputItem.visible? header.height + 30 : header.height
        anchors.left: parent.left
        anchors.leftMargin: 1
        width: parent.width - 1

        color: "transparent"
        height: 28

        property QtObject searchInputStyle : QtObject{
            property QtObject textStyle: Workspace.TextStyle{
                color : "#afafaf"
                font.family: "Open Sans, Courier"
                font.pixelSize: 12
                font.weight: Font.Light
            }
            property QtObject hintTextStyle: Workspace.TextStyle{}
            property color backgroundColor: '#070b0f'
            property color borderColor: '#323232'
            property double borderThickness: 1
            property color textSelectionColor: '#3d4856'
            property double radius: 3
        }

        Workspace.InputBox {
            id : searchInput
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 8

            width: parent.width > implicitWidth ? parent.width : implicitWidth

            onActiveFocusLost: {
                root.cancel()
            }
            style: searchInputBox.searchInputStyle

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
        if (!listView.currentItem) {
            root.cancel()
            return
        }

        var selector = root.activeIndex === 0 ? listView.currentItem.category : root.activeIndex

        var type = listView.currentItem.type
        var code = listView.currentItem.code
        var importSpace = listView.currentItem.importSpace

        if (selector === 2){
            var result = code
            if (idChecked && idInput.text !== "") result = result + "#" + idInput.text
            root.activeIndex = 2

            root.accept(importSpace, result)
        } else {
            root.activeIndex = selector
            root.accept(type, code)
        }
    }

    Item{
        id: container
        anchors.fill: parent
        anchors.topMargin: idInputItem && idInputItem.visible? header.height + 60 : header.height + 30


        property QtObject listViewStyle: QtObject{
            property QtObject labelStyle: Workspace.TextStyle{}
            property color backgroundColor: "#03070a"
            property color selectionBackgroundColor: "#091927"

            property double radius: 0
            property color borderColor: "#000"
            property double borderWidth: 0
            property double opacity: 0.95

            property Component scrollStyle: ScrollViewStyle {
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

        }

        Workspace.SelectableListView {
            id: categoryList
            anchors.top : parent.top
            anchors.left: parent.left

            height : root.height - container.anchors.topMargin
            width: root.width / 2

            model: {
                if (!root.addContainer) return null
                var types = root.addContainer.model.types()
                var importSpaces = root.addContainer.model.importSpaces()

                if (activeIndex === 0){
                    for (var i=1; i < types.length; ++i)
                        types[i] = "c  " + types[i]

                    importSpaces.splice(0,1)
                    for (var i=0; i < importSpaces.length; ++i)
                        importSpaces[i] = "{  " + importSpaces[i]

                    return types.concat(importSpaces)
                }
                return (activeIndex === 2 ? importSpaces : types)
            }
            style: container.listViewStyle
            delegate: Component{

                Rectangle{
                    width : categoryList.width

                    height : 25
                    color : ListView.isCurrentItem ? categoryList.style.selectionBackgroundColor : "transparent"
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
                            if (root.activeIndex === 0){
                                var selector = modelData[0]
                                var text = modelData.substring(3)

                                if (selector ===  'A') { // All
                                    root.addContainer.model.setImportFilter('')
                                    root.addContainer.model.setTypeFilter('')
                                } else if (selector === '{'){
                                    root.addContainer.model.setImportFilter(text)
                                    root.addContainer.model.setTypeFilter('')
                                } else {
                                    root.addContainer.model.setImportFilter('')
                                    root.addContainer.model.setTypeFilter(text)
                                }

                                return
                            }
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

        Workspace.SelectableListView {
            id: listView
            anchors.top : parent.top
            anchors.right: parent.right

            height : root.height - container.anchors.topMargin
            width: root.width / 2

            model: root.addContainer ? root.addContainer.model : null
            style: container.listViewStyle

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
                            root.acceptSelection()
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
