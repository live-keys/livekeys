import QtQuick 2.0
import lcvcore 1.0
import lcvimgproc 1.0

CvtColor{
    id : grayscale
    input : null
    code : CvtColor.CV_BGR2GRAY
    dstCn : 1
}
