import QtQuick 2.3
import QtQuick.Controls 2.2 as QC
import visual.shapes 1.0
import editor.private 1.0

Rectangle{
    id: runControl

    objectName: "runControl"

    width: parent ? parent.width : 200
    height: 25

    property QtObject editor: editor
    property QtObject currentTheme: null
    property var runTrigger: runControl.editor && runControl.editor.code.language ? runControl.editor.code.language.runTrigger : null

    property color topColor: currentTheme ? currentTheme.paneTopBackground : 'black'
    color : currentTheme ? currentTheme.panebackgroundOverlay : 'black'

    Rectangle{
        width: 30
        height: parent.height - 2
        color: runControl.currentTheme ? runControl.currentTheme.paneTopBackground : 'black'
        y: 1
        x: 1

        Triangle{
            id: compileButtonShape
            anchors.centerIn: parent
            width: compileButton.containsMouse ? 7 : 5
            height: compileButton.containsMouse ? 11 : 10
            state : "Released"
            rotation: Triangle.Right
            color: 'white'
            Behavior on height{ NumberAnimation{ duration: 100 } }
            Behavior on width{ NumberAnimation{ duration: 100 } }
        }

        MouseArea{
            id : compileButton
            anchors.fill: parent
            hoverEnabled: true
            onClicked: { runControl.editor.code.language.buildRunChannel() }
        }
    }

    Item{
        id: runnableNameContainer
        width: runnableName.width
        height: parent.height - 2
        anchors.left: parent.left
        anchors.leftMargin: 40
        y: 1

        Text{
            id: runnableName
            anchors.verticalCenter: parent.verticalCenter
            color: "#808691"
            text: {
                if ( !runControl.editor )
                    return ''
                if ( !runControl.editor.code.language )
                    return ''
                var rt = runControl.editor.code.language.runTrigger
                if ( !rt )
                    return ''
                if ( !rt.runnable )
                    return ''
                return rt.runnable.name
            }
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 12
            font.weight: Font.Normal
            elide: Text.ElideRight
            width: Math.min(implicitWidth, 120)
        }
    }

    Rectangle{
        width: 5
        height: parent.height - 2
        color: runControl.topColor
        anchors.left: runnableNameContainer.right
        anchors.leftMargin: 10
        y: 1
    }

    BindingPathView{
        id: bindingPathView
        anchors.left: runnableNameContainer.right
        anchors.leftMargin: 25
        model: runControl.runTrigger && runControl.runTrigger.bindingPath ? runControl.runTrigger.bindingPath : null
        textStyle: runControl.currentTheme ? runControl.currentTheme.inputLabelStyle.textStyle : bindingPathView.textStyle
        iconColor: runControl.currentTheme ? runControl.currentTheme.colorScheme.foregroundFaded : 'black'
        fileIcon: runControl.currentTheme ? runControl.currentTheme.fileIcon : null
        componentIcon: runControl.currentTheme ? runControl.currentTheme.componentIcon : null
        watcherIcon: runControl.currentTheme ? runControl.currentTheme.watcherIcon : null
        stackIcon: runControl.currentTheme ? runControl.currentTheme.stackIcon : null
    }

    Rectangle{
        anchors.right: parent.right
        anchors.rightMargin: 1
        width: 55
        height: parent.height - 2
        color: runControl.topColor
        y: 1

        RunTriggerTypeSelector{
            id: runTypeTriggerSelector
            anchors.left: parent.left
            anchors.leftMargin: 3
            anchors.top: parent.top
            anchors.topMargin: -2
            height: 25
            trigger: runControl.runTrigger ? runControl.runTrigger.triggerType : Document.RunManual
            onTriggerSelected: {
                if ( runControl.editor && runControl.editor.code.language && runControl.editor.code.language.configureRunTrigger ){
                    runControl.editor.code.language.configureRunTrigger({ triggerType: trigger })
                }
            }
        }
    }

    Item{
        anchors.right: parent.right
        anchors.rightMargin: 70
        width: 25
        height: parent.height

        Image{
            anchors.centerIn: parent
            source: "qrc:/images/palette-connections.png"
        }
        MouseArea{
            id: openConnections
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                if  ( !runControl.editor )
                    return

                if ( !connectionViewBox.visible ){
                    connectionList.model = runControl.editor.code.language.runChannels()
                    connectionViewBox.open()
                } else {
                    connectionViewBox.close()
                }
            }
        }
    }

    QC.Popup{
        id: connectionViewBox
        y: 25
        modal: false
        focus: true
        padding: 0
        closePolicy: QC.Popup.CloseOnEscape | QC.Popup.CloseOnPressOutsideParent
        width: parent.width
        height: Math.min(100, connectionList.listHeight + 2)

        contentItem: Rectangle{
            color: runControl.currentTheme ? runControl.currentTheme.colorScheme.backgroundOverlay : 'black'
            border.width: 1
            border.color: runControl.currentTheme ? runControl.currentTheme.colorScheme.backgroundBorder : 'black'

            BindingChannelListView{
                id: connectionList
                anchors.top : parent.top

                width: connectionViewBox.width

                iconColor: runControl.currentTheme ? currentTheme.colorScheme.foregroundFaded: 'black'
                backgroundColor: runControl.currentTheme ? currentTheme.colorScheme.backgroundOverlay : 'black'
                connectionRunnableColor: runControl.currentTheme ? currentTheme.colorScheme.middlegroundOverlay : 'black'
                labelStyle: runControl.currentTheme ? currentTheme.inputLabelStyle.textStyle : null
                selectionColor: runControl.currentTheme ? currentTheme.colorScheme.middleground : 'black'
                fileIcon: runControl.currentTheme ? currentTheme.fileIcon : null
                componentIcon: runControl.currentTheme ? currentTheme.componentIcon : null
                watcherIcon: runControl.currentTheme ? currentTheme.watcherIcon : null
                stackIcon: runControl.currentTheme ? currentTheme.stackIcon : null
            }
        }
        onClosed: {
            if ( runControl.editor && runControl.editor.code.language ){
                runControl.editor.code.language.configureRunTrigger({
                    channel: { model: connectionList.model, index: connectionList.model.selectedIndex }
                })
            }
        }
    }

}

