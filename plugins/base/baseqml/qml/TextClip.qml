import QtQuick 2.3
import base 1.0

Act{
    property string text: ''
    property var positions: undefined

    run: function(text, positions){
        if ( positions ){
            return text.substring(positions[0], positions[1])
        }

        return ''
    }

    args: ["$text", "$positions"]

}
