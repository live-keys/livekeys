#ifndef QIMAGEVIEW_H
#define QIMAGEVIEW_H

#include <QQuickItem>
#include "qmat.h"

/// \private
class QImageView : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* image       READ image        WRITE setImage        NOTIFY imageChanged)
    Q_PROPERTY(bool linearFilter READ linearFilter WRITE setLinearFilter NOTIFY linearFilterChanged)

public:
    explicit QImageView(QQuickItem* parent = nullptr);
    ~QImageView();

    QMat* image();
    void setImage(QMat* image);

    bool linearFilter() const;
    void setLinearFilter(bool filter);

signals:
    void imageChanged();
    void linearFilterChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

private:
    QMat* m_mat;
    bool  m_linearFilter;
};

inline QMat *QImageView::image(){
    return m_mat;
}

inline void QImageView::setImage(QMat *arg){
    if ( arg == nullptr )
        return;

    cv::Mat* matData = arg->cvMat();
    if ( static_cast<int>(implicitWidth()) != matData->cols || static_cast<int>(implicitHeight()) != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }
    m_mat = arg;

    emit imageChanged();
    update();
}

inline bool QImageView::linearFilter() const{
    return m_linearFilter;
}

inline void QImageView::setLinearFilter(bool filter){
    if ( filter != m_linearFilter ){
        m_linearFilter = filter;
        emit linearFilterChanged();
        update();
    }
}

#endif // QIMAGEVIEW_H
