#include "QMatAccess.hpp"
#include <QElapsedTimer>

using namespace cv;

QElapsedTimer timer;

namespace helpers{

template<typename T, typename LT> void cacheValues( Mat& m, QVariantList& output){
    T* data  = reinterpret_cast<T*>(m.data);
    int step = (int)(m.step / sizeof(T));
    for ( int i = 0; i < m.rows; ++i ){
        QList<LT> row;
        int rowstep = i * step;
        for ( int j = 0; j < m.cols * m.channels(); ++j ){
            row.append(data[rowstep + j]);
        }
        output.append(QVariant::fromValue(row));
    }
}

template<typename T, typename LT> void setValues( QVariantList& values, Mat& m){
    T* data  = reinterpret_cast<T*>(m.data);
    int step = (int)(m.step / sizeof(T));

    for ( int i = 0; i < m.rows; ++i ){
        QList<LT> row = values.at(i).value<QList<LT> >();
        int rowstep = i * step;
        for ( int j = 0; j < m.cols * m.channels(); ++j ){
            data[rowstep + j] = static_cast<T>(row.at(j));
        }
    }
}

}// namespace helpers

QMatAccess::QMatAccess(QQuickItem *parent)
    : QQuickItem(parent)
    , m_mat(0){
}

QMatAccess::QMatAccess(QMat *mat, QQuickItem *parent)
    : QQuickItem(parent)
    , m_mat(mat){

    Mat* cvmat = m_mat->cvMat();
    m_dataPtr  = cvmat->data;
    m_channels = cvmat->channels();

}

QMatAccess::~QMatAccess(){
}

void QMatAccess::setUpCache(){
    Mat* cvmat = m_mat->cvMat();

    switch(cvmat->depth()){
    case CV_8U:
        helpers::cacheValues<uchar, int>(*cvmat, m_values);
        break;
    case CV_8S:
        helpers::cacheValues<uchar, int>(*cvmat, m_values);
        break;
    case CV_16U:
        helpers::cacheValues<short, int>(*cvmat, m_values);
        break;
    case CV_16S:
        helpers::cacheValues<short, int>(*cvmat, m_values);
        break;
    case CV_32S:
        helpers::cacheValues<int, int>(*cvmat, m_values);
        break;
    case CV_32F:
        helpers::cacheValues<float, float>(*cvmat, m_values);
        break;
    case CV_64F:
        helpers::cacheValues<float, qreal>(*cvmat, m_values);
        break;
    }
}


QVariantList QMatAccess::values(){
    if ( m_values.empty() )
        setUpCache();
    return m_values;
}

void QMatAccess::setValues(QVariantList values){
    m_values = values;

    Mat* cvmat = m_mat->cvMat();
    switch(cvmat->depth()){
    case CV_8U:
        helpers::setValues<uchar, int>(m_values, *cvmat);
        break;
    case CV_8S:
        helpers::setValues<uchar, int>(m_values, *cvmat);
        break;
    case CV_16U:
        helpers::setValues<short, int>(m_values, *cvmat);
        break;
    case CV_16S:
        helpers::setValues<short, int>(m_values, *cvmat);
        break;
    case CV_32S:
        helpers::setValues<int, int>(m_values, *cvmat);
        break;
    case CV_32F:
        helpers::setValues<float, float>(m_values, *cvmat);
        break;
    case CV_64F:
        helpers::setValues<float, qreal>(m_values, *cvmat);
        break;
    }
}



//QMatAccess::QMatAccess(QObject *parent)
//    : QAbstractListModel(parent)
//    , m_input(0){

//    m_roles[Data] = "name";

//}

//QMatAccess::QMatAccess(QMat *mat, QObject *parent)
//    : QAbstractListModel(parent)
//    , m_input(mat){

//    m_roles[Data] = "name";
//}

//QVariant QMatAccess::data(const QModelIndex &, int ) const{
//    return 120;
//}

//int QMatAccess::rowCount(const QModelIndex &) const{
//    Mat* cvmat = m_input->cvMat();
//    return cvmat->rows * cvmat->cols * cvmat->channels();
//}

//QHash<int, QByteArray> QMatAccess::roleNames() const{
//        return m_roles;
//}
