import QtQuick 2.3
import base 1.0
import lcvcore 1.0

ImageView{
    id: root

    property VideoDecoder decoder : VideoDecoder{
        loop: true
    }
    property string file: ''
    onFileChanged: {
        if ( file.length > 0 )
            decoder.run(file).forward([root, 'image'])
    }
}

