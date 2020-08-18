import QtQuick 2.3
import base 1.0

Act{
    property string text: ''
    property var position: 0

    run: function(text, position){
        if ( position >= 0 ){
            var perLine = text.split('\n');
            var totalLength = 0;
            for (var i = 0; i < perLine.length; i++) {
                totalLength += perLine[i].length + 1;
                if (totalLength >= position)
                    return [
                        totalLength - perLine[i].length,
                        totalLength,
                        i + 1
                    ]
            }
        }
        return undefined
    }
    args: ["$text", "$position"]

}
