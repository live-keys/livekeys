#ifndef QMAT2DARRAY_HPP
#define QMAT2DARRAY_HPP

#include "QMatDisplay.hpp"
#include <QList>

class QMat2DArray : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QVariantList values READ values WRITE setValues NOTIFY valuesChanged)
    Q_PROPERTY(QMat::Type   type   READ type   WRITE setType   NOTIFY typeChanged)

public:
    explicit QMat2DArray(QQuickItem *parent = 0);
    ~QMat2DArray();

    const QVariantList& values() const;
    void setValues( const QVariantList& val );

    QMat::Type type() const;
    void setType(QMat::Type type);

    cv::Size assignValues();

signals:
    void valuesChanged();
    void typeChanged();

private:
    QMat::Type   m_type;
    QVariantList m_values;

    cv::Size     m_cachedSize;

};

inline const QVariantList &QMat2DArray::values() const{
    return m_values;
}

inline void QMat2DArray::setValues(const QVariantList &val){
    if ( val != m_values ){
        m_values     = val;
        m_cachedSize = assignValues();
    }
}

inline QMat::Type QMat2DArray::type() const{
    return m_type;
}

inline void QMat2DArray::setType(QMat::Type type){
    if ( m_type == type ){
        m_type = type;
        emit typeChanged();
        if ( m_cachedSize != cv::Size(0,0) ){
            output()->data()->create(m_cachedSize, CV_MAKETYPE(m_type, 1));
            assignValues();
            emit outChanged();
            update();
        }
    }
}

#endif // QMAT2DARRAY_HPP
