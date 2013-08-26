import QtQuick 2.0

TextEdit {
    id : editor
    focus : false
    font.family: "Courier New"
    font.pixelSize: 13
    color : "#fff"
    selectByMouse: true
    mouseSelectionMode: TextEdit.SelectCharacters
    selectionColor: "#3d4856"
    textFormat: TextEdit.PlainText
    text : ""
    wrapMode: TextEdit.NoWrap
    onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
    Keys.onPressed: {
        if ( event.key === Qt.Key_BracketRight || event.key === Qt.Key_BraceRight ){
            event.accepted = true
            if ( cursorPosition > 4 ){
                var clastpos = cursorPosition
                if( editor.text.substring(cursorPosition - 4, cursorPosition) === "    " ){
                    editor.text = editor.text.slice(0, clastpos - 4) + "}" + editor.text.slice(clastpos)
                    cursorPosition = clastpos - 3
                } else {
                    editor.text = editor.text.slice(0, clastpos) + "}" + editor.text.slice(clastpos)
                    cursorPosition = clastpos + 1
                }
            }
        } else
            event.accepted = false
    }
    Keys.onReturnPressed: {
        event.accepted = true
        var clastpos = cursorPosition
        var append = ""
        var cpos = cursorPosition - 1
        var bracketSearchEnd = false
        while ( cpos > 0 ){
            if ( !bracketSearchEnd ){
                if ( editor.text.charAt(cpos) !== ' ' ){
                    if ( editor.text.charAt(cpos) === '{' ){
                        append += "    "
                        bracketSearchEnd = true
                    } else {
                        bracketSearchEnd = true
                    }
                }
            }// bracket search
            if ( editor.text.charAt(cpos) === '\n' ){
                ++cpos;
                while( editor.text.charAt(cpos) === '\t' || editor.text.charAt(cpos) === ' ' ){
                    append += editor.text.charAt(cpos)
                    ++cpos
                }
                break;
            }
            --cpos;
        }
        editor.text = editor.text.slice(0, clastpos) + "\n" + append + editor.text.slice(clastpos)
        editor.cursorPosition = clastpos + 1 + append.length
    }
    Keys.onTabPressed: {
        event.accepted = true
        var clastpos = cursorPosition
        editor.text = editor.text.slice(0, clastpos) + "    " + editor.text.slice(clastpos)
        editor.cursorPosition = clastpos + 4
    }
}
