#include "QMat2DArray.hpp"

using namespace cv;

template<typename T> Mat* assignValuesHelper(const QVariantList& vmat, QMat*, QMat::Type type){
    int numberCols = 0;
    Mat* writeMat;
    for( int i = 0; i < vmat.size(); ++i ){
        if ( !vmat[i].canConvert(QMetaType::QVariantList) ){
            qDebug() << "Opencv error : values must me stored in a 2D array.";
            //*m->data() = cv::Mat(0, 0, CV_MAKETYPE(type, 1));
            return 0;
        }
        QVariantList row = vmat[i].toList();
        if ( i == 0 ){
            numberCols = row.size();
            writeMat = new Mat(vmat.size(), numberCols, CV_MAKETYPE(type, 1), cv::Scalar(0));
        }
        T* mdata = writeMat->ptr<T>(i);
        for( int j = 0; j < numberCols; ++j ){
            if ( j < row.size() )
                mdata[j] = row[j].value<T>();
            else
                mdata[j] = 0;
        }
    }
    return writeMat;
}

QMat2DArray::QMat2DArray(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_type(QMat::CV8U)
{
}

QMat2DArray::~QMat2DArray(){
}

cv::Size QMat2DArray::assignValues(){
    Mat* base = 0;
    switch(m_type){
    case CV_8U :  base = assignValuesHelper<unsigned char>(m_values, output(), m_type);
    case CV_8S :  base = assignValuesHelper<char>(         m_values, output(), m_type);
    case CV_16U : base = assignValuesHelper<unsigned int>( m_values, output(), m_type);
    case CV_16S : base = assignValuesHelper<int>(          m_values, output(), m_type);
    case CV_32S : base = assignValuesHelper<long>(         m_values, output(), m_type);
    case CV_32F : base = assignValuesHelper<float>(        m_values, output(), m_type);
    case CV_64F : base = assignValuesHelper<double>(       m_values, output(), m_type);
    }
    if ( base != 0 ){
        //*output()->data() = *base;
        //m_cachedMat = new QMat(base);
        //*m->data() = *writeMat;
        return base->size();
    }
    return cv::Size(0, 0);
}
