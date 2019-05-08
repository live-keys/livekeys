import QtQuick 2.3

Rectangle{
    id : root
    height : error.text !== '' ? error.height + 20 : 0

    property alias text: error.text
    property alias font: error.font

    function wrapMessage(errors){
        var lastErrorsText = ''
        var lastErrorsLog  = ''
        for ( var i = 0; errors && i < errors.length; ++i ){
            var lerror = errors[i]
            var errorFile = lerror.fileName
            var index = errorFile.lastIndexOf('/')
            if ( index !== -1 && index < errorFile.length - 1)
                errorFile = errorFile.substring(index + 1)

            lastErrorsText +=
                (i !== 0 ? '<br>' : '') +
                '<a href=\"' + lerror.fileName + ':' + lerror.lineNumber + '\">' +
                    errorFile + ':' + lerror.lineNumber +
                '</a>' +
                ' ' + lerror.message
            lastErrorsLog +=
                (lastErrorsLog === '' ? '' : '\n') +
                'Error: ' + lerror.fileName + ':' + lerror.lineNumber + ' ' + lerror.message
        }
        return {
            rich: lastErrorsText,
            log: lastErrorsLog
        }
    }

    Behavior on height {
        SpringAnimation { spring: 3; damping: 0.1 }
    }

    Rectangle{
        width : 14
        height : parent.height
        color : "#601818"
        visible: error.visible
    }
    Text {
        id: error
        anchors.left : parent.left
        anchors.leftMargin: 25
        anchors.verticalCenter: parent.verticalCenter

        width: parent.width
        wrapMode: Text.Wrap
        textFormat: Text.StyledText

        linkColor: "#c5d0d7"
        font.family: "Ubuntu Mono, Courier New, Courier"
        text: ''
        color: "#c5d0d7"
        visible : text === "" ? false : true

        onLinkActivated: {
            project.openFile(link.substring(0, link.lastIndexOf(':')), ProjectDocument.EditIfNotOpen)
        }
    }
}
