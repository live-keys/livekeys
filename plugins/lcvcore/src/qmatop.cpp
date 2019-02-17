#include "qmatop.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

namespace helpers{

template<typename T> int assignValues(const QVariantList& vmat, cv::Mat* m, QMat::Type type){
    int numberCols = 0;
    for( int i = 0; i < vmat.size(); ++i ){
        if ( !vmat[i].canConvert(QMetaType::QVariantList) ){
            qWarning("Open CV Error: Values must me stored in a 2D array.");
            m->create(0, 0, CV_MAKETYPE(type, 1));
            return 1;
        }
        QVariantList row = vmat[i].toList();
        if ( i == 0 ){
            numberCols = row.size();
            m->create(vmat.size(), numberCols, CV_MAKETYPE(type, 1));
            m->setTo(cv::Scalar(0));
        }
        T* mdata = m->ptr<T>(i);
        for( int j = 0; j < numberCols; ++j ){
            if ( j < row.size() )
                mdata[j] = row[j].value<T>();
            else
                mdata[j] = 0;
        }
    }
    return 0;
}


template<typename T, typename LT> void getValues( cv::Mat& m, QVariantList& output){
    T* data  = reinterpret_cast<T*>(m.data);
    int step = (int)(m.step / sizeof(T));

    for ( int i = 0; i < m.rows; ++i ){
        int rowstep = i * step;
        for ( int j = 0; j < m.cols * m.channels(); ++j ){
            output.append(data[rowstep + j]);
        }
    }
}

template<typename T> QMatrix4x4 matrix4x4( cv::Mat& m ){
    T* data  = reinterpret_cast<T*>(m.data);
    int step = (int)(m.step / sizeof(T));

    return QMatrix4x4(
        (float)data[step * 0 + 0], (float)data[step * 0 + 1], (float)data[step * 0 + 2], (float)data[step * 0 + 3],
        (float)data[step * 1 + 0], (float)data[step * 1 + 1], (float)data[step * 1 + 2], (float)data[step * 1 + 3],
        (float)data[step * 2 + 0], (float)data[step * 2 + 1], (float)data[step * 2 + 2], (float)data[step * 2 + 3],
        (float)data[step * 3 + 0], (float)data[step * 3 + 1], (float)data[step * 3 + 2], (float)data[step * 3 + 3]
    );
}

} // namespace

QMatOp::QMatOp(QObject *parent)
    : QObject(parent)
{
}

cv::Scalar QMatOp::toScalar(const QColor &color){
    return cv::Scalar(color.blue(), color.green(), color.red(), color.alpha());
}

cv::Rect QMatOp::toRect(const QRect &r){
    return cv::Rect(r.x(), r.y(), r.width(), r.height());
}

QMat *QMatOp::create(const QSize &size, int type, int channels){
    if ( size.isValid() ){
        return new QMat(new cv::Mat(size.height(), size.width(), CV_MAKETYPE(type, channels)));
    }

    lv::Exception e = CREATE_EXCEPTION(lv::Exception, "MatOp: Invalid size specified.", lv::Exception::toCode("qmlsize"));
    lv::ViewContext::instance().engine()->throwError(&e);
    return nullptr;
}

QMat *QMatOp::createFill(const QSize &size, int type, int channels, const QColor &color){
    if ( size.isValid() ){
        cv::Mat* m = new cv::Mat(size.height(), size.width(), CV_MAKETYPE(type, channels));
        m->setTo(toScalar(color));

        QMat* r = new QMat(m);
        return r;
    }

    lv::Exception e = CREATE_EXCEPTION(lv::Exception, "MatOp: Invalid size specified.", lv::Exception::toCode("qmlsize"));
    lv::ViewContext::instance().engine()->throwError(&e);
    return nullptr;
}

QMat *QMatOp::createFromArray(const QVariantList &a, int t){
    cv::Mat* m = new cv::Mat;
    int resultCode = 0;

    QMat::Type type = static_cast<QMat::Type>(t);

    switch(type){
    case CV_8U :  resultCode = helpers::assignValues<unsigned char>(  a, m, type); break;
    case CV_8S :  resultCode = helpers::assignValues<char>(           a, m, type); break;
    case CV_16U : resultCode = helpers::assignValues<unsigned short>( a, m, type); break;
    case CV_16S : resultCode = helpers::assignValues<short>(          a, m, type); break;
    case CV_32S : resultCode = helpers::assignValues<long>(           a, m, type); break;
    case CV_32F : resultCode = helpers::assignValues<float>(          a, m, type); break;
    case CV_64F : resultCode = helpers::assignValues<double>(         a, m, type); break;
    }
    if ( resultCode > 0 ){
        delete m;
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "MatOp: Values must me stored in a 2D array.", lv::Exception::toCode("qmlarray"));
        lv::ViewContext::instance().engine()->throwError(&e);
        return nullptr;
    }

    return new QMat(m);
}

void QMatOp::fill(QMat *m, const QColor &color){
    m->internal().setTo(toScalar(color));
}

void QMatOp::fillWithMask(QMat *m, const QColor &color, QMat *mask){
    m->internal().setTo(toScalar(color), mask->internal());
}

QMat *QMatOp::reloc(QMat *m){
    return QMat::reloc(m);
}

QMat *QMatOp::crop(QMat *m, const QRect &region){
    QMat* r = new QMat;
    m->internal()(toRect(region)).copyTo(r->internal());
    return r;
}

QMatrix4x4 QMatOp::to4x4Matrix(QMat *m){
    cv::Mat& cvmat = m->internal();

    if ( cvmat.rows == 4 && cvmat.cols == 4 && m->channels() == 1 ){

        switch(cvmat.depth()){
        case CV_8U: return helpers::matrix4x4<uchar>(cvmat);
        case CV_8S: return helpers::matrix4x4<uchar>(cvmat);
        case CV_16U: return helpers::matrix4x4<ushort>(cvmat);
        case CV_16S: return helpers::matrix4x4<short>(cvmat);
        case CV_32S: return helpers::matrix4x4<int>(cvmat);
        case CV_32F: return helpers::matrix4x4<float>(cvmat);
        case CV_64F: return helpers::matrix4x4<double>(cvmat);
        }
    }

    lv::Exception e = CREATE_EXCEPTION(lv::Exception, "MatOp: Matrix not of 4x4 and single channel type.", lv::Exception::toCode("array"));
    lv::ViewContext::instance().engine()->throwError(&e);
    return QMatrix4x4();
}

QVariantList QMatOp::toArray(QMat *m){
    QVariantList values;
    cv::Mat& cvmat = m->internal();
    switch(cvmat.depth()){
    case CV_8U:
        helpers::getValues<uchar, int>(cvmat, values);
        break;
    case CV_8S:
        helpers::getValues<uchar, int>(cvmat, values);
        break;
    case CV_16U:
        helpers::getValues<short, int>(cvmat, values);
        break;
    case CV_16S:
        helpers::getValues<short, int>(cvmat, values);
        break;
    case CV_32S:
        helpers::getValues<int, int>(cvmat, values);
        break;
    case CV_32F:
        helpers::getValues<float, float>(cvmat, values);
        break;
    case CV_64F:
        helpers::getValues<float, qreal>(cvmat, values);
        break;
    }

    return values;
}


