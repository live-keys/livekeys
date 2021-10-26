import QtQuick 2.3
import base 1.0
import fs 1.0 as Fs

Act{
    property string path: ''
    property var extensions: ['jpg', 'png', 'bmp', 'jpeg']

    args: ['$path', '$extensions']
    run: function(path, extensions){
        var ext = extensions
        if ( !path || !ext ){
            return []
        } else {
            return Fs.Dir.listDetail(path)
                .filter(f => !f.isDir)
                .map(f => path + '/' + f.name)
                .filter(f => ext.includes(Fs.Path.suffix(f)))
        }
    }
}
