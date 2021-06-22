#include "qmatio.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "live/exception.h"
#include "live/viewengine.h"

#include <QJSValueIterator>
#include <QImage>

QMatIO::QMatIO(QObject *parent)
    : QObject(parent)
{
}

cv::Mat QMatIO::fromQImage(const QImage &inImage, bool cloneImageData){
    switch ( inImage.format() ){
        // 8-bit, 4 channel
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied: {
            cv::Mat  mat( inImage.height(), inImage.width(),
                          CV_8UC4,
                          const_cast<uchar*>(inImage.bits()),
                          static_cast<size_t>(inImage.bytesPerLine())
                          );

            return (cloneImageData ? mat.clone() : mat);
        }

        // 8-bit, 3 channel
        case QImage::Format_RGB32: {
            if ( !cloneImageData ){
                THROW_EXCEPTION(lv::Exception, "Conversion requires cloning on RGB32.", lv::Exception::toCode("~Clone"));
            }

            cv::Mat  mat( inImage.height(), inImage.width(),
                          CV_8UC4,
                          const_cast<uchar*>(inImage.bits()),
                          static_cast<size_t>(inImage.bytesPerLine())
                          );

            cv::Mat  matNoAlpha;

            cv::cvtColor( mat, matNoAlpha, cv::COLOR_BGRA2BGR );   // drop the all-white alpha channel

            return matNoAlpha;
        }

        // 8-bit, 3 channel
        case QImage::Format_RGB888: {
            if ( !cloneImageData ){
                THROW_EXCEPTION(lv::Exception, "Conversion requires cloning on RGB32.", lv::Exception::toCode("~Clone"));
            }

            QImage swapped = inImage.rgbSwapped();

            return cv::Mat( swapped.height(), swapped.width(),
                            CV_8UC3,
                            const_cast<uchar*>(swapped.bits()),
                            static_cast<size_t>(swapped.bytesPerLine())
                            ).clone();
        }

        // 8-bit, 1 channel
        case QImage::Format_Indexed8: {
            cv::Mat  mat( inImage.height(), inImage.width(),
                          CV_8UC1,
                          const_cast<uchar*>(inImage.bits()),
                          static_cast<size_t>(inImage.bytesPerLine())
                          );

            return (cloneImageData ? mat.clone() : mat);
        }

        default:{
            THROW_EXCEPTION(lv::Exception, "Conversion requires cloning on RGB32.", lv::Exception::toCode("~Clone"));
        }
    }

    return cv::Mat();
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
        cv::imwrite(file.toStdString(), *image->internalPtr(), params);
    } catch ( cv::Exception& e ){
        qWarning("Failed to save image due to exception: %s", e.what());
        return false;
    }

    return true;
}
