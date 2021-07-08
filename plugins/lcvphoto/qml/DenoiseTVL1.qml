import QtQuick 2.0
import base 1.0
import lcvphoto 1.0 as Photo

Act{

    property var obs: []
    property double lambda: 1.0
    property int iters: 30

    run: Photo.Denoising.denoiseTVL1
    onComplete: exec()

    args: ["$obs", "$lambda", "$iters"]
}
