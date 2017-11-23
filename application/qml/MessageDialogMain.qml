import QtQuick 2.3

MessageDialogInternal {

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
            returnPressed : function(){ messageBox.close(); },
            escapePressed : function(){ messageBox.close(); }
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

        messageBox.message = message
        messageBox.returnPressed = options.returnPressed
        messageBox.escapePressed = options.escapePressed

        messageBox.visible = true
        messageBox.forceActiveFocus()
    }
    function close(){
        messageBox.message = ''
        messageBox.visible = false
        messageBoxButton1.text = ''
        messageBoxButton2.text = ''
        messageBoxButton3.text = ''
        editor.forceFocus()
    }

    MessageDialogButton{
        id: messageBoxButton1
        visible : text !== ''
    }

    MessageDialogButton{
        id: messageBoxButton2
        anchors.centerIn: parent
        visible : text !== ''
    }

    MessageDialogButton{
        id: messageBoxButton3
        anchors.right: parent.right
        visible : text !== ''
    }
}
