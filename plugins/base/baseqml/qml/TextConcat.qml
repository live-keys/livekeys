import QtQuick 2.3
import base 1.0

Act{
    property string text1: ''
    property string text2: ''

    run: function(text1, text2){
        return text1 + text2
    }
    args: ['$text1', '$text2']
}
