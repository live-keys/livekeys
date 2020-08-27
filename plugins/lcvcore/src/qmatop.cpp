#include "qmatop.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/qmlerror.h"

#include "opencv2/imgproc.hpp"

#include <QJSValueIterator>

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

void QMatOp::ListOperations::appendItem(lv::QmlObjectList *list, QObject *item)
{
    auto data = list->dataAs<std::vector<QMat*>>();
    data->push_back(qobject_cast<QMat*>(item));
}

int QMatOp::ListOperations::itemCount(lv::QmlObjectList *list)
{
    auto data = list->dataAs<std::vector<QMat*>>();
    return static_cast<int>(data->size());
}

QObject *QMatOp::ListOperations::itemAt(lv::QmlObjectList *list, int idx)
{
    auto data = list->dataAs<std::vector<QMat*>>();
    if (idx >= static_cast<int>(data->size()) || idx < 0) return nullptr;
    return (*data)[static_cast<unsigned>(idx)];
}

void QMatOp::ListOperations::removeItemAt(lv::QmlObjectList *list, int idx)
{
    auto data = list->dataAs<std::vector<QMat*>>();
    if (idx >= static_cast<int>(data->size()) || idx < 0) return;
    data->erase(data->begin()+idx);
}

void QMatOp::ListOperations::clearItems(lv::QmlObjectList *list)
{
    auto data = list->dataAs<std::vector<QMat*>>();
    for (int i = 0; i < static_cast<int>(data->size()); ++i) delete (*data)[i];
    data->clear();
}

