import QtQuick 2.3
import base 1.0
import fs 1.0 as Fs

QtObject{

    property string path: ''
    onPathChanged: {
        file = Fs.File.open(path, Fs.File.ReadOnly)
        if ( file === null ){
            throw linkError(new Error("Path does not exist"), this)
        }
        while ( !file.isEof() ){
            console.log("NEW LINE READ")
            __writableStream.push(file.readLine())
        }
    }

    property Stream stream: __writableStream.stream
    property Fs.FileDescriptor file : null

    property WritableStream __writableStream: WritableStream{
    }

}
