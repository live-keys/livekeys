import QtQuick 2.3
import base 1.0
import lcvcore 1.0

Grid{

    Act{
        id: videoDecoder

        property VideoDecoder decoder : VideoDecoder{}

        onComplete: {
            decoder.run(project.path("../../../samples/_videos/amherst-11_2754_3754.avi")).forward([decoderView, 'mat'])
        }
    }

    MatView{
        id: decoderView
    }
}

