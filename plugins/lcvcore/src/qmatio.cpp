#include "qmatio.h"
#include "opencv2/highgui.hpp"

QMatIO::QMatIO(QObject *parent)
    : QObject(parent)
{
}

QMat *QMatIO::read(const QString &path, int isColor){
    cv::Mat image = cv::imread(path.toStdString(), isColor);

    QMat* m = new QMat;
    m->internal() = image;

    return m;
}
