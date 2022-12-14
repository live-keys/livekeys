import QtQuick 2.3
import base 1.0
import fs 1.0 as Fs

Act{
    property string path: ''
    property int flags: 0

    returns: 'qml/object'
    args: ['$path', '$flags']

    run: function(path, flags){
        return Fs.Dir.read(path, flags)
    }

}
