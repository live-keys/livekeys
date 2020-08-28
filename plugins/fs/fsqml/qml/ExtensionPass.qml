import QtQuick 2.3
import base 1.0

Act{
    property string file: ''
    property var extensions: []

    run: function(file, extensions){
        if ( file.length && extensions.length ){
            return file
        }
        return ''
    }

    onComplete: exec()

    args: ["$file", "$extensions"]

}
