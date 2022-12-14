import QtQuick 2.3
import base 1.0 as B
import fs 1.0 as Fs

B.Act{
    property string path: ''

    returns: 'qml/object'
    args: ['$path']
    run: function(path){
        var file = Fs.File.open(path, Fs.File.ReadOnly)
        if ( file === null ){
            throw linkError(new Error("Path does not exist"), this)
        }
        return B.StreamOperations.createProvider(file, (writableStream, file) => {
            if ( !file.isEof() ){
                writableStream.push(file.readLine())
                return true
            } else {
                writableStream.close()
                return false
            }
        })
    }
}
