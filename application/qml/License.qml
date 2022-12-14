/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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
import QtQuick.Window 2.1
import QtQuick.Controls 2.2
import visual.input 1.0 as Input

Rectangle{
    id: root

    function closeBox(){
        root.parent.parent.box = null
    }

    signal close()

    Keys.onEscapePressed: root.closeBox()

    property color backgroundColor: "#02070b"
    property color titleColor: "#afafaf"
    property color containerBackgroundColor: "#02070b"
    property color licenseBackgroundColor: "#010203"
    property color licenseTextColor: "#93999e"
    property color licenseItemBackground: "#101920"
    property color licenseItemTextColor: "#afafaf"
    property color licenseItemHighlightBackground: "#14222e"
    property color licenseItemRequiredBackground: "#874600"
    property color licenseItemRequiredHighlightBackground: "#994f00"
    property color licenseAcceptButtonColor: "#0d1921"
    property color licenseAcceptButtonHighlightColor: "#101f2a"
    property color licenseAcceptButtonBorder: "#0d2434"
    property color licenseAcceptButtonTextColor: "#93999e"

    anchors.centerIn: parent
    color: root.backgroundColor
    width: 800
    height: 550
    focus: true
    opacity: root.visible ? 1 : 0
    border.width: 1
    border.color: "#040e14"
    Behavior on opacity{ NumberAnimation{ duration: 250} }

    Rectangle{
        id: boxTop
        width: parent.width
        height: 35
        color: 'transparent'
        Text{
            anchors.centerIn: parent
            text: 'Manage Licenses'

            color: root.titleColor
            font.pixelSize: 12
            font.family: "Open Sans, sans-serif"
//            font.weight: Font.Light
        }
        Rectangle{
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            width: 30
            height: 30
            color: "transparent"
            Text{
                anchors.centerIn: parent
                text: "x"
                color: root.titleColor
                font.pixelSize: closeBoxArea.containsMouse ? 22 : 18
                font.family: "Open Sans, sans-serif"
                font.weight: Font.Light
                Behavior on font.pixelSize{ NumberAnimation{ duration: 250 } }
            }
            MouseArea{
                id: closeBoxArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: root.closeBox()
            }
        }
    }

    Rectangle{
        id: boxLeft
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.top: parent.top
        anchors.topMargin: 35
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        width: 230
        color: root.containerBackgroundColor

        // ListView
        ScrollView{
            id: scrollView

            anchors.fill: parent

            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            ScrollBar.vertical.contentItem: Input.ScrollbarHandle{
                color: "#1f2227"
                visible: scrollView.contentHeight > scrollView.height
            }
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
            ScrollBar.horizontal.contentItem: Input.ScrollbarHandle{
                color: "#1f2227"
                visible: scrollView.contentWidth > scrollView.width
            }

            ListView{
                id: licenseList
                model : lk.settings.file('license')
                width: parent.width
                height: parent.height
                clip: true

                boundsBehavior : Flickable.StopAtBounds
                highlightMoveDuration: 100

                property int delegateHeight : 50

                delegate: Rectangle{

                    property string licenseId: model.id
                    property bool isValid: model.valid

                    color: {
                        if ( model.highlight ){
                            return ListView.isCurrentItem || licenseItemArea.containsMouse ?
                                    root.licenseItemRequiredHighlightBackground : root.licenseItemRequiredBackground
                        } else {
                            return ListView.isCurrentItem || licenseItemArea.containsMouse ?
                                    root.licenseItemHighlightBackground : root.licenseItemBackground
                        }
                    }
                    width: licenseList.width
                    height: licenseList.delegateHeight
                    Text{
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        text: model.alias
                        color: root.licenseItemTextColor

                        font.family: "Open Sans, sans-serif"
                        font.pixelSize: 12
                        font.weight: Font.Light
                    }
                    Image{
                        visible: model.valid
                        anchors.right: parent.right
                        anchors.rightMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        source : "qrc:/images/license-check.png"
                    }

                    MouseArea{
                        id: licenseItemArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            licenseList.currentIndex = index
                        }
                    }
                }
            }
        }

    }
    Rectangle{
        id: boxRight
        anchors.left: boxLeft.right
        anchors.leftMargin: 2
        anchors.top: parent.top
        anchors.topMargin: 35
        anchors.right: parent.right
        anchors.rightMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        color: root.containerBackgroundColor

        ScrollView{
            id: boxScrollView
            anchors.fill: parent
            anchors.topMargin: 2
            anchors.leftMargin: 2
            anchors.rightMargin: 2
            anchors.bottomMargin: acceptButton.visible ? 60 : 2

            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            ScrollBar.vertical.contentItem: Input.ScrollbarHandle{
                color: "#1f2227"
                visible: boxScrollView.contentHeight > boxScrollView.height
            }
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
            ScrollBar.horizontal.contentItem: Input.ScrollbarHandle{
                color: "#1f2227"
                visible: boxScrollView.contentWidth > boxScrollView.width
            }

            Text{
                id: messageLabel
                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.top: parent.top
                anchors.topMargin: 12

                text: licenseList.currentItem
                      ? lk.settings.file('license').licenseText(licenseList.currentItem.licenseId)
                      : "No license selected."

                width: boxRight.width - 24

                wrapMode: Text.WordWrap

                color: "#ccc"
                font.pixelSize: 12
                font.family: "Open Sans, sans-serif"
                font.weight: Font.Light
            }

        }

        Rectangle{
            id: acceptButton
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            anchors.right: parent.right
            anchors.rightMargin: 15
            width: 100
            height: 35
            visible: licenseList.currentItem ?
                         licenseList.currentItem.isValid ? false : true : false
            color: acceptButtonArea.containsMouse ?
                       root.licenseAcceptButtonHighlightColor : root.licenseAcceptButtonColor
            border.width: 1
            border.color: root.licenseAcceptButtonBorder
            Text{
                color: root.licenseAcceptButtonTextColor
                font.pixelSize: 12
                font.family: "Open Sans, sans-serif"
                font.weight: Font.Light
                text: "Accept"
                anchors.centerIn: parent
            }
            MouseArea{
                id: acceptButtonArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: lk.settings.file('license').acceptLicense(licenseList.currentItem.licenseId)
            }
        }
    }

}
