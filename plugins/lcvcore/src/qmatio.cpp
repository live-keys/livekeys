#include "qmatio.h"
#include "opencv2/core.hpp"
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

QMat *QMatIO::decode(const QByteArray &bytes, int isColor){
    cv::Mat minput(1, bytes.length(), CV_8UC1, (void*)(bytes.data()));
    cv::Mat image = cv::imdecode(minput, isColor);

    QMat* m = new QMat;
    m->internal() = image;

    return m;
}