lv::QmlObjectList* QMatOp::createMatList(const QJSValue &matArray)
{
    std::vector<QMat*>* data = new std::vector<QMat*>;
    if ( matArray.isArray() ){
        QJSValueIterator it(matArray);
        while ( it.hasNext() ){
            it.next();
            if ( it.name() != "length" ){
                QMat* m = qobject_cast<QMat*>(it.value().toQObject());
                if ( m ){
                    cv::Mat* nmat = new cv::Mat(*m->cvMat());
                    data->push_back(new QMat(nmat));
                } else {
                    data->clear();
                    break;
                }
            }
        }
    }
    return new lv::QmlObjectList(data, &typeid(std::vector<QMat*>),
        &ListOperations::itemCount, &ListOperations::itemAt, &ListOperations::appendItem,
        &ListOperations::removeItemAt, &ListOperations::clearItems, this);
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

QWritableMat *QMatOp::createWritable(const QSize &size, int type, int channels){
    if ( size.isValid() ){
        return new QWritableMat(new cv::Mat(size.height(), size.width(), CV_MAKETYPE(type, channels)));
    }

    lv::Exception e = CREATE_EXCEPTION(lv::Exception, "MatOp: Invalid size specified.", lv::Exception::toCode("qmlsize"));
    lv::ViewContext::instance().engine()->throwError(&e);
    return nullptr;
}

QWritableMat *QMatOp::createWritableFill(const QSize &size, int type, int channels, const QColor &color){
    if ( size.isValid() ){
        cv::Mat* m = new cv::Mat(size.height(), size.width(), CV_MAKETYPE(type, channels));
        m->setTo(toScalar(color));

        return new QWritableMat(m);
    }

    lv::Exception e = CREATE_EXCEPTION(lv::Exception, "MatOp: Invalid size specified.", lv::Exception::toCode("qmlsize"));
    lv::ViewContext::instance().engine()->throwError(&e);
    return nullptr;
}

QWritableMat *QMatOp::createWritableFromMat(QMat *mat){
    cv::Mat* m = new cv::Mat;
    mat->internal().copyTo(*m);
    return new QWritableMat(m);
}

void QMatOp::fill(QMat *m, const QColor &color){
    if ( m )
        m->internal().setTo(toScalar(color));
}

void QMatOp::fillWithMask(QMat *m, const QColor &color, QMat *mask){
    m->internal().setTo(toScalar(color), mask->internal());
}

QMat *QMatOp::reloc(QMat *m){
    return QMat::reloc(m);
}

QMat *QMatOp::crop(QMat *m, const QRect &region){
    if (!m)
        return nullptr;
    QMat* r = new QMat;
    m->internal()(toRect(region)).copyTo(r->internal());
    return r;
}

QMat *QMatOp::flip(QMat *m, int direction){
    QMat* r = new QMat;
    cv::flip(m->internal(), r->internal(), direction);
    return r;
}

QJSValue QMatOp::split(QMat *m){
    if (!m)
        return QJSValue();

    std::vector<cv::Mat> channels;
    cv::split(m->data(), channels);

    quint32 channelsSize = static_cast<quint32>(channels.size());

    lv::ViewEngine* engine = lv::ViewEngine::grab(this);
    QJSValue result = engine->engine()->newArray(channelsSize);

    for ( quint32 i = 0; i < channelsSize; ++i ){
        QMat* current = new QMat;
        *current->cvMat() = channels[i];
        result.setProperty(i, engine->engine()->newQObject(current));
    }
    return result;
}

QMat *QMatOp::merge(const QJSValue &matArray){
    std::vector<cv::Mat> channels;
    QJSValueIterator it(matArray);
    while ( it.hasNext() ){
        it.next();
        if ( it.name() != "length" ){
            QMat* m = qobject_cast<QMat*>(it.value().toQObject());
            if (m){
                channels.push_back(m->data());
            }
        }
    }

    QMat* r = new QMat;
    cv::merge(channels, *r->cvMat());
    return r;
}

QMat *QMatOp::spreadByLinearInterpolation(QJSValue reference, QJSValue spread){
    std::vector<double> referenceVector;
    QJSValueIterator it(reference);
    while ( it.hasNext() ){
        it.next();
        if ( it.name() != "length" )
            referenceVector.push_back(it.value().toNumber());
    }

    std::vector<double> spreadVector;
    QJSValueIterator it2(spread);
    while ( it2.hasNext() ){
        it2.next();
        if ( it2.name() != "length" )
            spreadVector.push_back(it2.value().toNumber());
    }

    if ( referenceVector.size() != spreadVector.size() ){
        THROW_QMLERROR(lv::Exception, "MatOp: reference and spread must be the same size.", lv::Exception::toCode("~ArraySize"), this);
        return nullptr;
    }

    QMat* r = new QMat(new cv::Mat(1, 256, CV_8U));
    uchar* lut = r->cvMat()->ptr();

    for(int i = 0; i< 256; i++){
        int j = 0;
        double xval = static_cast<double>(i);
        while(xval > referenceVector[static_cast<size_t>(j)]){
            j++;
        }
        if(xval == referenceVector[static_cast<size_t>(j)]){
            lut[i] = spreadVector[static_cast<size_t>(j)];
            continue;
        }
        double slope    = static_cast<double>(spreadVector[j] - spreadVector[j - 1]) / static_cast<double>(referenceVector[j] - referenceVector[j-1]);
        double constant = spreadVector[j] - slope * referenceVector[j];
        lut[i] = slope * xval + constant;
    }

    return r;
}

QMat *QMatOp::lut(QMat *m, QMat *lut){
    if ( !m || !lut )
        return nullptr;

    QMat* r = new QMat;

    try {
        cv::LUT(m->data(), lut->data(), *r->cvMat());
        cv::min(r->data(), 255, *r->cvMat());
        cv::max(r->data(), 0, *r->cvMat());
    } catch (cv::Exception& e) {
        THROW_QMLERROR(lv::Exception, e.what(), static_cast<lv::Exception::Code>(e.code), this);
        delete r;
        return nullptr;
    }

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

QMat *QMatOp::bitwiseXor(QMat *arg1, QMat *arg2)
{
    cv::Mat* a1 = arg1->cvMat();
    cv::Mat* a2 = arg2->cvMat();

    if (a1->rows != a2->rows || a1->cols != a2->cols || a1->type() != a2->type()) return nullptr;

    cv::Mat* resMat = new cv::Mat(a1->rows, a1->cols, a1->type());
    cv::bitwise_xor(*a1, *a2, *resMat);

    QMat* result = new QMat(resMat);
    return result;


}

QMat *QMatOp::bitwiseOr(QMat *arg1, QMat *arg2)
{
    cv::Mat* a1 = arg1->cvMat();
    cv::Mat* a2 = arg2->cvMat();

    if (a1->rows != a2->rows || a1->cols != a2->cols || a1->type() != a2->type()) return nullptr;

    cv::Mat* resMat = new cv::Mat(a1->rows, a1->cols, a1->type());
    cv::bitwise_or(*a1, *a2, *resMat);

    QMat* result = new QMat(resMat);
    return result;
}

QMat *QMatOp::bitwiseAnd(QMat *arg1, QMat *arg2)
{
    cv::Mat* a1 = arg1->cvMat();
    cv::Mat* a2 = arg2->cvMat();

    if (a1->rows != a2->rows || a1->cols != a2->cols || a1->type() != a2->type()) return nullptr;

    cv::Mat* resMat = new cv::Mat(a1->rows, a1->cols, a1->type());
    cv::bitwise_and(*a1, *a2, *resMat);

    QMat* result = new QMat(resMat);
    return result;
}

QMat *QMatOp::bitwiseNot(QMat *arg)
{
    cv::Mat* a1 = arg->cvMat();

    cv::Mat* resMat = new cv::Mat(a1->rows, a1->cols, a1->type());
    cv::bitwise_not(*a1, *resMat);

    QMat* result = new QMat(resMat);
    return result;
}


