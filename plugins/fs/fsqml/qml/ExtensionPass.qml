import QtQuick 2.3
import base 1.0
import fs 1.0 as Fs

Act{
    property string file: ''

    property var extensions: []

    run: function(file, extensions){
        if ( file.length && extensions.length ){
            var ext = Fs.Path.suffix(file)
            for ( var i = 0; i < extensions.length; ++i ){
                if ( extensions[i] === ext )
                    return file
            }
        }
        return ''
    }

    onComplete: exec()

    args: ["$file", "$extensions"]

}
