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

        var aitem = lk.layers.workspace.panes.activePane
        if ( aitem )
            aitem.forceActiveFocus()
    }

    property Component buttonStyle: Rectangle{
        id: dialogButton

        property alias text: buttonLabel.text
        property var callback : function(){}

        signal clicked()
        onClicked: parent.clicked()

        color: buttonMouseArea.containsMouse ? "#11212c" : "#0b151c"
        width: 100
        height: 30

        Text{
            id: buttonLabel
            anchors.centerIn: parent
            text: ""
            color: "#ccc"
            font.pixelSize: 12
            font.family: "Open Sans, sans-serif"
            font.weight: Font.Light
        }
        MouseArea{
            id: buttonMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                dialogButton.clicked()
            }
        }
    }

    Loader{
        id: messageBoxButton1

        property string text: ''
        property var callback: function(){}
        signal clicked()

        visible : text !== ''
        sourceComponent: root.buttonStyle
        onItemChanged: {
            if ( item )
                item.text = Qt.binding(function(){ return messageBoxButton1.text } )
        }
        onClicked: messageBoxButton1.callback(root)
    }

    Loader{
        id: messageBoxButton2

        property string text: ''
        property var callback: function(){}
        signal clicked()

        anchors.horizontalCenter: parent.horizontalCenter
        visible : text !== ''
        sourceComponent: root.buttonStyle
        onItemChanged: {
            if ( item )
                item.text = Qt.binding(function(){ return messageBoxButton2.text } )
        }
        onClicked: messageBoxButton2.callback(root)
    }

    Loader{
        id: messageBoxButton3

        property string text: ''
        property var callback: function(){}
        signal clicked()

        anchors.right: parent.right
        visible : text !== ''
        sourceComponent: root.buttonStyle
        onItemChanged: {
            if ( item )
                item.text = Qt.binding(function(){ return messageBoxButton3.text } )
        }
        onClicked: messageBoxButton3.callback(root)
    }
}
