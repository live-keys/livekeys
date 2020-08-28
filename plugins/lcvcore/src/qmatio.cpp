#include "qmatio.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include <QJSValueIterator>

QMatIO::QMatIO(QObject *parent)
    : QObject(parent)
{
}

QMat *QMatIO::read(const QString &path, int isColor){
    if ( path.isEmpty() )
        return nullptr;

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

bool QMatIO::write(const QString &file, QMat *image, QJSValue options){
    if ( !image ){
        qWarning("MatIO.write: Null image provided");
        return false;
    }

    std::vector<int> params;

    if ( options.isObject() ){
        QJSValueIterator optionsIt(options);
        while( optionsIt.hasNext() ){
            optionsIt.next();
            if ( optionsIt.name() == "jpegQuality" ){
                params.push_back(cv::IMWRITE_JPEG_QUALITY);
                params.push_back(optionsIt.value().toInt());
            } else if ( optionsIt.name() == "pngCompression" ){
                params.push_back(cv::IMWRITE_PNG_COMPRESSION);
                params.push_back(optionsIt.value().toInt());
            } else if ( optionsIt.name() == "pxmBinary" ){
                params.push_back(cv::IMWRITE_PXM_BINARY);
                params.push_back(optionsIt.value().toInt());
            }
        }
    }

    try{
        cv::imwrite(file.toStdString(), *image->cvMat(), params);
    } catch ( cv::Exception& e ){
        qWarning("Failed to save image due to exception: %s", e.what());
        return false;
    }

    return true;
}
