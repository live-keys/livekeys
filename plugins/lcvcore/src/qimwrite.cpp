#include "qimwrite.h"
#include "opencv2/highgui/highgui.hpp"
#include <QJSValueIterator>

QImWrite::QImWrite(QObject *parent)
    : QObject(parent)
{
}

QImWrite::~QImWrite(){
}

bool QImWrite::saveImage(const QString &file, QMat *image){
    if (!image)
        return false;

    try{
        return cv::imwrite(file.toStdString(), *image->cvMat(), m_convertedParams);
    } catch ( cv::Exception& e ){
        qWarning("Failed to save image due to exception: %s", e.what());
        return false;
    }
}

void QImWrite::setParams(const QJSValue &params){
    m_convertedParams.clear();
    if ( params.isObject() ){
        QJSValueIterator paramsIt(params);
        while( paramsIt.hasNext() ){
            paramsIt.next();
            if ( paramsIt.name() == "jpegQuality" ){
                m_convertedParams.push_back(CV_IMWRITE_JPEG_QUALITY);
                m_convertedParams.push_back(paramsIt.value().toInt());
            } else if ( paramsIt.name() == "pngCompression" ){
                m_convertedParams.push_back(CV_IMWRITE_PNG_COMPRESSION);
                m_convertedParams.push_back(paramsIt.value().toInt());
            } else if ( paramsIt.name() == "pxmBinary" ){
                m_convertedParams.push_back(CV_IMWRITE_PXM_BINARY);
                m_convertedParams.push_back(paramsIt.value().toInt());
            }
        }
    }

    m_params = params;
    emit paramsChanged();
}
