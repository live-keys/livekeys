import QtQuick 2.3

MessageDialogInternal{
    id: root

    function show(
        message,
        options
    ){
        var defaults = {
            button1Name : '',
            button1Function : null,
            button2Name : '',
            button2Function : null,
            button3Name : '',
            button3Function : null,
            returnPressed : function(){ root.close(); },
            escapePressed : function(){ root.close(); }
        }
        for ( var i in defaults )
            if ( typeof options[i] == 'undefined' )
                options[i] = defaults[i]

        if ( options.button1Name !== '' ){
            messageBoxButton1.text     = options.button1Name
            messageBoxButton1.callback = options.button1Function
        }
        if ( options.button2Name !== '' ){
            messageBoxButton2.text     = options.button2Name
            messageBoxButton2.callback = options.button2Function
        }
        if ( options.button3Name !== '' ){
            messageBoxButton3.text     = options.button3Name
            messageBoxButton3.callback = options.button3Function
        }

        root.message = message
        root.returnPressed = options.returnPressed
        root.escapePressed = options.escapePressed

        root.visible = true
        root.forceActiveFocus()
    }

    function close(){
        root.message = ''
        root.visible = false
        messageBoxButton1.text = ''
        messageBoxButton2.text = ''
        messageBoxButton3.text = ''
        root.destroy()

        var aitem = livecv.layers.workspace.panes.activePane
        if ( activePane )
            activePane.forceActiveFocus()
    }

    MessageDialogButton{
        id: messageBoxButton1
        visible : text !== ''
        onClicked: messageBoxButton1.callback(root)
    }

    MessageDialogButton{
        id: messageBoxButton2
        anchors.centerIn: parent
        visible : text !== ''
        onClicked: messageBoxButton2.callback(root)
    }

    MessageDialogButton{
        id: messageBoxButton3
        anchors.right: parent.right
        visible : text !== ''
        onClicked: messageBoxButton3.callback(root)
    }
}
