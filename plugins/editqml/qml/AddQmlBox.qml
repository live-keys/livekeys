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

    width: 350
    height: 280
    color: "#03070a"
    opacity: 0.95

    property color selectionColor: "#091927"

    property QtObject addContainer : null

    property string fontFamily: 'Open Sans, Courier'
    property int fontSize: 12
    property int smallFontSize: 9

    property int activeIndex : 0
    onActiveIndexChanged: {
        searchInput.text = ''
        root.addContainer.propertyModel.setFilter('')
        root.addContainer.itemModel.setFilter('')
    }

    property ListView activeList : activeIndex === 0 ? propertyList : itemList

    function assignFocus(){
        searchInput.forceActiveFocus()
    }

    property var cancel: function(){ }
    property var accept : function(type, data){ }

    function getCompletion(){
        if ( root.activeList.currentItem ){
            return root.activeList.currentItem.completion
        }
        return ""
    }

    function highlightNext(){
        if ( root.activeList.currentIndex + 1 <  root.activeList.count ){
            root.activeList.currentIndex++;
        } else {
            root.activeList.currentIndex = 0;
        }
    }
    function highlightPrev(){
        if ( root.activeList.currentIndex > 0 ){
            root.activeList.currentIndex--;
        } else {
            root.activeList.currentIndex = root.activeList.count - 1;
        }
    }

    function highlightNextPage(){
        var noItems = Math.floor(root.activeList.height / 25)
        if ( root.activeList.currentIndex + noItems < root.activeList.count ){
            root.activeList.currentIndex += noItems;
        } else {
            root.activeList.currentIndex = root.activeList.count - 1;
        }
    }
    function highlightPrevPage(){
        var noItems = Math.floor(root.activeList.height / 25)
        if ( root.activeList.currentIndex - noItems >= 0 ){
            root.activeList.currentIndex -= noItems;
        } else {
            root.activeList.currentIndex = 0;
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

        text: addContainer ? (activeIndex === 0 ? "Properties" : "Items") + " for " + addContainer.objectType : ""
    }

    TextButton{
        anchors.top: parent.top
        anchors.topMargin: 3
        anchors.right: parent.right
        anchors.rightMargin: 73
        text: 'Property'
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
        anchors.top: parent.top
        anchors.topMargin: 3
        anchors.right: parent.right
        anchors.rightMargin: 2
        text: 'Object'
        height: 22
        width: 70
        fontPixelSize: 12
        fontFamily: "Open Sans, sans-serif"
        radius: 5

        backgroundColor: isActive ? "#061a29" : "#111"
        property bool isActive : activeIndex === 1

        onClicked : {
            root.activeIndex = 1
        }
    }

    Rectangle{
        id: searchInputBox
        anchors.top: parent.top
        anchors.topMargin: 25
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
            onFocusChanged: {
                if ( !focus )
                    root.cancel()
            }

            text: ""
            onTextChanged: {
                if ( root.activeIndex === 0 ){
                    root.addContainer.propertyModel.setFilter(text)
                } else {
                    root.addContainer.itemModel.setFilter(text)
                }
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
                if ( root.activeIndex === 0 ){
                    root.accept(propertyList.currentItem.type, propertyList.currentItem.code)
                } else {
                    root.accept(itemList.currentItem.importSpace, itemList.currentItem.code)
                }
            }
            Keys.onEscapePressed: {
                root.cancel()
            }
        }
    }

    Item{
        id: propertiesContainer
        anchors.fill: parent
        anchors.topMargin: 55
        visible: root.activeIndex === 0

        ScrollView{
            anchors.top : parent.top
            anchors.left: parent.left

            height : root.height - 55
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
                id : propertyCategoryList
                anchors.fill: parent
                anchors.rightMargin: 2
                anchors.bottomMargin: 5
                anchors.topMargin: 0
                visible: true
                opacity: root.opacity

                model: root.addContainer ? root.addContainer.propertyModel.types() : null

                currentIndex: 0
                onCountChanged: currentIndex = 0

                boundsBehavior : Flickable.StopAtBounds
                highlightMoveDuration: 100

                delegate: Component{

                    Rectangle{
                        width : propertyCategoryList.width
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
                                propertyCategoryList.currentIndex = index
                                if ( modelData === 'All' )
                                    root.addContainer.propertyModel.setTypeFilter('')
                                else
                                    root.addContainer.propertyModel.setTypeFilter(modelData)
                            }
                        }
                    }
                }
            }
        }

        ScrollView{
            anchors.top : parent.top
            anchors.right: parent.right

            height : root.height - 55
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
                id : propertyList
                anchors.fill: parent
                anchors.rightMargin: 2
                anchors.bottomMargin: 5
                anchors.topMargin: 0
                visible: true
                opacity: root.opacity
                model: root.addContainer ? root.addContainer.propertyModel : null

                currentIndex: 0
                onCountChanged: currentIndex = 0

                boundsBehavior : Flickable.StopAtBounds
                highlightMoveDuration: 100

                delegate: Component{

                    Rectangle{
                        property string objectType : model.objectType
                        property string type : model.type
                        property string code: model.code

                        width : propertyList.width
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
                                propertyList.currentIndex = index
                            }
                            onDoubleClicked: {
                                if ( root.activeIndex === 0 ){
                                    root.accept(propertyList.currentItem.type, propertyList.currentItem.code)
                                } else {
                                    root.accept(itemList.currentItem.importSpace, itemList.currentItem.code)
                                }
                            }
                        }
                    }
                }

            }
        }
    }

    Item{
        id: itemsContainer
        anchors.fill: parent
        anchors.topMargin: 55
        visible: root.activeIndex === 1

        ScrollView{
            anchors.top : parent.top
            anchors.left: parent.left

            height : root.height - 55
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
                id : itemsCategoryList
                anchors.fill: parent
                anchors.rightMargin: 2
                anchors.bottomMargin: 5
                anchors.topMargin: 0
                visible: true
                opacity: root.opacity
                model: root.addContainer ? root.addContainer.itemModel.importSpaces() : null

                currentIndex: 0
                onCountChanged: currentIndex = 0

                boundsBehavior : Flickable.StopAtBounds
                highlightMoveDuration: 100

                delegate: Component{

                    Rectangle{
                        width : itemsCategoryList.width
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
                                itemsCategoryList.currentIndex = index
                                if ( modelData === 'All' )
                                    root.addContainer.itemModel.setImportFilter('')
                                else
                                    root.addContainer.itemModel.setImportFilter(modelData)
                            }
                        }
                    }
                }

            }
        }


        ScrollView{
            anchors.top : parent.top
            anchors.right: parent.right

            height : root.height - 55
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
                id : itemList
                anchors.fill: parent
                anchors.rightMargin: 2
                anchors.bottomMargin: 5
                anchors.topMargin: 0
                visible: true
                opacity: root.opacity
                model: root.addContainer ? root.addContainer.itemModel : null

                currentIndex: 0
                onCountChanged: currentIndex = 0

                boundsBehavior : Flickable.StopAtBounds
                highlightMoveDuration: 100

                delegate: Component{

                    Rectangle{
                        property string code: model.code

                        width : itemList.width
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
                                itemList.currentIndex = index
                            }
                            onDoubleClicked: {
                                if ( root.activeIndex === 0 ){
                                    root.accept(propertyList.currentItem.type, propertyList.currentItem.code)
                                } else {
                                    root.accept(itemList.currentItem.importSpace, itemList.currentItem.code)
                                }
                            }
                        }
                    }
                }

            }
        }
    }

}
